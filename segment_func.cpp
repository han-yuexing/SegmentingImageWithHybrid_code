#include "stdafx.h"
#include "segment.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <fstream>

#include "edge_detection.h"

using namespace cv;
using namespace std;


//测试显示图片
void segmentImg::showImg(std::string filename){
	image = cv::imread(filename);
	cv::imshow("test",image);
	cv::waitKey(0);
}


// 分割方法Main
void segmentImg::segmentMain(std::string filename,int dim,int hs,int hr,int edge_mode,int edge_count,int clustering_mode,int kmeans_k,double dbscan_eps,int dbscan_min){

	Mat src = imread(filename,IMREAD_UNCHANGED); //读入原始图像

	Mat gray_img, color_img,color_temp;
	int width=660, height;
	float rows_temp=src.rows;
	float cols_temp=src.cols;
	float size_rate = cols_temp / width; //缩放比例
	height = (rows_temp / size_rate)/100+0.099;
	height = height*100;

	//width=660; height=500;
	//resize(src,gray_img,Size(width,height));   //如果以灰度图模式读取原图像的话
	//cvtColor(gray_img,color_img,CV_GRAY2BGR);

	if(src.channels() == 1){ //假如为单通道灰度图
	   resize(src,gray_img,Size(width,height));
	   cvtColor(gray_img,color_img,CV_GRAY2BGR);
	}
	else if(src.channels() == 3){ //假如为三通道彩图
	   resize(src,color_img,Size(width,height));
	   cvtColor(color_img,gray_img,CV_BGR2GRAY);
	}
	  else if(src.channels() == 4){ //假如为四通道彩图
	   resize(src,color_temp,Size(width,height));
	   cvtColor(color_temp,color_img,CV_BGRA2BGR);
	   cvtColor(color_img,gray_img,CV_BGR2GRAY);
	}

//cout<<"开始初步分割..."<<endl;
//--------------------------------------------------------------------------------------------------------分割方法第一部分
	//高斯滤波
	Mat filter_img;  
	GaussianBlur(color_img, filter_img, Size( dim,dim ), 0, 0);

	////meanshift
    Mat meanshift_img; //分割后图像
    int maxPyrLevel = 0;  //金字塔层数
	pyrMeanShiftFiltering( filter_img, meanshift_img, hs, hr, maxPyrLevel); //meanshift :空间窗口大小hs,色彩窗口大小hr

	Mat meanshift_gray;
	cvtColor(meanshift_img,meanshift_gray,CV_BGR2GRAY);
	//imshow("meanshift_gray",meanshift_gray); //画meanshift结果
	imwrite("results\\meanshift_result_gray.png",meanshift_gray);
	//cvWaitKey(0);

	//Mat meanshift_gray=imread("meanshift_result_gray.png",0);  //测试，跳过前面meanshift步骤
	//if(meanshift_gray.empty())
	//	exit(0);
	
    Mat mask( meanshift_gray.rows, meanshift_gray.cols, CV_8UC1, Scalar::all(0) );  //掩模, 填充模板

	//--------------------加入边缘检测---------------
	//Mat edge2(gray_img.size(),CV_8U,Scalar(0));
	//newFDoG(gray_img,color_img,edge2,0.99);//测试
	//doubleThresholdEdge(gray_img,color_img,edge2); //测试

	if(edge_mode == 1){
		Mat edge_result(gray_img.size(),CV_8U,Scalar(0));
		if(edge_count == 0){ //假如是第一次边缘检测
			doubleThresholdEdge(gray_img,color_img,edge_result);
			imwrite("results\\EdgeFresult.png",edge_result);
			edge_count++;
		}else{//不是第一次可以直接读取上次的检测结果
			Mat edge_result = imread("results\\EdgeFresult.png",0); //读入边缘图像
		}
		for(int i=0;i<edge_result.rows;i++)
			for(int j=0;j<edge_result.cols;j++){
				if(edge_result.at<uchar>(i,j)!=0){
					mask.at<uchar>(i,j)=255;
				}
			}
	}

	//FloodFill
	float area_block = meanshift_gray.rows * meanshift_gray.cols;
	//int range_area=area_block * 0.0003 + 0.5;  //用于抛弃过小的blocks,要改进
	int range_area=100; //50,100,200  

	vector<ConnectedComp> ccompSet; //blocks集

	//int blocks_area=0;
	int flag_floodfill = 8;
    for( int y = 1; y < meanshift_gray.rows-1; y++ )
    {  
        for( int x = 1; x < meanshift_gray.cols-1; x++ )  
        {  
            if( mask.at<uchar>(y, x) == 0 )  //非0处即为1，表示已经经过填充，不再处理  
            {   
				newfloodfill( meanshift_gray, mask, Point(x,y), 5, 5, flag_floodfill ,range_area,ccompSet);
				//std::cout<<blocks_area<<"  ,";
            }  
        }  
    }  
	//imshow("mask",mask);
	//cout<<ccompSet.size();
	
	//画floodfill结果-----------检验第一部分结果------------------------
	Mat result_floodfill(meanshift_gray.rows, meanshift_gray.cols, CV_8UC3, Scalar::all(0) );
	floodfillPaint(result_floodfill,gray_img,ccompSet); 
	imwrite("results\\flood.png",result_floodfill);
	//imshow("floodfill_result",result_floodfill);
	//waitKey(0);
	

	
	//-------------------------------------------------------------------------分割方法第一部分结束，开始第二部分
	//-------------------------------------------------------------------------------------开始提取特征，进行分类

	//输出特征
	outputMeanAndVar(ccompSet,gray_img);
	//outputGrayVal(ccompSet,gray_img);
	outputHOG(ccompSet,gray_img);
	
	
	//..................................................结合python脚本部分..................................
	Mat result_clustering(meanshift_gray.rows, meanshift_gray.cols, CV_8UC3, Scalar::all(0) );
	//默认为kmeans
	if(clustering_mode == 1){
	 ofstream poi("results\\kmeansvar.txt");
	 if(!poi) return; 
	 poi<<kmeans_k<<'\n'<<2;
	 poi.close();

	 system("python kmeans.py");
	 clustringAmerging_k(result_clustering, gray_img, ccompSet,kmeans_k);
	}
	else if(clustering_mode == 2){

	 ofstream poi("results\\dbvar.txt");
	 if(!poi) return; 
	 poi<<dbscan_eps<<'\n'<<dbscan_min;
	 poi.close();

	 system("python dbscan.py");
	 clustringAmerging_db(result_clustering, gray_img, ccompSet);
	}
	
	//cout<<"分类结束，显示结果..."<<endl;
	//-------------------------------------------------------------------------------------------
	//cv::imshow("src",src);
	//cv::imshow("result_clustering",result_clustering);
	cv::imwrite("results\\result_clustering.png",result_clustering);
    //waitKey(0);

	
}


//如果第一部分参数不变的话，利用中间结果的分割过程
void segmentImg::segmentMainSimple(std::string filename,int edge_mode,int edge_count,int clustering_mode,int kmeans_k,double dbscan_eps,int dbscan_min){
	
	Mat src = imread(filename,IMREAD_UNCHANGED); //读入原始图像
	//Mat src = imread("图片5.png",0);  //读入灰度图像

	Mat gray_img, color_img,color_temp;
	int width=660, height;
	float rows_temp=src.rows;
	float cols_temp=src.cols;
	float size_rate = cols_temp / width; //缩放比例
	height = (rows_temp / size_rate)/100+0.099;
	height = height*100;

	//resize(src,gray_img,Size(width,height));   //如果以灰度图模式读取原图像的话
	//cvtColor(gray_img,color_img,CV_GRAY2BGR);

	if(src.channels() == 1){ //假如为单通道灰度图
	   resize(src,gray_img,Size(width,height));
	   cvtColor(gray_img,color_img,CV_GRAY2BGR);
	}
	else if(src.channels() == 3){ //假如为三通道彩图
	   resize(src,color_img,Size(width,height));
	   cvtColor(color_img,gray_img,CV_BGR2GRAY);
	}
	  else if(src.channels() == 4){ //假如为四通道彩图
	   resize(src,color_temp,Size(width,height));
	   cvtColor(color_temp,color_img,CV_BGRA2BGR);
	   cvtColor(color_img,gray_img,CV_BGR2GRAY);
	}
    //----------------------------------------------------------------------------------------------------分割第一部分开始

	Mat meanshift_gray=imread("results\\meanshift_result_gray.png",0);  //测试，跳过前面meanshift步骤
	if(meanshift_gray.empty() || gray_img.empty())
		return;
	
    Mat mask( meanshift_gray.rows, meanshift_gray.cols, CV_8UC1, Scalar::all(0) );  //掩模, 填充模板

	//--------------------加入边缘检测---------------
	//Mat edge2(gray_img.size(),CV_8U,Scalar(0));
	//newFDoG(gray_img,color_img,edge2,0.99);//测试
	//doubleThresholdEdge(gray_img,color_img,edge2); //测试

	if(edge_mode == 1){
		Mat edge_result(gray_img.size(),CV_8U,Scalar(0));
		if(edge_count == 0){ //假如是第一次边缘检测
			doubleThresholdEdge(gray_img,color_img,edge_result);
			imwrite("results\\EdgeFresult.png",edge_result);
			edge_count++;
		}else{//不是第一次可以直接读取上次的检测结果
			Mat edge_result = imread("results\\EdgeFresult.png",0); //读入边缘图像
		}
		for(int i=0;i<edge_result.rows;i++)
			for(int j=0;j<edge_result.cols;j++){
				if(edge_result.at<uchar>(i,j)!=0){
					mask.at<uchar>(i,j)=255;
				}
			}
	}

	//FloodFill
	float area_block = meanshift_gray.rows * meanshift_gray.cols;
	//int range_area=area_block * 0.0003 + 0.5;  //用于抛弃过小的blocks,要改进
	int range_area=100; //50,100,200
	vector<ConnectedComp> ccompSet; //blocks集

	//int blocks_area=0;
	int flag_floodfill = 8;
    for( int y = 1; y < meanshift_gray.rows-1; y++ )
    {  
        for( int x = 1; x < meanshift_gray.cols-1; x++ )  
        {  
            if( mask.at<uchar>(y, x) == 0 )  //非0处即为1，表示已经经过填充，不再处理  
            {   
				newfloodfill( meanshift_gray, mask, Point(x,y), 5, 5, flag_floodfill ,range_area,ccompSet);
				//std::cout<<blocks_area<<"  ,";
            }  
        }  
    }  
	//imshow("mask",mask);
	//cout<<ccompSet.size();
	
	//画floodfill结果-----------检验第一部分结果------------------------
	Mat result_floodfill(meanshift_gray.rows, meanshift_gray.cols, CV_8UC3, Scalar::all(0) );
	floodfillPaint(result_floodfill,gray_img,ccompSet);
	imwrite("results\\flood.png",result_floodfill);
	//imshow("floodfill_result",result_floodfill);
	//waitKey(0);
	

	
	//-------------------------------------------------------------------------分割方法第一部分结束，开始第二部分
	//-------------------------------------------------------------------------------------开始提取特征，进行分类

	//输出特征
	outputMeanAndVar(ccompSet,gray_img);
	//outputGrayVal(ccompSet,gray_img);
	outputHOG(ccompSet,gray_img);
	
	
	//..................................................结合python脚本部分..................................
	Mat result_clustering(meanshift_gray.rows, meanshift_gray.cols, CV_8UC3, Scalar::all(0) );
	//默认为kmeans
	if(clustering_mode == 1){
	 ofstream poi("results\\kmeansvar.txt");
	 if(!poi) return; 
	 poi<<kmeans_k<<'\n'<<2;
	 poi.close();

	 system("python kmeans.py");
	 clustringAmerging_k(result_clustering, gray_img, ccompSet,kmeans_k);
	}
	else if(clustering_mode == 2){

	 ofstream poi("results\\dbvar.txt");
	 if(!poi) return; 
	 poi<<dbscan_eps<<'\n'<<dbscan_min;
	 poi.close();

	 system("python dbscan.py");
	 clustringAmerging_db(result_clustering, gray_img, ccompSet);
	}
	
	//cout<<"分类结束，显示结果..."<<endl;
	//-------------------------------------------------------------------------------------------
	//cv::imshow("src",src);
	//cv::imshow("result_clustering",result_clustering);
	cv::imwrite("results\\result_clustering.png",result_clustering);
    //waitKey(0);
}





//-------------------------------------------
//--------------------------------------------------------递归floodFill,8邻域，绝对值小于一个范围
int floodFilldigui8(Mat &_image,Point sPoint,int loDiff,int upDiff, ConnectedComp &ccomp, int flags,Mat &_mask){

	if((sPoint.x !=0) && (sPoint.x != _image.cols-1) &&(sPoint.y !=0) &&(sPoint.y !=_image.rows-1)&&(_mask.at<uchar>(sPoint.y, sPoint.x) == 0)){
	    
		ccomp.pointSet.push_back(sPoint);
		ccomp.area += 1;
		_mask.at<uchar>(sPoint.y, sPoint.x)=255;

		if(abs((int(_image.at<uchar>(sPoint.y-1,sPoint.x-1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x)))) <= upDiff)
			floodFilldigui8(_image,Point(sPoint.x-1,sPoint.y-1),loDiff,upDiff, ccomp, flags,_mask);
		
		if(abs((int(_image.at<uchar>(sPoint.y-1,sPoint.x)))-(int(_image.at<uchar>(sPoint.y,sPoint.x)))) <= upDiff)
			floodFilldigui8(_image,Point(sPoint.x,sPoint.y-1),loDiff,upDiff, ccomp, flags,_mask);
		
		if(abs((int(_image.at<uchar>(sPoint.y-1,sPoint.x+1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui8(_image,Point(sPoint.x+1,sPoint.y-1),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y,sPoint.x-1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui8(_image,Point(sPoint.x-1,sPoint.y),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y,sPoint.x+1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x)))) <= upDiff)
		floodFilldigui8(_image,Point(sPoint.x+1,sPoint.y),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y+1,sPoint.x-1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui8(_image,Point(sPoint.x-1,sPoint.y+1),loDiff,upDiff, ccomp, flags,_mask); 

		if(abs((int(_image.at<uchar>(sPoint.y+1,sPoint.x)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui8(_image,Point(sPoint.x,sPoint.y+1),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y+1,sPoint.x+1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui8(_image,Point(sPoint.x+1,sPoint.y+1),loDiff,upDiff, ccomp, flags,_mask);

		//return 0;

	}
	else return 0;
}


//-------------------------------------------
//--------------------------------------------------------递归floodFill,4邻域，绝对值小于一个范围
int floodFilldigui4(Mat &_image,Point sPoint,int loDiff,int upDiff, ConnectedComp &ccomp, int flags,Mat &_mask){

	if((sPoint.x !=0) && (sPoint.x != _image.cols-1) &&(sPoint.y !=0) &&(sPoint.y !=_image.rows-1)&&(_mask.at<uchar>(sPoint.y, sPoint.x) == 0)){
	    
		ccomp.pointSet.push_back(sPoint);
		ccomp.area += 1;
		_mask.at<uchar>(sPoint.y, sPoint.x)=255;
		
		if(abs((int(_image.at<uchar>(sPoint.y-1,sPoint.x)))-(int(_image.at<uchar>(sPoint.y,sPoint.x)))) <= upDiff)
			floodFilldigui4(_image,Point(sPoint.x,sPoint.y-1),loDiff,upDiff, ccomp, flags,_mask);
		

		if(abs((int(_image.at<uchar>(sPoint.y,sPoint.x-1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui4(_image,Point(sPoint.x-1,sPoint.y),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y,sPoint.x+1)))-(int(_image.at<uchar>(sPoint.y,sPoint.x)))) <= upDiff)
		floodFilldigui4(_image,Point(sPoint.x+1,sPoint.y),loDiff,upDiff, ccomp, flags,_mask);

		if(abs((int(_image.at<uchar>(sPoint.y+1,sPoint.x)))-(int(_image.at<uchar>(sPoint.y,sPoint.x))))<=upDiff)
		floodFilldigui4(_image,Point(sPoint.x,sPoint.y+1),loDiff,upDiff, ccomp, flags,_mask);

		//return 0;

	}
	else return 0;
}



//---------------------------------------------------------------------------------------------------FloodFill
//--------------------------------------------------------------------------------------------------排除掉block面积过小的
int segmentImg::newfloodfill( Mat &_image, Mat &_mask,
                   Point seedPoint,
				   int loDiff, int upDiff, int flag_floodfill ,int range_area,vector<ConnectedComp> &ccompSet)
{
    ConnectedComp ccompSingle;
	ccompSingle.area=0;


	if(flag_floodfill == 8)
         floodFilldigui8(_image, seedPoint, loDiff, upDiff, ccompSingle, flag_floodfill, _mask);
	else if(flag_floodfill == 4)
		 floodFilldigui4(_image, seedPoint, loDiff, upDiff, ccompSingle, flag_floodfill, _mask);

	
	if(ccompSingle.area>range_area) //面积大于阈值
		ccompSet.push_back(ccompSingle);
	
    return cvRound(ccompSingle.area);
}


void segmentImg::floodfillPaint( Mat &_result,Mat &_gray,vector<ConnectedComp> &ccompSet){
	RNG rng = theRNG();  
	vector<ConnectedComp> ccompSetResult; //floodfill最终结果

	for(int n=0;n<ccompSet.size();n++){
	Vec3b newVal( rng(256), rng(256), rng(256) ); 
	           for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画floodfill结果
		           _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=newVal;
	            }
	}
}

//-----------------------------------------------------------------------------------------------特征提取、聚类相关函数
//-----------------------------------------------------------------------输出均值和方差到txt
void segmentImg::outputMeanAndVar(vector<ConnectedComp> &ccompSet, Mat &_gray){
	//-----------------------------------------------算标准差、均值
	double maxVar = 0;
	for(int n=0;n<ccompSet.size();n++){
	
		int count =0,sum =0;
		for(int i=0; i<ccompSet[n].pointSet.size();i++){
		    sum+=(int)_gray.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x);
				count+=1;
		}

		ccompSet[n].mean = sum/count;
		sum =0;
		for(int i=0; i<ccompSet[n].pointSet.size();i++){
			sum+=pow(((int)_gray.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)-ccompSet[n].mean),2);
			}

		ccompSet[n].var = sqrt(sum/count);

		if(maxVar<ccompSet[n].var)
			maxVar=ccompSet[n].var;
	}
	//std::cout<<maxVar<<",";


	//输出特征值
	ofstream poi("results\\mean.txt");
	if(!poi)return; 
	for(int i=0;i<ccompSet.size();i++)
		poi<<ccompSet[i].mean<<' '<<'\n';
	poi.close();

	ofstream poi2("results\\var.txt");
	if(!poi2)return; 
	for(int i=0;i<ccompSet.size();i++)
		poi2<<ccompSet[i].var<<' '<<'\n';
	poi2.close();
}



////----------------------------------------------------------------------------输出区域灰度值到txt
//void segmentImg::outputGrayVal(vector<ConnectedComp> &ccompSet,Mat &_gray){
//	char szFileName[MAX_PATH_FILENAME];
//
//	for(int num =0;num<ccompSet.size();num++){
//		sprintf(szFileName, "feature/grayVal%d.txt", num);
//		ofstream poi2(szFileName);
//		if(!poi2)return;
//	    for(int i = 0;i<ccompSet[num].pointSet.size();i++)
//          {    
//		   poi2<<(int)_gray.at<uchar>(ccompSet[num].pointSet[i].y,ccompSet[num].pointSet[i].x)<<' ';
//           }
//	poi2<<'\n';
//	poi2.close();
//	}
//}

//----------------------------------------------------------------------------输出区域HOG特征到txt
void segmentImg::outputHOG(vector<ConnectedComp> &ccompSet,Mat &_gray){
	ofstream poi3("results\\hogHist.txt");
    if(!poi3)return;

	for(int n=0;n<ccompSet.size();n++){

		for(int i=0;i<hogBins;i++) //初始化直方图
			ccompSet[n].hogHist[i]=0.0;

		double step_ori=2*CV_PI/hogBins; 

		for(int m=0;m<ccompSet[n].pointSet.size();m++){
			hogPoint hPoint_temp;
			//hPoint_temp.x = ccompSet[n].pointSet[m].x;
			//hPoint_temp.y = ccompSet[n].pointSet[m].y;
			hPoint_temp.mag = 0.;
			hPoint_temp.ori = 0.;

			//计算像素的梯度大小和梯度方向
			int x_temp = ccompSet[n].pointSet[m].x;
			int y_temp = ccompSet[n].pointSet[m].y;

			double dx = _gray.at<uchar>(y_temp,x_temp+1)-_gray.at<uchar>(y_temp,x_temp-1);
		    double dy = _gray.at<uchar>(y_temp+1,x_temp)-_gray.at<uchar>(y_temp-1,x_temp);
		
			hPoint_temp.mag = sqrt(dx*dx + dy*dy) / ccompSet[n].pointSet.size(); //梯度大小
		
		    //atan2返回[-Pi, Pi]的弧度值
			hPoint_temp.ori = atan2(dy, dx);   //梯度方向，于x轴正方向夹角
			if(hPoint_temp.ori<0)
				hPoint_temp.ori += 2*CV_PI;//转换成0~2PI

			//poi3<<hPoint_temp.mag<<','<<hPoint_temp.ori<<'\n';//测试结果一

			//统计梯度直方图
			for(int i=0;i<hogBins;i++)
				if( ( hPoint_temp.ori==(i*step_ori) || hPoint_temp.ori>(i*step_ori) ) && (hPoint_temp.ori<((i+1)*step_ori)) )
					ccompSet[n].hogHist[i] += hPoint_temp.mag; 

			
		}

		for(int i=0;i<hogBins;i++)
			poi3<<ccompSet[n].hogHist[i]<<' ';//测试结果二
		poi3<<'\n';

		//poi3.close();
	}
	poi3.close();
}


//-------------------------------------------------等比例缩放

void resizeRatio(Mat &src,Mat &dst,int dst_w,int dst_h){
	int widthSrc = src.cols;  
	int heightSrc = src.rows;  
  
    float t = 1.;                                             
  
    //int dst_w = 130, dst_h = 130; // 长宽不能都大于原图尺寸
  
    float len = t * std::max(widthSrc, heightSrc);           
    float len_ = std::min(dst_w, dst_h);          
      
    float ratio = len_ / len;                             
  
    int w_ = ratio * widthSrc;                                   
    int h_ = ratio * heightSrc;  
    int delta_x = (dst_w - w_) / (2 * ratio);     
    int delta_y = (dst_h - h_) / (2 * ratio);     
  
    for (int i=0; i<widthSrc; i++)  
    {  
        for (int j=0; j<heightSrc; j++)  
        {  
            float u = (i + delta_x) / len ;  
            float v = (j + delta_y) / len  ;  
  
            int x = u * len_;  
            int y = v * len_;  
  
			dst.at<uchar>(y, x) = src.at<uchar>(j, i);  
        }  
    }
}


//----------------------------------------------------------如果选择kmeans的话
void segmentImg::clustringAmerging_k(Mat &_result, Mat &_gray, vector<ConnectedComp> &ccompSet, int kmeans_k){
	
	ifstream in("results\\kmeans_labels.txt"); //读入标签文件
	int *flagCluster = new int[ccompSet.size()];//把标签存入一个数组
    
	for (int i = 0; i < ccompSet.size(); i++) {
            in >> flagCluster[i];
        }

	in.close();

	if(kmeans_k == 2){//加入类别数目是2
	int mean0 = 0, mean1 = 0, count_mean0=0, count_mean1=0;
	for (int i = 0; i < ccompSet.size(); i++) {  //
		if(flagCluster[i] == 0){
			mean0 += ccompSet[i].mean;
			count_mean0++;
		}
		else if(flagCluster[i] == 1)
		{
			mean1 += ccompSet[i].mean;
			count_mean1++;
		}
	}
	mean0=mean0/count_mean0;
	mean1=mean1/count_mean1;

	Vec3b newValBai(255,255,255);
	Vec3b newValHei(0,0,0);
	Vec3b newVal0, newVal1;
	int mergeflag;
	if(mean0 > mean1){
		newVal0 = newValBai; 
		newVal1 = newValHei;
		mergeflag=0;
	}
	else
	{
		newVal0 = newValHei; 
		newVal1 = newValBai;
		mergeflag=1;
	}
	for(int n=0;n<ccompSet.size();n++){
		       if(flagCluster[n] == 0)
	              for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
		            //_result.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x)=_gray.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x);
		           _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=newVal0;
	              }
			   else
				   for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
		            //_result.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x)=_gray.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x);
		           _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=newVal1;
	               }
	}
	}else{ //加入kmeans的k多于2，即类别数目多于2

		RNG rng = theRNG();  
		Vec3b *colorGroup = new Vec3b[kmeans_k]; //颜色数组
		for(int i=0; i<kmeans_k; i++){
			Vec3b newVal( rng(256), rng(256), rng(256) );
			colorGroup[i]=newVal;
		}

		for(int n=0;n<ccompSet.size();n++){
			for(int j=0;j<kmeans_k;j++)
		       if(flagCluster[n] == j)
	              for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
					  _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=colorGroup[j];
	              }
		}
		//delete []colorGroup;

	}

	//---------------------------------------提取块并进行排列----------------------------------------------------
	double *sum_block=new double[kmeans_k]; 
    for(int i=0;i<kmeans_k;i++) //初始化
        sum_block[i]=0;
	int *count_block= new int[kmeans_k];
	for(int i=0;i<kmeans_k;i++)
        count_block[i]=0;

	for (int i = 0; i<ccompSet.size(); i++){  //统计每一类的块数
	    for(int j=0; j<kmeans_k;j++)
	        if(flagCluster[i] == j)  //.........
		        sum_block[j]++;
    }

	//固定每行10/5个,每个130*130
	int widthMerge = 650; //650；1300

	int *heightMerge= new int[kmeans_k];
	for(int i=0;i<kmeans_k;i++)
        heightMerge[i]=int(sum_block[i]/5 + 0.99) * 130; //5；10

	Mat *mergeImg= new Mat[kmeans_k]; //合成图数组
	for(int i=0;i<kmeans_k;i++){
        Mat mergeImgTemp( heightMerge[i], widthMerge, CV_8UC1, Scalar::all(0) );
	    mergeImgTemp.copyTo(mergeImg[i]);
	}


	for(int n=0;n<ccompSet.size();n++){
	   Mat middle( _gray.rows, _gray.cols, CV_8UC1, Scalar::all(0) ); //中间图
	   for(int k=0; k<kmeans_k;k++)
	       if(flagCluster[n] == k){  //...................
	           for(int i=0; i<ccompSet[n].pointSet.size();i++){ 
			       middle.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x) = _gray.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x);
	              }

				  cv::Rect r0=cv::boundingRect(cv::Mat(ccompSet[n].pointSet));

				  cv::Mat imageROI;
		          imageROI=middle(r0);
		          Mat smallArea( 130, 130, CV_8UC1, Scalar::all(0) );

				  if(r0.height>=130 || r0.width>=130){
		            resizeRatio(imageROI,smallArea,130,130);
		            }
		           else{
			       for(int y1=64-(imageROI.rows/2),y2=0;y1<64-(imageROI.rows/2)+imageROI.rows;y1++,y2++)
				      for(int x1=64-(imageROI.cols/2),x2=0;x1<64-(imageROI.cols/2)+imageROI.cols;x1++,x2++)
					    smallArea.at<uchar>(y1,x1)=imageROI.at<uchar>(y2,x2);
		          }

	              int yRange = (count_block[k])/5; //10/5.。。。。。。。。。。。。。。。。。。。。。。
		          int xRange = (count_block[k])%5; //10/5。。。。。。。。。。。。。。。。。。。。。

		          for(int y=(yRange*130),y2 =0;y<((yRange*130)+130);y++,y2++)
			         for(int x=(xRange*130),x2 =0;x<((xRange*130)+130);x++,x2++){
				        mergeImg[k].at<uchar>(y,x) = smallArea.at<uchar>(y2,x2);
			          }

                  count_block[k]++;
			   }
	}

	//imshow("a",mergeImg[0]);
	//imshow("b",mergeImg[1]);
	char windowName[50];
	char string_temp[30];
	sprintf_s(string_temp, "该图像中的金相共有 %d 类: ", kmeans_k);
	results = string_temp;

	for(int i=1;i<=kmeans_k;i++){
		int j=sum_block[i-1];
		//sprintf_s(windowName, "component %d", i);
		//imshow(windowName,mergeImg[(i-1)]);
		sprintf_s(windowName, "results\\component %d.png", i);
		sprintf_s(string_temp, "第 %d 类金相数目为 %d 个; ", i,j);
		//sprintf_s(string_temp+j, "第 %d 类金相数目为 %d 个\n", i,sum_block[i-1]);
		results=results+string_temp;
		imwrite(windowName,mergeImg[(i-1)]);
	}

	delete []flagCluster;
	delete []count_block;
	delete []heightMerge;
	delete []mergeImg;
	//imwrite("a.png",mergeImg[0]);
	//imwrite("b.png",mergeImg[1]);

}

//---------------------------------------------------------------如果选择dbscan的话
void segmentImg::clustringAmerging_db(Mat &_result, Mat &_gray, vector<ConnectedComp> &ccompSet){
	ifstream in("results\\dbscan_labels.txt"); //读入标签文件
	int *flagCluster = new int[ccompSet.size()];//把标签存入一个数组
    
	for (int i = 0; i < ccompSet.size(); i++) {
            in >> flagCluster[i];
        }

	ifstream in2("results\\dbscan_nlabels.txt"); //读入dbscan类别数
	int dbscan_n;
	in2>>dbscan_n;

	in.close();
	in2.close();

	if(dbscan_n == 2){//加入类别数目是2
	int mean0 = 0, mean1 = 0, count_mean0=0, count_mean1=0;
	for (int i = 0; i < ccompSet.size(); i++) {  //
		if(flagCluster[i] == 0){
			mean0 += ccompSet[i].mean;
			count_mean0++;
		}
		else if(flagCluster[i] == 1)
		{
			mean1 += ccompSet[i].mean;
			count_mean1++;
		}
	}
	mean0=mean0/count_mean0;
	mean1=mean1/count_mean1;

	Vec3b newValBai(255,255,255);
	Vec3b newValHei(0,0,0);
	Vec3b newVal0, newVal1;
	int mergeflag;
	if(mean0 > mean1){
		newVal0 = newValBai; 
		newVal1 = newValHei;
		mergeflag=0;
	}
	else
	{
		newVal0 = newValHei; 
		newVal1 = newValBai;
		mergeflag=1;
	}
	for(int n=0;n<ccompSet.size();n++){
		       if(flagCluster[n] == 0)
	              for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
		            //_result.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x)=_gray.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x);
		           _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=newVal0;
	              }
			   else
				   for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
		            //_result.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x)=_gray.at<uchar>(ccomp.pointSet[i].y,ccomp.pointSet[i].x);
		           _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=newVal1;
	               }
	}
	}else{ //加入kmeans的k多于2，即类别数目多于2

		RNG rng = theRNG();  
		Vec3b *colorGroup = new Vec3b[dbscan_n]; //颜色数组
		for(int i=0; i<dbscan_n; i++){
			Vec3b newVal( rng(256), rng(256), rng(256) );
			colorGroup[i]=newVal;
		}

		for(int n=0;n<ccompSet.size();n++){
			for(int j=0;j<dbscan_n;j++)
		       if(flagCluster[n] == j)
	              for(int i=0; i<ccompSet[n].pointSet.size();i++){ //画结果图
					  _result.at<cv::Vec3b>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x)=colorGroup[j];
	              }
		}
		delete []colorGroup;
	}

	//---------------------------------------提取块并进行排列----------------------------------------------------
	double *sum_block=new double[dbscan_n]; 
    for(int i=0;i<dbscan_n;i++) //初始化
        sum_block[i]=0;
	int *count_block= new int[dbscan_n];
	for(int i=0;i<dbscan_n;i++)
        count_block[i]=0;

	for (int i = 0; i<ccompSet.size(); i++){  //统计每一类的块数
	    for(int j=0; j<dbscan_n;j++)
	        if(flagCluster[i] == j)  //.........
		        sum_block[j]++;
    }

	//固定每行10/5个,每个130*130
	int widthMerge = 650; //1300/650

	int *heightMerge= new int[dbscan_n];
	for(int i=0;i<dbscan_n;i++)
        heightMerge[i]=int(sum_block[i]/5 + 0.99) * 130;

	Mat *mergeImg= new Mat[dbscan_n]; //合成图数组
	for(int i=0;i<dbscan_n;i++){
        Mat mergeImgTemp( heightMerge[i], widthMerge, CV_8UC1, Scalar::all(0) );
	    mergeImgTemp.copyTo(mergeImg[i]);
	}


	for(int n=0;n<ccompSet.size();n++){
	   Mat middle( _gray.rows, _gray.cols, CV_8UC1, Scalar::all(0) ); //中间图
	   for(int k=0; k<dbscan_n;k++)
	       if(flagCluster[n] == k){  //...................
	           for(int i=0; i<ccompSet[n].pointSet.size();i++){ 
			       middle.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x) = _gray.at<uchar>(ccompSet[n].pointSet[i].y,ccompSet[n].pointSet[i].x);
	              }

				  cv::Rect r0=cv::boundingRect(cv::Mat(ccompSet[n].pointSet));

				  cv::Mat imageROI;
		          imageROI=middle(r0);
		          Mat smallArea( 130, 130, CV_8UC1, Scalar::all(0) );

				  if(r0.height>=130 || r0.width>=130){
		            resizeRatio(imageROI,smallArea,130,130);
		            }
		           else{
			       for(int y1=64-(imageROI.rows/2),y2=0;y1<64-(imageROI.rows/2)+imageROI.rows;y1++,y2++)
				      for(int x1=64-(imageROI.cols/2),x2=0;x1<64-(imageROI.cols/2)+imageROI.cols;x1++,x2++)
					    smallArea.at<uchar>(y1,x1)=imageROI.at<uchar>(y2,x2);
		          }

	              int yRange = (count_block[k])/5; //10/5
		          int xRange = (count_block[k])%5; //10/5

		          for(int y=(yRange*130),y2 =0;y<((yRange*130)+130);y++,y2++)
			         for(int x=(xRange*130),x2 =0;x<((xRange*130)+130);x++,x2++){
				        mergeImg[k].at<uchar>(y,x) = smallArea.at<uchar>(y2,x2);
			          }

                  count_block[k]++;
			   }
	}

	//imshow("a2",mergeImg[0]);
	//imshow("b2",mergeImg[1]);
	char windowName[50];
	char string_temp[30];
	sprintf_s(string_temp, "该图像中的金相共有 %d 类: ", dbscan_n);
	results = string_temp;

	for(int i=1;i<=dbscan_n;i++){
		int j=sum_block[i-1];
		//sprintf_s(windowName, "component %d", i);
		//imshow(windowName,mergeImg[(i-1)]);
		sprintf_s(windowName, "results\\component %d.png", i);
		sprintf_s(string_temp, "第 %d 类金相数目为 %d 个; ", i,j);
		results=results+string_temp;
		imwrite(windowName,mergeImg[(i-1)]);
	}


	delete []flagCluster;
	delete []count_block;
	delete []heightMerge;
	delete []mergeImg;
}