#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <sstream>
#include<iostream>
#include <unistd.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	cout<<"Read a image"<<endl;
    CvCapture* capture = cvCreateCameraCapture(0);
    IplImage* frame;
	while(1){
		frame = cvQueryFrame(capture);
		//if(!frame)break;
		cvShowImage("Camera", frame);
		char c = cvWaitKey(33);
		if(c == 27) break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Camera");
    
    return 0;
}
