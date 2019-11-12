#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"  

#include "imatrix.h"
#include "ETF.h"
#include "fdog.h"
#include "myvec.h"
#include "edge_detection.h"

using namespace cv;
using namespace std;

void newFDoG(Mat grayImage, Mat image, Mat &result_image, double tao){
	//double tao = 0.99;
	double thres = 0.7;
	double sigma = 1.0;
	double sigma3 = 1.0;

	int wz = 15;
	double colSig = 15.0;
	double spaSig = 10.0;
	int iterFDoG = 1;

	
	//bilateral filtering
	Mat filterImg;
	////color sigma对降噪起了作用
	bilateralFilter(grayImage, filterImg, wz, colSig, spaSig);
	////imshow("Edge Image", grayImage);
	////imshow("bilateralImage", filterImg);
	////waitKey(0);
	grayImage = filterImg;
	

	imatrix img;
	int index;

	// flag=1 use a,b   flag=0, use I(L?)
	img.init(image.rows, image.cols);

	for (int i = 0; i < image.rows; i++) {
			img.p[i] = new int[image.cols];
			for (int j = 0; j < image.cols; j++) {
				index = i*image.cols+j;
				//printf("1: %d\n", grayImage.data[index]);
				img.p[i][j] = grayImage.data[index];
				//printf("2: %d\n", img.p[i][j]);
			}
	}

	int image_x = img.getRow();
	int image_y = img.getCol();
	ETF e;
	e.init(image_x, image_y);
	e.set(img); // get gradients from input image
	//e.set2(img); // get gradients from gradient map
	e.Smooth(4, 2);
	

	GetFDoG(img, e, sigma, sigma3, tao);

	//Iterative FDoG
	int pxVal;
	for (int i = 0; i<iterFDoG; i++)
	{
		for(int j = 0; j < img.getRow(); j++)
		{
			for(int k = 0; k < img.getCol(); k++)
			{
				pxVal = (int)img[j][k] + (int)(grayImage.data[j*img.getCol()+k]);
				if(pxVal > 255)
					pxVal = 255;
				img[j][k] = pxVal;
			}
		}
		GetFDoG(img, e, sigma, sigma3, tao);
	}
	
	GrayThresholding(img, thres); 

	Mat saveImg = grayImage;
	for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				index = i*image.cols+j;
				saveImg.data[index] = img.p[i][j];
			}
	}

	saveImg.copyTo(result_image);

	//imshow("Edge Image", saveImg);
}



void doubleThresholdEdge(Mat _grayImage, Mat image, Mat &edge_result){
	double tao1=1.0, tao2=0.96;
	Mat imageTao1, imageTao2;

	Mat grayImage1,grayImage2;
	_grayImage.copyTo(grayImage1);
	_grayImage.copyTo(grayImage2);
	newFDoG(grayImage1,image,imageTao1,tao1);
	newFDoG(grayImage2,image,imageTao2,tao2);

	//对imageTao1和2进行优化
	Mat img_binary,img_binary2;
	threshold(imageTao2,img_binary,100,255,CV_THRESH_BINARY_INV);
	threshold(imageTao1,img_binary2,100,255,CV_THRESH_BINARY_INV);

	std::vector<std::vector<cv::Point>> contours,contours2;
    cv::findContours(img_binary,contours,/*轮廓的数组*/CV_RETR_LIST,/*检索所有的轮廓，并将其放入list中*/CV_CHAIN_APPROX_NONE/*获取每个轮廓的每个像素*/);
	 cv::findContours(img_binary2,contours2,/*轮廓的数组*/CV_RETR_LIST,/*检索所有的轮廓，并将其放入list中*/CV_CHAIN_APPROX_NONE/*获取每个轮廓的每个像素*/);

	//消除小轮廓
	int cmin=30; //最小轮廓长度
	std::vector<std::vector<cv::Point>>::const_iterator itc=contours.begin();
	while(itc!=contours.end()){
			 if(itc->size()<cmin)
					itc=contours.erase(itc);
			 else
					++itc;
		  }

	std::vector<std::vector<cv::Point>>::const_iterator itc2=contours2.begin();
	while(itc2!=contours2.end()){
			 if(itc2->size()<cmin)
					itc2=contours2.erase(itc2);
			 else
					++itc2;
		  }

	//在黑色图像上绘制白色轮廓
	cv::Mat imageTao2_new(imageTao2.size(),CV_8U,Scalar(0));
	cv::Mat imageTao1_new(imageTao1.size(),CV_8U,Scalar(0));

	cv::drawContours(
		imageTao2_new,contours,
			  -1,//绘制所有轮廓（可用来指定需要绘制的轮廓的索引值）
			  cv::Scalar(255),//颜色为白色
			   -1 //轮廓线的绘制宽度
				);

	cv::drawContours(
		imageTao1_new,contours2,
			  -1,//绘制所有轮廓（可用来指定需要绘制的轮廓的索引值）
			  cv::Scalar(255),//颜色为白色
			   -1 //轮廓线的绘制宽度
				);

	//Mat imageTao1_new;//把tao1变为黑底白线
	//threshold(imageTao1,imageTao1_new,100,255,CV_THRESH_BINARY_INV);

	//imshow("edge_tao2",imageTao2_new);
	//imshow("edge_tao1",imageTao1_new);

	//----------------------------------------------------------连接双阈值--------------------
	//把tao2的边缘连成连续的轮廓    
	int Queue[999999][2] = {0};//用数组模拟队列  
	int front = 0;//队头  
	int rear = 0;//队尾  

	for(int i=0;i<imageTao2.rows;i++)
		for(int j=0;j<imageTao2.cols;j++){
			if(imageTao2_new.at<uchar>(i,j)>0){//强点入队
				Queue[rear][0]=i;
				Queue[rear][1]=j;
				rear++;
				edge_result.at<uchar>(i,j)=imageTao2_new.at<uchar>(i,j);
				imageTao2_new.at<uchar>(i,j)=0; //避免重复计算
			}

			while(front!=rear){//队不空
				//队头出队
				int temp_i=Queue[front][0];
				int temp_j=Queue[front][1];
				front++;

				if((temp_i>0)&&(temp_i<(imageTao2.rows-1))&&(temp_j>0)&&(temp_j < (imageTao2.cols-1))){ //8连通域寻找可能边缘点

					//左上方
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j-1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i-1,temp_j-1)=imageTao1_new.at<uchar>(temp_i-1,temp_j-1);
						imageTao1_new.at<uchar>(temp_i-1,temp_j-1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//正上方
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i-1,temp_j)=imageTao1_new.at<uchar>(temp_i-1,temp_j);
						imageTao1_new.at<uchar>(temp_i-1,temp_j)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j;
						rear++;
					}

					//右上方
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j+1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i-1,temp_j+1)=imageTao1_new.at<uchar>(temp_i-1,temp_j+1);
						imageTao1_new.at<uchar>(temp_i-1,temp_j+1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j+1;
						rear++;
					}

					//正左方
					if(imageTao1_new.at<uchar>(temp_i,temp_j-1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i,temp_j-1)=imageTao1_new.at<uchar>(temp_i,temp_j-1);
						imageTao1_new.at<uchar>(temp_i,temp_j-1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//正右方
					if(imageTao1_new.at<uchar>(temp_i,temp_j+1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i,temp_j+1)=imageTao1_new.at<uchar>(temp_i,temp_j+1);
						imageTao1_new.at<uchar>(temp_i,temp_j+1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i;
						Queue[rear][1]=temp_j+1;
						rear++;
					}

					//左下方
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j-1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i+1,temp_j-1)=imageTao1_new.at<uchar>(temp_i+1,temp_j-1);
						imageTao1_new.at<uchar>(temp_i+1,temp_j-1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//正下方
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i+1,temp_j)=imageTao1_new.at<uchar>(temp_i+1,temp_j);
						imageTao1_new.at<uchar>(temp_i+1,temp_j)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j;
						rear++;
					}

					//右下方
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j+1)>0){ //把在强点周围的弱点变为强点
						imageTao2_new.at<uchar>(temp_i+1,temp_j+1)=imageTao1_new.at<uchar>(temp_i+1,temp_j+1);
						imageTao1_new.at<uchar>(temp_i+1,temp_j+1)=0;//避免重复计算
						//入队
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j+1;
						rear++;
					}
				}
			} 
		} 

		//imshow("edge_result",edge_result);
	
}