/*
Authored by: Hager Radi
15 Dec. 2015
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

int main(int argc, char *argv[])
{
    VideoCapture video;
    video.open("/home/hagar13/Desktop/traffic1.mp4");
    if (!video.isOpened())
    {
        std::cout << "!!! cvCreateFileCapture didn't found the file !!!\n";
        return -1;
    }
    Mat current,next , frame1, frame2 , difference , final_diff , detected_edges;
    namedWindow("video", CV_WINDOW_AUTOSIZE);
   // video >> current;
    Mat src_gray, canny_output , err,channels, labels, stats, centroids;
    std::vector<cv::Point2f> features_prev, features_next;
    vector<uchar> featuresFound;
      video >> current;
//      cvtColor(current,frame1,CV_BGR2GRAY);
//      cv::goodFeaturesToTrack(frame1, // the image
//       features_prev,   // the output detected features
//       52,  // the maximum number of features
//       0.2,     // quality level
//       20     // min distance between two features
//       );
    while(true)
    {
        try{

        video >> next;
            if (!current.empty() && !next.empty()){
              //  cout<<current.type();

                cvtColor(current,frame1,CV_BGR2GRAY);
                cvtColor(next,frame2,CV_BGR2GRAY);
                cout<<frame1.type();
                absdiff(frame1,frame2,difference);
                cv::goodFeaturesToTrack(frame1, // the image
                 features_prev,   // the output detected features
                 52,  // the maximum number of features 50
                 0.2,     // quality level  0.2
                 20     // min distance between two features 20
               );
                for(int i=0 ; i<features_prev.size() ; i++){
                    circle(current,features_prev[i] , 5 , Scalar(0,255,255));
                }
               // Tracker:init(current,difference);
                cout<<features_prev.size()<<'\n';
               if(features_prev.size() > 20){
                cv::calcOpticalFlowPyrLK(
                    frame1, frame2, // 2 consecutive images
                    features_prev, // input point positions in first im
                    features_next, // output point positions in the 2nd
                    featuresFound,    // tracking success
                    err      // tracking error
                  );
                cout<<features_next.size()<<'\n';
                for(int i=0 ; i<featuresFound.size() ; i++){
                //    circle(current,featuresFound[i] , 5 , Scalar(0,255,0));

                    Point2f p0( ceil( features_prev[i].x ), ceil( features_prev[i].y ) );
                    Point2f p1( ceil( features_next[i].x ), ceil( features_next[i].y ) );
                    line(current, p0, p1, CV_RGB(255,0,0), 5 );
                }
//                cv::Mat H= cv::findHomography(features_prev, features_next);

//                if( !H.empty()) {
//                        // Warp the current frame to match the previous frame.
//                        cv::warpPerspective( frame1, frame2, H, frame1.size(), cv::INTER_NEAREST| cv::WARP_INVERSE_MAP);
//                  }

              }

                threshold(difference,final_diff, 15 , 255 ,CV_THRESH_BINARY);
               //  draw_motion(final_diff,current);
                 // features_prev = features_next;
                  imshow("diff", final_diff);
                  imshow("video", current);
                  current = next.clone();
                  features_prev.clear();
                  features_prev = features_next;
          }
         waitKey(33);
        }
        catch (Exception& e)
        {
            const char* err_msg = e.what();
            std::cout << "exception caught: imshow:\n" << err_msg << std::endl;
        }
    }
    waitKey(0);
    return 0;
}
