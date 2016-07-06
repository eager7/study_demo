#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <sstream>
#include<iostream>
#include <unistd.h>

using namespace std;
using namespace cv;

void createAlphaMat(Mat &mat)
{
	for(int i = 0; i < mat.rows; ++i){
		for (int j = 0; j < mat.cols; ++j) {
			Vec4b &rgba = mat.at<Vec4b>(i, j);
			rgba[0] = UCHAR_MAX;
			rgba[1] = saturate_cast<uchar>(float(mat.cols - j) / (float)mat.cols * UCHAR_MAX);
			rgba[2] = saturate_cast<uchar>(float(mat.rows - j) / (float)mat.rows * UCHAR_MAX);
			rgba[3] = saturate_cast<uchar>(0.5 * (rgba[1]+rgba[2]));
		}
	}
}

int main(int argc, char **argv)
{
	cout<<"Write a image"<<endl;

	Mat mat(480, 640, CV_8UC4);
	createAlphaMat(mat);
	namedWindow("Show");

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);


	imshow("Show", mat);
	waitKey(2000);
	destroyWindow("Show");

	try {
		imwrite("test.png", mat, compression_params);
	}catch (runtime_error& ex){
		cout<<ex.what()<<endl;
		return 1;
	}
	cout<<"Sucess Save Picture"<<endl;
	return 0;
}