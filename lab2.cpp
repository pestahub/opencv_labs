
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>

#include <iostream>
#include <cmath>

int disparity = 100;

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


void box_filter(cv::Mat& image, int boxSize)
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
      image.at<uint8_t>(v, u) = mean;

    }
  }
}

void box_blur(cv::Mat plane)
{
  cv::TickMeter tm; 
  cv::Mat diff(plane.rows, plane.cols, plane.type());
  cv::Mat box_output = plane.clone();
  cv::Mat blur_output = plane.clone();
  tm.start();
  box_filter(box_output, 7);
  tm.stop();
  std::cout << "box filter: " << tm.getTimeMilli() << std::endl;
  tm.start();
  cv::blur(blur_output, blur_output, cv::Size(7,7));
  tm.stop();
  std::cout << "Blur: " << tm.getTimeMilli() << std::endl;
  disparity = subsctract(box_output, blur_output, diff);
  std::cout << "Image  similarity: " << 100 - disparity << "%" << std::endl;


  cv::imshow("plane", plane);
  cv::imshow("box_output", box_output);
  cv::imshow("blur_output", blur_output);
  cv::imshow("diff", diff);
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

void gauss(cv::Mat plane)
{
  gauss_copy = plane.clo
  gaussFilter(plane, 1, 3);
}

int main( int argc, char** argv ) 
{
  
  cv::Mat plane;
  plane = imread("/home/pesta/opencv_labs/images/Lenna.png" ,cv::IMREAD_GRAYSCALE);
  cv::resize(plane, plane, cv::Size(500, 500));
  // box_blur(plane);
  gauss(plane);
  cv::waitKey(0);
  cv::destroyAllWindows();
  return 0;
}