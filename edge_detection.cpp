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
	////color sigma�Խ�����������
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

	//��imageTao1��2�����Ż�
	Mat img_binary,img_binary2;
	threshold(imageTao2,img_binary,100,255,CV_THRESH_BINARY_INV);
	threshold(imageTao1,img_binary2,100,255,CV_THRESH_BINARY_INV);

	std::vector<std::vector<cv::Point>> contours,contours2;
    cv::findContours(img_binary,contours,/*����������*/CV_RETR_LIST,/*�������е����������������list��*/CV_CHAIN_APPROX_NONE/*��ȡÿ��������ÿ������*/);
	 cv::findContours(img_binary2,contours2,/*����������*/CV_RETR_LIST,/*�������е����������������list��*/CV_CHAIN_APPROX_NONE/*��ȡÿ��������ÿ������*/);

	//����С����
	int cmin=30; //��С��������
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

	//�ں�ɫͼ���ϻ��ư�ɫ����
	cv::Mat imageTao2_new(imageTao2.size(),CV_8U,Scalar(0));
	cv::Mat imageTao1_new(imageTao1.size(),CV_8U,Scalar(0));

	cv::drawContours(
		imageTao2_new,contours,
			  -1,//��������������������ָ����Ҫ���Ƶ�����������ֵ��
			  cv::Scalar(255),//��ɫΪ��ɫ
			   -1 //�����ߵĻ��ƿ��
				);

	cv::drawContours(
		imageTao1_new,contours2,
			  -1,//��������������������ָ����Ҫ���Ƶ�����������ֵ��
			  cv::Scalar(255),//��ɫΪ��ɫ
			   -1 //�����ߵĻ��ƿ��
				);

	//Mat imageTao1_new;//��tao1��Ϊ�ڵװ���
	//threshold(imageTao1,imageTao1_new,100,255,CV_THRESH_BINARY_INV);

	//imshow("edge_tao2",imageTao2_new);
	//imshow("edge_tao1",imageTao1_new);

	//----------------------------------------------------------����˫��ֵ--------------------
	//��tao2�ı�Ե��������������    
	int Queue[999999][2] = {0};//������ģ�����  
	int front = 0;//��ͷ  
	int rear = 0;//��β  

	for(int i=0;i<imageTao2.rows;i++)
		for(int j=0;j<imageTao2.cols;j++){
			if(imageTao2_new.at<uchar>(i,j)>0){//ǿ�����
				Queue[rear][0]=i;
				Queue[rear][1]=j;
				rear++;
				edge_result.at<uchar>(i,j)=imageTao2_new.at<uchar>(i,j);
				imageTao2_new.at<uchar>(i,j)=0; //�����ظ�����
			}

			while(front!=rear){//�Ӳ���
				//��ͷ����
				int temp_i=Queue[front][0];
				int temp_j=Queue[front][1];
				front++;

				if((temp_i>0)&&(temp_i<(imageTao2.rows-1))&&(temp_j>0)&&(temp_j < (imageTao2.cols-1))){ //8��ͨ��Ѱ�ҿ��ܱ�Ե��

					//���Ϸ�
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j-1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i-1,temp_j-1)=imageTao1_new.at<uchar>(temp_i-1,temp_j-1);
						imageTao1_new.at<uchar>(temp_i-1,temp_j-1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//���Ϸ�
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i-1,temp_j)=imageTao1_new.at<uchar>(temp_i-1,temp_j);
						imageTao1_new.at<uchar>(temp_i-1,temp_j)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j;
						rear++;
					}

					//���Ϸ�
					if(imageTao1_new.at<uchar>(temp_i-1,temp_j+1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i-1,temp_j+1)=imageTao1_new.at<uchar>(temp_i-1,temp_j+1);
						imageTao1_new.at<uchar>(temp_i-1,temp_j+1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i-1;
						Queue[rear][1]=temp_j+1;
						rear++;
					}

					//����
					if(imageTao1_new.at<uchar>(temp_i,temp_j-1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i,temp_j-1)=imageTao1_new.at<uchar>(temp_i,temp_j-1);
						imageTao1_new.at<uchar>(temp_i,temp_j-1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//���ҷ�
					if(imageTao1_new.at<uchar>(temp_i,temp_j+1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i,temp_j+1)=imageTao1_new.at<uchar>(temp_i,temp_j+1);
						imageTao1_new.at<uchar>(temp_i,temp_j+1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i;
						Queue[rear][1]=temp_j+1;
						rear++;
					}

					//���·�
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j-1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i+1,temp_j-1)=imageTao1_new.at<uchar>(temp_i+1,temp_j-1);
						imageTao1_new.at<uchar>(temp_i+1,temp_j-1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j-1;
						rear++;
					}

					//���·�
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i+1,temp_j)=imageTao1_new.at<uchar>(temp_i+1,temp_j);
						imageTao1_new.at<uchar>(temp_i+1,temp_j)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j;
						rear++;
					}

					//���·�
					if(imageTao1_new.at<uchar>(temp_i+1,temp_j+1)>0){ //����ǿ����Χ�������Ϊǿ��
						imageTao2_new.at<uchar>(temp_i+1,temp_j+1)=imageTao1_new.at<uchar>(temp_i+1,temp_j+1);
						imageTao1_new.at<uchar>(temp_i+1,temp_j+1)=0;//�����ظ�����
						//���
						Queue[rear][0]=temp_i+1;
						Queue[rear][1]=temp_j+1;
						rear++;
					}
				}
			} 
		} 

		//imshow("edge_result",edge_result);
	
}