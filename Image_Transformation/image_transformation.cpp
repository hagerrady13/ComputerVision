// Hagar Rady Mahmoud, 900123208
// Computer Vision Assignment 1
#include <QCoreApplication>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
using namespace cv;
using namespace std;

// Rotation around the origin (practice)
void Rotate(Mat &imSrc,float theta){

    float theta_rad = (theta * 3.14159265)/180.0;
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    cout<<imSrc.size();
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
            int xnew = int((i*cos(theta_rad))-(j*sin(theta_rad)));
            int ynew = int((j*cos(theta_rad))+(i*sin(theta_rad)));
             // imDst.at<uchar>(int((j*cos(theta_rad))+(i*sin(theta_rad))),int((i*cos(theta_rad))-(j*sin(theta_rad))))=imSrc.at<uchar>(j,i);
            if(xnew < 0 || xnew>=srcH || ynew < 0 || ynew>=srcW )
            {
                imDst.at<uchar>(j,i) = 0;
            }
            else{
                   imDst.at<uchar>(j,i)=imSrc.at<uchar>(ynew,xnew);
            }
        }
    }

    imshow( "Rotated Image", imDst);                // Show our image inside it.
    waitKey(90000);
}
// Rotation around an arbitrary point
void Arb_Rotate(Mat &imSrc,int px,int py,float theta){
    float theta_rad = (theta * 3.14159265)/180.0;
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;
cout<<"shjfsj0";
    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    cout<<imSrc.size();
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){

            int xnew = int(((i-px)*cos(theta_rad))-((j-py)*sin(theta_rad)))+px;
            int ynew = int(((j-py)*cos(theta_rad))+(((i-px)*sin(theta_rad))))+py;

            if(xnew < 0 || xnew>=srcH || ynew < 0 || ynew>=srcW )
            {
                imDst.at<uchar>(j,i) = 0;
            }
            else{
                   imDst.at<uchar>(j,i)=imSrc.at<uchar>(ynew,xnew);
            }
        }
    }
    imSrc = imDst;
    imshow( "Rotated Image", imDst);                // Show our image inside it.
    waitKey(90000);
}
// Translation
void Translate(Mat &imSrc,int dx,int dy){
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;
    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    for(int i=0 ; i< (srcH-dx) ; i++){
        for(int j=0 ; j< (srcW-dy) ; j++ ){
          //  dst[(i+dx)*srcW + (j+dy)] = src[i*srcW + j];
            imDst.at<uchar>(j+dy,i+dx) = imSrc.at<uchar>(j,i);
        }
    }
    imSrc= imDst;
  //  imshow( "Translated Image", imDst );                // Show our image inside it.
   // waitKey(90000);
}
// Scaling
void Scale(Mat &imSrc, float Sfactor){      // not working for small factors
  //  int Sfactor = 2.5;
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;
    Mat imDst = Mat::zeros(int(srcH*Sfactor), int(srcW*Sfactor) ,imSrc.type() );

    for(int i=0 ; i< int(srcH*Sfactor) ; i++){
        for(int j=0 ; j< int(srcW*Sfactor) ; j++ ){
         //   dst[(i)*(srcW*Sfactor) + j] = src[int(i/Sfactor)*srcW + int(j/Sfactor)];
            int newi = (int)(i/Sfactor);
            int newj = (int)(j/Sfactor);
            if(newi <0 || newi>= srcH || newj<0 || newj>= srcW ){
                imDst.at<uchar>(j,i) = 0;
            }
            imDst.at<uchar>(j,i) = imSrc.at<uchar>(newj,newi);
        }
    }
    imSrc= imDst;
 //   imshow( "Scaled Image", imDst );
   // waitKey(90000);
}
// combined Transformation (Translation,Scaling, Rotation)
void Combined(Mat &image,int dx,int dy,float Sfactor,int px,int py,float theta){
  // Mat Dst = Mat::zeros(imSrc.size(), imSrc.type() );
    Translate(image,dx,dy);
    Scale(image,Sfactor);
    Arb_Rotate(image,px,py,theta);
    imshow( "All Image", image );
}
// Negative Transformation
void Neg_Transform(Mat &imSrc){
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    int L = 256;

    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
            imDst.at<uchar>(j,i) = L- 1 - imSrc.at<uchar>(j,i);
        }
    }

    imshow( "Negative Transformation", imDst );
    waitKey(90000);
}
// Log Transformation
void Log_Transform(Mat &imSrc){
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    int L = 256;
    double c = (L-1) / log(L);

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );

    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
         //   dst[i*srcW + j] = c*log(1+src[i*srcW + j]);
            imDst.at<uchar>(j,i) = c*log(1+imSrc.at<uchar>(j,i));
        }
    }

    imshow( "Log Transformation", imDst );
    waitKey(90000);
}
// Inverse Log Transformation
void InvLog_Transform(Mat &imSrc){
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    int L = 256;
    double c = log(L) /(L-1);

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );

    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
            imDst.at<uchar>(j,i) = exp(c*imSrc.at<uchar>(j,i));
        }
    }

    imshow( "Inverse Log Transformation", imDst );
    waitKey(90000);
}
/*
void Nth_Transform(Mat &imSrc,float n){   // Not wokring well
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    int L = 256;
    double c = (L-1) / log(L);

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );

    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
            //dst[i*srcW + j] = 255*pow((src[i*srcW + j]),n);
            imDst.at<uchar>(j,i) = pow(imSrc.at<uchar>(j,i),n);
        }
    }
    imshow( "Nth Transformation", imDst );
    waitKey(90000);
} */

// Bit Slicing , takes the bit number as an input
void Bit_slicing(Mat &imSrc,int bit_no){

    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){

            imDst.at<uchar>(j,i) = imSrc.at<uchar>(j,i) & (1<<bit_no);
        }
    }

    imshow( "Bit-slicing", imDst );
    waitKey(90000);
}
// compress the image to only the first 5 out of 8 planes
void Bits_compression(Mat &imSrc){

    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){

            imDst.at<uchar>(j,i) = imSrc.at<uchar>(j,i) & 0xf8;
        }
    }

    imshow( "the 5 Planes compressed", imDst );
    waitKey(90000);
}
// Histogram Equalization
void Histo_equal(Mat &imSrc){

    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;
    unsigned char *src = imSrc.data;
    unsigned char *dst = new unsigned char [srcH*srcW];
    unsigned int Histo[256];
    unsigned int Accum[256];
    // get the number of occurances of each gray level

    memset(Histo,0,sizeof(Histo));
    memset(Accum,0,sizeof(Accum));
    for(int i= 0 ; i<(srcW*srcH) ; i++){
        Histo[int(src[i])]++;
    }

    int sum=0;
    for(int i=0 ; i<256 ; i++){
        sum += Histo[i];
        Accum[i] = sum;
    }
    double alpha = 255.0/(srcW*srcH);
    // get the new image
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j< (srcW) ; j++ ){
            dst[i*srcW+j] = int(Accum[src[i*srcW+j]]*alpha);
        }
    }
    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    imDst.data = dst;
    imshow( "Histogram Equalization", imDst );
    delete []dst;
    waitKey(90000);
}
// Triangular Folding
void Tri_folding(Mat &imSrc){
    unsigned int srcH = imSrc.rows;
    unsigned int srcW = imSrc.cols;

   // unsigned char *src = imSrc.data;
    //unsigned char *dst = new unsigned char [srcH*srcW];

    Mat imDst = Mat::zeros(imSrc.size(), imSrc.type() );
    for(int i=0 ; i< (srcH) ; i++){
        for(int j=0 ; j<=i ; j++ ){
         //  dst[i*srcW + j] = int(src[(srcW-i)*srcW + j] + src[i*srcW+j])/2 ;
           imDst.at<uchar>(j,i) = int(imSrc.at<uchar>(srcW-j,i) + imSrc.at<uchar>(j,i))/2;
        }
    }
    // imDst.data = dst;
    imshow( "Triangular Folding", imDst );
    waitKey(90000);
}
int main(int argc, char *argv[])
{
  //  QCoreApplication a(argc, argv);
    int dx,dy,bit_no,px,py;
    float Scale_factor,rot_angle,n;
    string imageName;
    if( argc > 1)
    {
         imageName = argv[1];
         dx = atoi(argv[2]);
         dy = atoi(argv[3]);
         Scale_factor = atof(argv[4]);
         px = atoi(argv[5]);
         py = atoi(argv[6]);
         rot_angle = atoi(argv[7]);
         bit_no = atoi(argv[8]);
         n = atof(argv[9]);
    }
    else{
        cout <<  "No enough Arguments" << endl ;
        return -1;
    }
    Mat image;
    image = imread(imageName.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // Read the file
    if( image.empty() )                      // Check for invalid input
    {
         cout <<  "Could not open or find the image" << endl ;
         return -1;
    }
   // Translate(image,dx,dy);
  //  Scale(image,Scale_factor);
  //  Rotate(image,rot_angle);
  //  Arb_Rotate(image,px,py,rot_angle);
   // Combined(image,dx,dy,Scale_factor,px,py,rot_angle);
  //  Neg_Transform(image);
 //   Log_Transform(image);
    InvLog_Transform(image);
   // Bit_slicing(image,bit_no);
   //  Bits_compression(image);
   // Histo_equal(image);
  //  Tri_folding(image);
  //  imshow( "Original Image", image );
    waitKey(0);
    return 0;

  //  return a.exec();
}
