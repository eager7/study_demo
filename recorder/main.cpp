#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>

enum TVideoFormat{		
	KYUV420Planar,
	KYUV420SemiPlanar,
	KBmpRGB,
	KBmpBGR,
	KYUV422_YUYV,
	KYUV422_YVYU,
	KYUV444,
	KBmpBGRA,
	KBmpRGBA,
	KGray,
	KBmpYYY,
	KYUV422_VYUY,
	KYUV422_UYVY,
	KYUV444Planar
};

typedef struct ESMHeader {
	char pFileStr[15];                            // .esm identifier
	unsigned char nVersionMajor, nVersionMinor;   // .esm version number
	unsigned char nBPP;                           // bytes per pixel
	unsigned int nWidth, nHeight;                 // frames dimensions
	unsigned int nFrameSize;                      // frame buffer size. if 0, then size is nWidth*nHeight*nBPP
	                                              // otherwise, use given size.
	unsigned char nInputType;                     // from TVideoFormat enum
	unsigned char nRotation;                      // from TRotation enum
	unsigned char bFlipped;                       // 1 for flipped, 0 otherwise
	unsigned char bMirrored;                      // 1 for mirrored, 0 otherwise
	unsigned char nAvgFPS;                        // average FPS
	unsigned int nFramesCount;                    // number of video frames
	unsigned char nPersonCount;                   // allocated number of persons per frame
	unsigned char nPartsCount;                    // allocated number of parts per person
	unsigned char bHasAcceleromterData;           // acceleration on x,y,z axes for each frame

	// additional data sizes (in bytes)
	unsigned int nVideoAdditionalDataSize;
	unsigned int nFrameAdditionalDataSize;
	unsigned int nPersonAdditionalDataSize;
	unsigned int nPartAdditionalDataSize;
}ESMHeader;

struct buffer {
	void   *start;
	size_t length;
};

static void xioctl(int fh, int request, void *arg) {
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

	if (r == -1) {
		fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void show_usage()
{
    printf("Video recorder. Usage:\n");
    printf("    eyesight_recorder <output_file> <length>\n");
    printf("    eyesight_recorder --yuv <output_file> <length>\n\n");
    printf("    output_file - name of output file\n");
    printf("    length      - recording length [in seconds; max=60]\n");
    printf("    --yuv       - [optional] Force YUV format; otherwise ESM is used.\n\n");
}

int main(int argc, char *argv[]) {
	struct v4l2_format fmt;
	struct v4l2_buffer buf;
	struct v4l2_requestbuffers req;
	enum v4l2_buf_type type;
	fd_set fds;
	struct timeval tv;
	int r, fd = -1;
	unsigned int i, n_buffers;
	struct buffer *buffers;
	
    bool bESM = true;
    char * outputFile = NULL;
    unsigned int length = 0;
    
    FILE * fOut;

    /* Parse command line arguments */
    int indx = 1;
    if ((argc != 3) && (argc != 4))
    {
        show_usage();
        return 0;
    }
    if (argc == 4)
    {
        if (0 == strcmp(argv[indx++], "--yuv"))
        {
            bESM = false;
        } else
        {
            show_usage();
            return 0;
        }
    }
    
    outputFile = argv[indx++];
    length = strtoul(argv[indx], NULL, 0);
    if (0 == length)
    {
        show_usage();
        return 0;
    }
    if (length > 60)
    {
        printf("Error: length is too big (should be between 0 and 60)\n");
        show_usage();
        return 0;
    }
    
    printf("Saving %d seconds to %s [", length, outputFile);
    if (bESM)
        printf("ESM Format]\n");
    else
        printf("YUV Format]\n");

    /* Convert to frames */
    length = length * 30;
    
    fOut = fopen(outputFile, "wb");
    if (NULL == fOut)
    {
        printf("Error: Unable to create file %s\n", outputFile);
        return -1;
    }
    
    /* If ESM, write header */
    if (bESM)
    {
        ESMHeader header;
	    memcpy(header.pFileStr, "ES_MOVIE_HEADER", 15);
	    header.nVersionMajor = 1;
        header.nVersionMinor = 0;
	    header.nBPP = 16;
	    header.nWidth = 640;
        header.nHeight = 480;
	    header.nFrameSize = 640*480*2;
	    header.nInputType = KYUV422_YUYV;
	    header.nRotation = 0;
	    header.bFlipped = 0;
	    header.bMirrored = 0;
	    header.nAvgFPS = 30;
	    header.nFramesCount = length;
	    header.nPersonCount = 0;
	    header.nPartsCount = 0;
	    header.bHasAcceleromterData = 0;
        header.nVideoAdditionalDataSize = 0;
	    header.nFrameAdditionalDataSize = 0;
	    header.nPersonAdditionalDataSize = 0;
        header.nPartAdditionalDataSize = 0;
        fwrite(&header, 1, sizeof(ESMHeader), fOut);    
    }
    
    /* Open Video Device */   
	fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
	if (fd < 0) {
		perror("Cannot open device");
		exit(EXIT_FAILURE);
	}

	struct v4l2_control ctrl;
	ctrl.id = V4L2_CID_POWER_LINE_FREQUENCY;
	ctrl.value = V4L2_CID_POWER_LINE_FREQUENCY_50HZ;
	xioctl(fd, VIDIOC_S_CTRL, &ctrl);

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 640;
	fmt.fmt.pix.height = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	xioctl(fd, VIDIOC_S_FMT, &fmt);
	if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
		printf("Libv4l didn't accept Pixel format. Can't proceed.\n");
		exit(EXIT_FAILURE);
	}
	if ((fmt.fmt.pix.width != 640)
			|| (fmt.fmt.pix.height != 480)) {
		printf("Warning: driver is sending image at %dx%d\n", fmt.fmt.pix.width,
				fmt.fmt.pix.height);
	}
	memset(&req, 0, sizeof(req));
	req.count = 10;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	xioctl(fd, VIDIOC_REQBUFS, &req);

	buffers = (struct buffer *) calloc(req.count, sizeof(buffer));
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		memset(&buf, 0, sizeof(buf));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		xioctl(fd, VIDIOC_QUERYBUF, &buf);

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL, buf.length,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start) {
			perror("mmap");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < n_buffers; ++i) {
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		xioctl(fd, VIDIOC_QBUF, &buf);
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	xioctl(fd, VIDIOC_STREAMON, &type);

    int progress = 0;
	while (length > 0) {
		/* Wait for video frames */
		do {
			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select(fd + 1, &fds, NULL, NULL, &tv);
		} while ((r == -1 && (errno = EINTR)));
		if (r == -1) {
			perror("select");
			return errno;
		}

		//memset(&buf, 0, sizeof(buf));
		//buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		//buf.memory = V4L2_MEMORY_MMAP;
		xioctl(fd, VIDIOC_DQBUF, &buf);

        // Dump frame to file 
        fwrite((unsigned char*)buffers[buf.index].start, 1, buf.bytesused, fOut);

		xioctl(fd, VIDIOC_QBUF, &buf);
		
		progress++;
		if (progress == 30)
		{
		    printf(".");
		    fflush(stdout);
		    progress=0;
		}
		
		length--;
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	xioctl(fd, VIDIOC_STREAMOFF, &type);
	for (i = 0; i < n_buffers; ++i)
		munmap(buffers[i].start, buffers[i].length);
	free(buffers);
	close(fd);
	fclose(fOut);
	
	printf("\nDone.\n");
	
	return 0;
}
