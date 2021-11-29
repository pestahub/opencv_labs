#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "ManyImg.hpp";
#include <iostream>


#define _USE_MATH_DEFINES
#include <cmath>

const double M_E = 2.71828182845904523536;


void boxFilter(cv::Mat& image, int boxSize)
{
	boxSize = (boxSize - 1) / 2;
	for (int u = boxSize; u < image.cols - boxSize; u++)
	{
		for (int v = boxSize; v < image.rows - boxSize; v++)
		{
			cv::Rect roiRect(u - boxSize, v - boxSize, 2 * boxSize + 1, 2 * boxSize + 1);
			cv::Mat roi = image(roiRect);
			int sum = cv::sum(roi)[0];
			int mean = (int)(sum / (roi.rows * roi.cols));
			image.at<cv::uint8_t>(v, u) = mean;  //cv::mean(roi)[0];

		}
	}

}

void gaussFilter(cv::Mat& image, double sigma, int kernelSize)
{
	cv::Mat kernel(kernelSize, kernelSize, CV_32FC1);
	int kernelOffset = (kernelSize - 1) / 2;

	for (int u = -kernelOffset; u <= kernelOffset; u++)
	{
		for (int v = -kernelOffset; v <= kernelOffset; v++)
		{
			kernel.at<float>(v + (kernelSize - 1) / 2, u + (kernelSize - 1) / 2) = pow(M_E, -(v * v + u * u) / (2*sigma));
		}
	}
	kernel /= cv::sum(kernel);			// normalization

 	cv::filter2D(image, image, -1, kernel, cv::Point(kernelOffset, kernelOffset));

}

void laplaseFilter(cv::Mat& original)
{
	float kernelArray[] = {0.0, 1.0, 0.0, 1.0, -4.0, 1.0, 0.0, 1.0, 0.0};
	//float kernelArray[] = { -1.0, -1.0, -1.0, -1.0, -8.0, -1.0, -1.0, -1.0, -1.0 };
	cv::Mat kernel(3, 3, CV_32FC1, kernelArray);

	cv::filter2D(original, original, -1, kernel, cv::Point(-1, -1));

}

void unsharpMask(cv::Mat& original, cv::Mat& blurred, cv::Mat& output, double k = 1)
{
	output = original - blurred;
	output = k * output + original;
}

void laplaseUnsharp(cv::Mat& original, cv::Mat& laplase, double k = 1)
{
	original = original - k * laplase;
}

int subsctract(cv::Mat& img1, cv::Mat& img2, cv::Mat& result)
{
	double sumDiff = 0;
	for (int u = 0; u < img1.cols; u++)
	{
		for (int v = 0; v < img1.rows; v++)
		{
			uint8_t diff = abs(img1.at<uint8_t>(v, u) - img2.at<uint8_t>(v, u));
			sumDiff += diff;
			result.at<uint8_t>(v, u) = diff;

		}
	}
	sumDiff = sumDiff * 100 / 255;
	sumDiff = sumDiff / img1.size().area();
	return (int)sumDiff;
}

void logTransform(cv::Mat& img, double c)
{
	
	cv::MatIterator_<uint8_t> it, end;
	for (it = img.begin<uint8_t>(), end = img.end<uint8_t>(); it != end; ++it)
	{
		*it = (uint8_t)(c * log(*it + 1));
	}
}

int main() {
	cv::Mat original = cv::imread("../popug.jpg");
	cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);
	cv::Mat bfCopy = original.clone();
	cv::Mat blurCopy = original.clone();
	cv::Mat diff(original.rows, original.cols, original.type());
	cv::Mat unsharped(original.rows, original.cols, original.type());
	cv::Mat unsharped2(original.rows, original.cols, original.type());
	cv::TickMeter tm; 

	int disparity = 100;
	int task = 7;

	switch (task)
	{
	case 1:
		boxFilter(bfCopy, 7);

		cv::imshow("Task 1", bfCopy);
		break;

	case 2:
		tm.start();
		boxFilter(bfCopy, 7);
		tm.stop();
		std::cout << "Own implementation: " << tm.getTimeMilli() << std::endl;
		tm.start();
		cv::blur(blurCopy, blurCopy, cv::Size(7,7));
		tm.stop();
		std::cout << "Blur: " << tm.getTimeMilli() << std::endl;
		disparity = subsctract(bfCopy, blurCopy, diff);
		std::cout << "Image  similarity: " << 100 - disparity << "%" << std::endl;

		ShowManyImages("Task 2", 4, original, bfCopy, blurCopy, diff);
		break;

	case 3:
		std::cout << "Included in task #2" << std::endl;
		break;

	case 4:
		boxFilter(bfCopy, 7);
		//gaussFilter(blurCopy, 1, 3);
		cv::GaussianBlur(blurCopy, blurCopy, cv::Size(3, 3), 0.8);
		disparity = subsctract(original, blurCopy, diff);
		std::cout << "Image  similarity: " << 100 - disparity << "%" << std::endl;
		if (disparity < 5) logTransform(diff, 10);

		ShowManyImages("Task 4", 4, original, bfCopy, blurCopy, diff);
		break;

	case 5:
		gaussFilter(blurCopy, 1.4, 7);
		//cv::GaussianBlur(blurCopy, blurCopy, cv::Size(7, 7), 1.4);
		unsharpMask(original, blurCopy, unsharped, 3);
		boxFilter(bfCopy, 7);
		unsharpMask(original, bfCopy, unsharped2, 3);
		disparity = subsctract(unsharped, unsharped2, diff);
		std::cout << "Image  similarity: " << 100 - disparity << "%" << std::endl;

		ShowManyImages("Task 5", 4, original, unsharped, unsharped2, diff);
		break;
	case 6:
		laplaseFilter(bfCopy);

		cv::imshow("Laplase", bfCopy);
		break;

	case 7:
		laplaseFilter(bfCopy);
		unsharped2 = original.clone();
		laplaseUnsharp(unsharped2, bfCopy, 1.5);

		ShowManyImages("Task 7", 3, original, bfCopy, unsharped2);
		break;

	default:
		break;
	}
	
	
	char key = (char)cv::waitKey(-1);
}