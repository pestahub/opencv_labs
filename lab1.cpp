
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 

#include <iostream>

using namespace cv;


void robot(Mat img, Point pos)
{
  rectangle(img, Point(pos.x+10,pos.y+10), Point( pos.x+70, pos.y+70), Scalar(0,0,255), 4);
  circle(img, Point(pos.x+40,  pos.y+40), 15, Scalar(255,0,255), FILLED);
  rectangle(img, Point(pos.x, pos.y+15), Point( pos.x+10, pos.y+25), Scalar(0,0,0), 4);
  rectangle(img, Point( pos.x, pos.y+55), Point( pos.x+10, pos.y+65), Scalar(0,0,0), 4);
  rectangle(img, Point( pos.x+70, pos.y+15), Point( pos.x+80, pos.y+25), Scalar(0,0,0), 4);
  rectangle(img, Point( pos.x+70, pos.y+55), Point( pos.x+80, pos.y+65), Scalar(0,0,0), 4);
}

int main( int argc, char** argv ) 
{
  
  Mat plane;
  plane = imread("/home/pesta/opencv_labs/images/plane.jpeg" ,cv::IMREAD_COLOR);
  
  if(! plane.data ) {
    std::cout <<  "Image plane not found or unable to open" << std::endl ;
    return -1;
  }
  
  Mat image = plane.clone();
  Point current_point = Point(0, image.rows/2 - 30);
  
  namedWindow( "image", WINDOW_AUTOSIZE );
  while (1)
  {

    current_point = Point(current_point.x + 1, (image.rows/2 - 30) + sin(((double)current_point.x + 1.0)/10)*100);
    std::cout << current_point.x << "   " << current_point.y << std::endl;
    circle(image, Point(current_point.x+40,  current_point.y+40), 2, Scalar(0,0,255), FILLED);
    Mat new_image = image.clone();
    robot(new_image, current_point);
    imshow("image", new_image);
    cv::waitKey(10);
    if (current_point.x == (image.cols / 2 - 40))
    {
      imwrite("sin_robot.png", new_image);
    }
    if (current_point.x == (image.cols - 80))
    {
      break;
    }
  }

  destroyAllWindows();
  return 0;
}