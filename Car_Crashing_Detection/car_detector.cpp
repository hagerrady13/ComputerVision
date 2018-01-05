/*
Authored by: Hager Radi
15 Dec. 2015
Car detection in videos
*/
#include <QCoreApplication>
#include <cv.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
using namespace cv;
using namespace std;

// detect the accidents
void detect_crash(vector<Rect > &all, Mat & img){
    Rect t;
    for(int i=0 ; i< all.size() ; i++){
        for(int j= i+1 ; j<all.size() ; j++){
            t = all[i] & all[j];              // check the intersection between any two rectangles
            if(t.area() > 5){
                putText(img , "ACCIDENT HERE!" , Point(50,50), FONT_HERSHEY_COMPLEX , 2.0 , CV_RGB(0,255,0) , 2.0);
            }
        }
    }
}
// outline the overall motion in the frame
int draw_motion(Mat &diff,Mat dst){
    int changes_count = 0;
    int min_x = diff.cols, max_x = 0;
    int min_y = diff.rows, max_y = 0;
    for(int j = 0; j < diff.rows; j+=2){ // height
                for(int i = 0 ; i < diff.cols ; i+=2){ // width
                    // check if at pixel (j,i) intensity is equal to 255
                    // this means that the pixel is different in the sequence
                    // of images (prev_frame, current_frame, next_frame)
                    if(diff.at<uchar>(j,i) == 255)
                    {
                        changes_count++;
                        if(min_x>i) min_x = i;
                        if(max_x<i) max_x = i;
                        if(min_y>j) min_y = j;
                        if(max_y<j) max_y = j;
                    }
                }
            }
            if(changes_count){
                //check if not out of bounds
                if(min_x-10 > 0) min_x -= 10;
                if(min_y-10 > 0) min_y -= 10;
                if(max_x+10 < diff.cols-1) max_x += 10;
                if(max_y+10 < diff.rows-1) max_y += 10;
                // draw rectangle round the changed pixel
                Point x(min_x,min_y);
                Point y(max_x,max_y);
                Rect rect(x,y);
               // Mat cropped = result(rect);
               // cropped.copyTo(result_cropped);
                rectangle(dst,rect,Scalar(0,255,255),1);
            }
}

//background subtraction
void Background_Subtract(Mat &one,Mat &two, Mat &diff){
    diff = Mat::zeros(one.size(), one.type() );
    for(int i= 0 ; i<one.rows ; i++){
        for(int j=0 ; j<one.cols ; j++){
            diff.at<uchar>(i,j) = abs(two.at<uchar>(i,j) - one.at<uchar>(i,j));
        }
    }
}

RNG rng(12345);

int main(int argc, char *argv[])
{
   // QCoreApplication a(argc, argv);
    VideoCapture video;
    video.open( "/home/hagar13/Desktop/traffic1.mp4");
    video.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    video.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    if (!video.isOpened())
    {
        std::cout << "!!! cvCreateFileCapture didn't found the file !!!\n";
        return -1;
    }
    Mat current,next , frame1, frame2 , difference , final_diff , detected_edges;
    namedWindow("video", CV_WINDOW_AUTOSIZE);
    while(true)
    {
           video.read(current);
           video.read(next);
        if (!current.empty() && !next.empty()){

            cvtColor(current,frame1,CV_RGB2GRAY);
            cvtColor(next,frame2,CV_RGB2GRAY);
             Background_Subtract(frame1,frame2 , difference);
             threshold(difference,final_diff, 10 , 255 ,CV_THRESH_BINARY);
            // draw_motion(final_diff, drawing);
                vector<vector<Point> > contours;
                vector<Rect> Recs;
                vector<Vec4i> hierarchy;
               // RNG rng(12345);

                findContours( final_diff, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                /// Draw contours
                Mat drawing = Mat::zeros( final_diff.size(), CV_8UC3 );
                cout<<contours.size()<<'\n';

                for( int i = 0; i< contours.size(); i++ )
                {
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    drawContours( current, contours, i, color, 2, 8, hierarchy, 0, Point() );
                    drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                }

                for( int i = 0; i< contours.size(); i++ )
                 {
                     //  Find the area of contour
                         double a=contourArea( contours[i],false);
                        if(a > 2500){
                         // Find the bounding rectangle for biggest contour
                             Rect bounding_rect=boundingRect(contours[i]);
                             Recs.push_back(bounding_rect);
                             rectangle(current, bounding_rect,  Scalar(0,255,0),2, 8,0);
                       }
                 }
                detect_crash(Recs, current);
                imshow( "Result window", drawing );
                imshow("diff", final_diff);
                imshow("video", current);
            // current = next;
      }
        if (waitKey(10) == 'q')
            break;
    }
    waitKey(0);
    return 0;
}
