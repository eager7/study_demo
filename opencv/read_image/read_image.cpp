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
	Mat img1 = imread("test.jpg");
	Mat img2 = imread("test.jpg", 0);
	Mat img3 = imread("test.jpg", 199);
	namedWindow("Show");
	imshow("Show", img1);
	waitKey(3000);
	imshow("Show", img2);
	waitKey(3000);
	imshow("Show", img3);
	waitKey(6000);
	destroyWindow("Show");
	return 0;
}