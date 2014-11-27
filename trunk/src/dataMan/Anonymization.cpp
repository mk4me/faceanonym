#include "Anonymization.h"
#include <iostream>

using namespace Anonimizator;


CFaceFinder::CFaceFinder(const std::string& videoName, const std::string& frontalClassifier, const std::string& profileClassifier): m_capture(videoName)
{
	m_detector= new CFaceDetector(frontalClassifier, profileClassifier);
	m_detector->start();
}

CFaceFinder::~CFaceFinder()
{
	delete m_detector;
}

std::vector<pairFrameFace> CFaceFinder::getAllDetectedFaces()
{
	std::vector<pairFrameFace> faces;
	bool endVideo = false;
	while(!endVideo)
	{
		cv::Mat frame;
		if (m_capture.read(frame))
		{
			m_detector->storeNewImage(frame);
			vecRect area = m_detector->getArea();
			faces.push_back(std::make_pair(frame.clone(),area));
			
			for (int i = 0; i< area.size(); i++)
			{
				cv::rectangle(frame,area[i],cv::Scalar(0,0,255),3);
			}
			cv::imshow("draw",frame);
			cv::waitKey(10);
		}
		else
			endVideo = true;
	}
	return faces;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
void CContours::findContoursCanny(cv::Mat& src, bool show)
{
	cv::Mat src_gray;
	cv::cvtColor(src,src_gray, CV_BGR2GRAY);
	cv::blur(src_gray, src_gray, cv::Size(3,3));

	cv::Mat edges;
	std::vector<cv::Vec4i> hierarchy;

	// Detect edges using canny
	Canny(src_gray, edges, m_threshold, m_threshold*2, 3);

	// Find contours
	findContours(edges,m_contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	if (show)
	{
		cv::Mat drawing = cv::Mat::zeros(edges.size(), CV_8UC3);
		for(int i = 0; i< m_contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(0,0,255);
			drawContours(drawing, m_contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		}
		cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		cv::imshow("Contours", drawing);
		cv::waitKey(1);
	}
}

void CContours::getContours(std::vector<cv::Point2f> &cont)
{
	std::vector<cv::Point2f> tmp;

	for (int i=0; i<m_contours.size(); i++)
	{
		for (int j=0; j<m_contours[i].size(); j++)
		{
			tmp.push_back(cv::Point2f(m_contours[i][j]));
		}
	}

	cont = tmp;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
void COpticalFlowPLK::init(std::vector<cv::Point2f> conturs)
{
	int maxCount = conturs.size();
	prevPts = conturs;
	status = (char *)cvAlloc(maxCount);
	prevImg = 0;
}


void COpticalFlowPLK::trackPosition(cv::Mat img)
{
	if (prevImg.empty())
	{
		prevImg = cv::Mat(img.rows, img.cols, CV_8UC3);
		cv::imshow("test",prevImg);
		cv::waitKey(1);
	}

}




//
//int COpticalFlowPLK::trackPosition(IplImage* gry)
//{
//	//dla pierwszego obiegu, gdy nie ma poprzedniego obrazka
//	if (!eig)
//	{
//		pgry = cvCreateImage(cvGetSize(gry),8,1);
//		pry = cvCreateImage(cvGetSize(gry),8,1);
//		ppyr = cvCreateImage(cvGetSize(gry),8,1);
//		eig = cvCreateImage(cvGetSize(gry),32,1);
//		tmp = cvCreateImage(cvGetSize(gry),32,1);
//		t = cvCreateImage(cvGetSize(gry),8,1);
//	}
//	
//	count = m_maxCount;
//	cvGoodFeaturesToTrack(gry,eig,tmp,pts_cur,&count,quality,min_distance,NULL,3,0,0.04);
//	
//
//	for(i=0; i<count;i++)
//	{
//		pts_cur_trk[i] = pts_cur[i]; // obecny
//		pts_prv_trk[i] = pts_prv[i]; // poprzedni
//	}
//
//	k=0;
//
//	if(pcount > 0 && count > 0)
//	{
//		cvCalcOpticalFlowPyrLK(pgry,gry,ppyr,pry,pts_prv_trk,pts_cur_trk,count,cvSize(11,11),3,stat,0,cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,20,0.03),flags);
//
//		flags |= CV_LKFLOW_PYR_A_READY;
//		for(i=k=0; i< count; i++)
//		{
//			if(stat[i] == 1)
//			{
//				pts_cur_trk[k] = pts_cur_trk[i];
//				pts_prv_trk[k] = pts_prv_trk[i];
//				k++;
//			}
//			
//		}
//	}
//
//	pcount = count;
//	count = k;
//	//cvShowImage("Pgry", pgry);
//	//cv::waitKey(1);
//	cvCopy(gry,pgry);
//	//cvShowImage("gry", gry);
//	//cv::waitKey(1);
//	CV_SWAP(ppyr, pry, stmp);
//	CV_SWAP(pts_prv, pts_cur,spts); //zatrzymuje wszystkie punkty
//
//	return count;
//}
//void COpticalFlowPLK::init(int maxCount)
//{
//	m_maxCount = maxCount;
//	flags = 0;
//	quality = 0.01;
//	min_distance = 10;
//	avg_blocksize = 3;
//	count = 0;
//	pcount = 0;
//	pts_cur = (CvPoint2D32f*)cvAlloc(m_maxCount* sizeof(CvPoint2D32f));
//	pts_prv = (CvPoint2D32f*)cvAlloc(m_maxCount* sizeof(CvPoint2D32f));
//	pts_cur_trk = (CvPoint2D32f*)cvAlloc(m_maxCount* sizeof(CvPoint2D32f));
//	pts_prv_trk = (CvPoint2D32f*)cvAlloc(m_maxCount* sizeof(CvPoint2D32f));
//	stat = (char *)cvAlloc(m_maxCount);
//	eig = 0;
//}/*
//
//void COpticalFlowPLK::getCurrentTrackedPosition(CvPoint2D32f **ctrk)
//{
//	*ctrk = pts_cur_trk;
//}
//
//void COpticalFlowPLK::getPreviousTrackedPosition(CvPoint2D32f **ptrk)
//{
//	*ptrk = pts_prv_trk;
//}
//
//void COpticalFlowPLK::getCount(int* c)
//{
//	*c = count;
//}
//

