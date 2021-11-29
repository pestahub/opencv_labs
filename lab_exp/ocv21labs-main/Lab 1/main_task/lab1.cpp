#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

cv::Scalar backYellow(0, 204, 255);
double scalingFactor = 100; 

void addTransparent(cv::Mat frame, cv::Mat transp) {
	cv::Mat mask;
	std::vector<cv::Mat> layers;

	cv::split(transp, layers); // seperate channels
	cv::Mat rgb[3] = { layers[0],layers[1],layers[2] };
	mask = layers[3]; // png's alpha channel used as mask
	cv::merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
	transp.copyTo(frame, mask);
}

int main() {
	cv::Mat back = cv::imread("../back.png");
	cv::Mat robot = cv::imread("../rumba.png", cv::IMREAD_UNCHANGED);
	//td::cout << robot << std::endl;
	cv::namedWindow("Result");

	const int offsetY = 512;			// baseline
	int robotX = robot.cols/2;
	int robotY = offsetY;
	cv::Point oldPoint(robotX, robotY);


	while (robotX < back.cols - robot.cols)
	{
		robotX++;
		robotY = (int)(std::sin(robotX/scalingFactor)* scalingFactor) + offsetY;

		cv::Point newPoint(robotX, robotY);
		cv::line(back, oldPoint, newPoint, backYellow, 3);
		oldPoint = newPoint;

		cv::Mat backBuffer(1080, 1080, CV_8UC4);
		back.copyTo(backBuffer);

		cv::Mat back_roi( backBuffer, cv::Rect(robotX- robot.cols/2, robotY- robot.rows/2, robot.cols, robot.rows));
		addTransparent(back_roi, robot);
		
		cv::imshow("Result", backBuffer);
		cv::waitKey(13);
		if (robotX == back.cols / 2) {
			cv::imwrite("../shot.png", backBuffer);
			std::cout << robotX;
		}
	}

	
	char key = (char)cv::waitKey(-1);
}