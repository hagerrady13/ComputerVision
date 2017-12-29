/*
Authored by: Hager Radi
15 Nov. 2015
A C++ implementation of Coin detection from 2D Images. It is based on circle Hough Transform algorithm.
*/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
using namespace cv;
using namespace std;

unsigned int min_r = 53;           //minimum radius
unsigned int max_r = 65;           // maximum radius
double totalSum = 0.0;

Mat original_image , source;

vector< vector<pair<int,int> > >Houghs (max_r - min_r);    // the accumulator

// Functions
Mat getEdges();                             // implements edge detection algorithm using sobel operator
Mat DetectEdges();                         // detect edges using Canny
void HoughTrans(Mat &);                   // implements circular hough transformation
void Accumulate_hough(Mat &,int ,int ,int );      // accumulate in hough space
void Accumulate_pixel(Mat &,int ,int );
void draw_circle();                                  // draw the detected circles over the coins

int main(int argc, char *argv[])
{
	string imageName;
    if( argc > 1)
    {
        imageName = argv[1];
	}
    
    source = imread(imageName.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // Read the file
	original_image = imread(imageName.c_str() , CV_LOAD_IMAGE_COLOR );

    if( original_image.empty() || source.empty() )                      // Check for invalid input
    {
         cout <<  "Could not open or find the image" << endl ;
		 system("pause");
         return -1;
    }
	//using Canny 
//	Mat Binary = DetectEdges();
//	HoughTrans(Binary);
	
	// using my algorithm
	Mat Binary= getEdges();
	HoughTrans(Binary);
	imshow( "Original Image with coins detected", original_image);
	waitKey(0);
	system("pause");
    return 0;
}
// get the edges using sobel operator
Mat getEdges(){

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
			int Gx = 0 , Gy=0;
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
			// using 150 as threshold
			imDst.at<uchar>(x,y) = sqrt(pow(Gx, 2) + pow(Gy, 2)) > 150 ? 255 : 0;
		}
		
	}
	// Display the edges as binary image
	imshow( "Binary Image", imDst );
	return imDst;
}
Mat DetectEdges()
{
	  Mat imDst = Mat::zeros(source.size(), source.type() );
	  /// Reduce noise with a kernel 3x3
	//  blur( original_image, imDst, Size(3,3) );
	  GaussianBlur( source, imDst, Size(3,3), 2, 2 );
	  imshow("Blurred Image", imDst );
	  /// Canny detector
	  Canny( imDst, imDst, 100, 200, 3 );

	  imshow("Edges detected", imDst );

	  return imDst;
}
// Find the circles
void HoughTrans(Mat &binary){
	unsigned int srcH = binary.rows;
    unsigned int srcW = binary.cols;
	
	vector<pair<int,int> >centerz;
	// threshold with canny = 230
	const int threshold = 253;

	cout<<"Min r: " << min_r << "max r: " <<max_r <<'\n';
	
	for(unsigned int r = min_r; r < max_r; r++)
	{
		centerz.clear();
		// create a hough space for each radius
		Mat hough_space = Mat::zeros(binary.size(), binary.type() );
	
		// find all the edges
		for(unsigned int x = 0; x < srcH; x++)
		{
			for(unsigned int y = 0; y < srcW ; y++)
			{
				// if an edge found, accumlate
				if(int(binary.at<uchar>(x,y)) == 255)
				{
				    Accumulate_hough(hough_space,x,y,r);
				}
			}
		}
		for(unsigned int x = 0; x < binary.rows ; x++)
		{
			for(unsigned int y = 0; y < binary.cols ; y++)
			{
				if(int(hough_space.at<uchar>(x,y)) > threshold)
				{
					centerz.push_back(make_pair(x,y));
				}
			}
		}
		Houghs[r - min_r] = centerz;
	}
	draw_circle();
}
void Accumulate_hough(Mat &hough_space,int x,int y,int radius){
    // accumlate each point in the circle
	
	const double PI = 3.14159;

	for (double angle=0; angle<=2*PI; angle+=0.01)  
     //This will have the coordinates  we want to draw a point at
	{
		Accumulate_pixel(hough_space,x + radius*cos( angle ),y + radius*sin( angle ));
    }
	
}
void Accumulate_pixel(Mat &hough_space,int x,int y){
	// make sure that the indeces are valid
	if(x >=0 && x < hough_space.rows && y >= 0 && y < hough_space.cols )
	{
		// acummulate this point by 1
		hough_space.at<uchar>(x,y)++;
	}
}
void draw_circle(){
	vector<pair<int,pair<int,int> > >final;        // a vector to hold the final circles detected
	// Eliminate circles that have close radii to solve the problem of detecting the inner circles
	for(unsigned int r = min_r; r < max_r; r++)
	{
		for(unsigned int i = 0; i < Houghs[r-min_r].size() ; i++)
		{
			bool flag = true;
			for(unsigned int t = r ; t < max_r; t++)
			{
				unsigned int j;
				if(t==r) { j = i+1; }
				else{ j = 0; }
				while(j < Houghs[t-min_r].size())
				{
						if( abs(Houghs[r-min_r][i].first - Houghs[t-min_r][j].first ) < 20  && abs(Houghs[r-min_r][i].second - Houghs[t-min_r][j].second ) < 20 ){
							flag = false;
						}
						j++;
				} 
			}
			if(flag){
				final.push_back(make_pair(r,make_pair(Houghs[r-min_r][i].first,Houghs[r-min_r][i].second)) );
			} 
			//final.push_back(make_pair(r,make_pair(Houghs[r-min_r][i].first,Houghs[r-min_r][i].second)) );
		}
	}
	for(int i=0 ; i < final.size() ; i++ ){
		Point p(final[i].second.second ,final[i].second.first );
		circle( original_image , p , final[i].first , Scalar( 255, 0, 0 ) , 2 , 8, 0);
		if(final[i].first >= min_r && final[i].first < 55){
			totalSum+= 0.25;
		}
		else if ( final[i].first >=55 && final[i].first < 60 ){
			totalSum+= 0.5;
		}
		else{
			totalSum += 1.0;
		} 
		/*if(final[i].first >= min_r && final[i].first < 58){
			totalSum+= 0.25; 
		} 
		else if ( final[i].first >=58 && final[i].first < 62 )
		{
			totalSum+= 0.5; 
		} 
		else
		{ 
			totalSum += 1.0; 
		} */
		cout<<"Circle with radius "<<final[i].first<<" and center "<<final[i].second.first<<" , "<<final[i].second.second<<'\n';
	}
	cout<<"The total sum equals:"<<totalSum<<'\n';

}

