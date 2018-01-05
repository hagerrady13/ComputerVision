/*
Authored by: Hager Radi
28 Nov. 2015
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
using namespace cv;
using namespace std;


#define PI 3.14159265

 vector<Vec4i> lines;
 Mat image, out ,dst , temp;
 std::vector<cv::Point2f> FinalCorners;

 // direction vectors
 const int dx[] = {+1, 0, -1, 0};
 const int dy[] = {0, +1, 0, -1};

 // matrix dimensions
 const int rows = 800;
 const int cols = 800;

 // get the edges using sobel operator
 Mat getEdges(Mat &source){

     unsigned int srcH = source.rows;
     unsigned int srcW = source.cols;

     Mat imDst = Mat::zeros(source.size(), source.type() );

     //do blurring before detecting the edges
     GaussianBlur( source, source, Size(3,3), 2, 2 );

     int Lx[3][3];
     int Ly[3][3];

     // for vertical edges
     Lx[0][0] = -1;  Lx[0][1] = +0;  Lx[0][2] = +1;
     Lx[1][0] = -2;  Lx[1][1] = +0;  Lx[1][2] = +2;
     Lx[2][0] = -1;  Lx[2][1] = +0;  Lx[2][2] = +1;

     // for horizental edges
     Ly[0][0] = +1;  Ly[0][1] = +2;  Ly[0][2] = +1;
     Ly[1][0] = +0;  Ly[1][1] = +0;  Ly[1][2] = +0;
     Ly[2][0] = -1;  Ly[2][1] = -2;  Ly[2][2] = -1;

     //loop through all the pixels and calculate the gradient
     for(unsigned int x = 0; x < srcH; x++)
     {
         for(unsigned int y = 0; y < srcW; y++)
         {
             float Gx = 0 , Gy=0;
             // check the boundry pixels
             if(x==0 || x == srcH-1 || y==0 || y == srcW-1){
                 Gx = 0;
                 Gy = 0;
             }
             else{
                 for(int i = -1; i <= 1; i++)
                 {
                     for(int j = -1; j <= 1; j++)
                     {
                         // Acculumlate Gx and Gy
                         Gx += (Lx[1+i][1+j]*int(source.at<uchar>(i+x,j+y)) );
                         Gy += (Ly[1+i][1+j]*int(source.at<uchar>(i+x,j+y)) );

                     }
                 }
             }

             // 75 for (1,2,30)
             if(imDst.at<uchar>(x,y) = sqrt(pow(Gx, 2) + pow(Gy, 2)) > 60 ){
                 imDst.at<uchar>(x,y) = 255;
                // cout<<atan2(Gy,Gx) * 180 /PI<<'\n';
               }else{
                 imDst.at<uchar>(x,y) = 0;
             }
         }

     }
     // Display the edges as binary image
     imshow( "Edges Detected!", imDst );
     return imDst;
 }
 // Compute the intersections between the lines detected in Hough
Point2f GetIntersect(cv::Vec4i a, cv::Vec4i b)
{
   // From WikiPedia (Line-line intersection)
   int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
   int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

   float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4));
   if (d != 0 )
   {
     cv::Point2f pt;
     pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
     pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
            // 20 for (1,2,3)
           int Intersect_t = 50;
           if(pt.x < min(x1,x2)- Intersect_t || pt.x > max(x1,x2) + Intersect_t  || pt.y < min(y1,y2) - Intersect_t || pt.y > max(y1,y2) + Intersect_t){
                return Point2f(-1,-1);
           }
           if(pt.x < min(x3,x4)- Intersect_t|| pt.x > max(x3,x4)+Intersect_t || pt.y < min(y3,y4)-Intersect_t || pt.y > max(y3,y4)+Intersect_t){
                return Point2f(-1,-1);
           }
     return pt;
   }
   else
     return cv::Point2f(-1,-1);

}
// Exclude the lines that are not horizental nor vertical  (not used)
void checkLines(){
    int a,b,c,d;

    for(int i=0 ; i<lines.size() ; i++){
        a = lines[i][0];
        b = lines[i][1];
        c = lines[i][2];
        d = lines[i][3];

        int t = 9;
        if(abs(c-a) >= t && abs(d-b) >= t){     // not vertical nor horizental
                lines.erase(lines.begin()+i);
                i = i-1;
      }
    }
}
// check if they are rectangle   ( if only half of the points around it are white)
bool CheckCorners(Point2f t){
    int count=0;
    const int bound  = 2 ;
    for(int k= 1 ; k <= bound ; k++)
        for(int i=0 ; i< 4 ; i++ ){
            int newx = t.x+ dx[i] , newy = t.y+ k*dy[i];
            if(newx <= rows && newy <= cols){
                if(dst.at<uchar>(newy,newx) == 255){
                    count++;
                }
            }
        }
    if(count > (bound*2)){
        return false;
    }
    else{
        return true;
    }
}
// check if each 4 points form a rectangle
bool Find_Recs( Point a, Point b, Point c, Point d)
{
    const int rec = 5 ;

    double cx,cy;
    double dd1,dd2,dd3,dd4;


    cx=(a.x + b.x + c.x + d.x )/4; // getting the center points
    cy=(a.y + b.y + c.y+ d.y)/4;

    dd1 = sqrt(pow((cx-a.x),2)+pow((cy-a.y),2));
    dd2 = sqrt(pow((cx-b.x),2)+pow((cy-b.y),2));
    dd3 = sqrt(pow((cx-c.x),2)+pow((cy-c.y),2));
    dd4 = sqrt(pow((cx-d.x),2)+pow((cy-d.y),2));

    return (abs(dd1-dd2)<=rec) && (abs(dd1-dd3)<=rec) && (abs(dd1-dd4)<=rec);
}
// Find all the rectangles, draw them
void Detect_Rect(){
    const int rec_t = 15 ;
    int rec_count = 0;
    for(int k= 0; k< FinalCorners.size() ; k++){
        for(int j = k+1 ; j< FinalCorners.size() ; j++){
                for(int l = j+1 ; l < FinalCorners.size() ; l++){
                        for(int i = l+1 ; i < FinalCorners.size() ; i++){
                                Point a,b,c,d;
                                a = FinalCorners[k];
                                b = FinalCorners[j];
                                c = FinalCorners[l];
                                d = FinalCorners[i];
                                if(Find_Recs(a , b, c, d)){
                                    rec_count ++;
                                    if( abs(a.y - c.y) > rec_t && abs(a.x - c.x) > rec_t){
                                        rectangle(out, a , c,Scalar( 255, 255, 255 ),3);
                                        circle( out ,a , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        circle( out , c , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        cout<< "Vertex 1: "<<"("<<a.x<<" , "<<a.y<<" ) ";
                                        cout<< "Vertex 2: "<<"("<<c.x<<" , "<<c.y<<" ) "<<'\n';

                                    }
                                    else if (abs(a.y - b.y) > rec_t && abs(a.x - b.x) > rec_t){
                                        rectangle(out, a , b ,Scalar( 255, 255, 255 ),3);
                                        circle( out ,a , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        circle( out , b , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        cout<< "Vertex 1: "<<"("<<a.x<<" , "<<a.y<<" ) ";
                                        cout<< "Vertex 2: "<<"("<<b.x<<" , "<<b.y<<" ) "<<'\n';
                                    }
                                    else{
                                        rectangle(out, a , d ,Scalar( 255, 255, 255 ),3);
                                        circle( out ,a , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        circle( out , d , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
                                        cout<< "Vertex 1: "<<"("<<a.x<<" , "<<a.y<<" ) ";
                                        cout<< "Vertex 2: "<<"("<<d.x<<" , "<<d.y<<" ) "<<'\n';
                                    }
                                }
                            }
                     }
                }

          }
    cout<<"Rectangles count: "<<rec_count<<'\n';
}
// Eliminate the corners that are not rectangles and remove the ones that are near to each other.
void Eliminate_corners(vector<  Point2f > corners){
       // 20 (1, 2), 22(3) , 15 (5)
    const int Corner_t = 15 ;
    for(int i = 0 ; i< corners.size() ; i++){
        bool flag = true;
        for(int j=i+1 ; j<corners.size() ; j++){
            if(abs(corners[i].x - corners[j].x) < Corner_t && abs(corners[i].y - corners[j].y) < Corner_t){
                flag= false;
            }
        }
        if(flag && CheckCorners(corners[i])){
            FinalCorners.push_back(corners[i]);
            circle(temp, corners[i] , 5 ,  Scalar( 110, 220, 0 ), 2, 8 );
        }
    }
    imshow("Final Corners", temp);
}


int main(int argc, char *argv[])
{
    string imageName;
    if(argc > 1){
        imageName = argv[1];
    }
    else{
        cout<<"Not enough Arguments";
    }
    // only needed for displaying purposes
    image = imread(imageName.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // Read the file
    temp = imread(imageName.c_str() , CV_LOAD_IMAGE_COLOR);
    out = imread(imageName.c_str() , CV_LOAD_IMAGE_COLOR);

    if( image.empty() || out.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << endl ;
        return -1;
    }

      resize(image,image,Size(rows,cols));//resize image
      resize(out,out,Size(rows,cols));//resize image
      resize(temp,temp,Size(rows,cols));//resize image

      dst = getEdges(image);
      cvtColor(image, image, CV_GRAY2BGR);

        // hough lines detection
      // 90 (1,2 , 3) , 50 (5)
      HoughLinesP(dst, lines, 1, CV_PI/180, 50 , 50 , 8);

      for( size_t i = 0; i < lines.size(); i++ )
      {
        line(image, Point(lines[i][0],lines[i][1]), Point(lines[i][2], lines[i][3]),Scalar( 0, 0, 255 ), 0.5, CV_AA);
        line(temp, Point(lines[i][0],lines[i][1]), Point(lines[i][2], lines[i][3]),Scalar( 0, 0, 255 ), 0.5, CV_AA);
      }
      imshow(" Lines detected", image);


      std::vector<cv::Point2f> corners;

      for( size_t i = 0; i < lines.size(); i++ )
      {
          for(int j=i+1 ; j<lines.size() ; j++){
              Point2f In_point = GetIntersect(lines[i],lines[j]);
              if(In_point.x  >=0 && In_point.y >= 0){
                  corners.push_back(In_point);
                  circle( image , In_point , 5 ,  Scalar( 110, 220, 0 ), 1, 8 );
              }
            }
      }
      imshow(" Intersections detected", image);

      Eliminate_corners(corners);
      cout<<"Corners before Elimination: " <<corners.size()<<" , Corners after:  "<<FinalCorners.size()<<'\n';

      Detect_Rect();

      imshow(" Rectangles detected", out);

      waitKey(0);

    return 0;
}
