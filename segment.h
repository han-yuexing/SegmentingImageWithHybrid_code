
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core/internal.hpp"
#include <string.h>
#include <iostream>

#ifndef SEGMENT_H
#define SEGMENT_H

#define MAX_PATH_FILENAME 200 //文件名长度
#define hogBins 9 //hog直方图bin数量
#define lbpBins 256 //lbp直方图bin数量

typedef struct ConnectedComp
{
    int area;     //面积
	std::vector<cv::Point> pointSet; //区域内的点集
	int mean; //均值
	double var; //标准差
	double hogHist[hogBins]; //hog直方图
	double lbpHist[lbpBins];
	int num; //编号
}
ConnectedComp;

struct hogPoint  //描述hog像素点的结构体
{
	//int x; //x,y坐标
	//int y;
    double ori;//梯度方向 
	double mag; //梯度幅度
};

class segmentImg{
private:
	cv::Mat image;
public:
	std::string results;
	void showImg(std::string filename);
	void segmentMain(std::string filename,int dim,int hs,int hr,int edge_mode,int edge_count,int clustering_mode,int kmeans_k,double dbscan_eps,int dbscan_min);
	void segmentMainSimple(std::string filename,int edge_mode,int edge_count,int clustering_mode,int kmeans_k,double dbscan_eps,int dbscan_min);
	int newfloodfill( cv::Mat &_image, cv::Mat &_mask,cv::Point seedPoint,int loDiff, int upDiff, int flags ,int range1,std::vector<ConnectedComp> &ccompSet);
	void floodfillPaint( cv::Mat &_result,cv::Mat &_gray,std::vector<ConnectedComp> &ccompSet);
	void outputMeanAndVar(std::vector<ConnectedComp> &ccompSet, cv::Mat &_gray);
    //void outputGrayVal(std::vector<ConnectedComp> &ccompSet,cv::Mat &_gray);
	void outputHOG(std::vector<ConnectedComp> &ccompSet,cv::Mat &_gray);
	void clustringAmerging_k(cv::Mat &_result, cv::Mat &_gray, std::vector<ConnectedComp> &ccompSet, int kmeans_k);
    void clustringAmerging_db(cv::Mat &_result, cv::Mat &_gray, std::vector<ConnectedComp> &ccompSet);
};

//int newfloodfill( Mat &_image, Mat &_mask,Point seedPoint,int loDiff, int upDiff, int flags ,int range1,vector<ConnectedComp> &ccompSet);
//void floodfillPaint( Mat &_result,Mat &_gray,vector<ConnectedComp> &ccompSet);


#endif