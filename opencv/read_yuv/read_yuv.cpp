#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include<iostream>
#include <unistd.h>
#include <stdio.h>
#include <image_transport/image_transport.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	cout<<"Read a image"<<endl;
	namedWindow("Show");
	
	FILE *fp = fopen("yuv.data", "r");
	fseek(fp,0,SEEK_END);
	int length=ftell(fp);
	rewind(fp);
	unsigned char *buffer = new unsigned char[length];
	fread(buffer, 0, length, fp);
	Mat mat(480, 640, CV_8UC2, (unsigned char*)buffer);
	//mat.data << buffer;
    Mat resultM;
    cvtColor(mat, resultM, CV_YUV2BGR_YUYV);
	imshow("Show", resultM);
	waitKey(6000);
	destroyWindow("Show");
	
	return 0;
}
