
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core/internal.hpp"
#include <string.h>
#include <iostream>

#ifndef SEGMENT_H
#define SEGMENT_H

#define MAX_PATH_FILENAME 200 //�ļ�������
#define hogBins 9 //hogֱ��ͼbin����
#define lbpBins 256 //lbpֱ��ͼbin����

typedef struct ConnectedComp
{
    int area;     //���
	std::vector<cv::Point> pointSet; //�����ڵĵ㼯
	int mean; //��ֵ
	double var; //��׼��
	double hogHist[hogBins]; //hogֱ��ͼ
	double lbpHist[lbpBins];
	int num; //���
}
ConnectedComp;

struct hogPoint  //����hog���ص�Ľṹ��
{
	//int x; //x,y����
	//int y;
    double ori;//�ݶȷ��� 
	double mag; //�ݶȷ���
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