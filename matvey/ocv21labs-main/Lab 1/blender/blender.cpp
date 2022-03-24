#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


int main() {


	cv::Mat img1 = cv::imread("../img1.png");
	cv::resize(img1, img1, cv::Size(440, 120));
	cv::Mat img2 = cv::imread("../img2.png");
	cv::resize(img2, img2, cv::Size(440, 120));
	cv::Mat result;
	cv::namedWindow("Result");

	double alpha=0.5;
	double beta = (1.0 - alpha); 
	cv::addWeighted(img1, alpha, img2, beta, 0.0, result);

		
	cv::imshow("Result", result);
	cv::waitKey();

}