/*	\brief		
	\author		Magdalena Pawlyta
	\version	1.0
	\date		27.10.2014
*/

#pragma once
#ifndef Anonimization_h__
#define Anonimization_h__
#include "faceAnonymizer.h"

namespace Anonimizator
{

	 typedef std::vector<cv::Rect> vecRect;
	 typedef std::pair<cv::Mat,vecRect> pairFrameFace;

	/*	\brief		
		\author		Magdalena Pawlyta
		\version	2.0
		\date		14.11.2014
	*/
	class CFaceFinder
	{
	public:
		/*	\brief		Constructor with a specific videoName.
			\param		videoName path to a file containing video.
			\param		frontalClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for frontal face detection.
			\param		profileClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for profile face detection.
			\attention	File with frontal and profile classifiers as the same!
		*/
		CFaceFinder(const std::string& videoName, const std::string& frontalClassifier, const std::string& profileClassifier);
	
		~CFaceFinder();

		/*	\brief		retrieves and stores the frame and the faces found
			Method		grabs video capture frame by frame; in each frame found probable area of face and return 
						this areas and frames in std::vector<std::pair<cv::Mat,std::vector<cv::Rect>>>
			\returns	vector pairs - video frame and position of found face, for whole recording	
		*/
		std::vector<pairFrameFace> getAllDetectedFaces();

	private:
		cv::VideoCapture m_capture;					//!< capture, where movie is stored.
		cv::CascadeClassifier m_clasFrontal;	//!< classifier for frontal face detection
		cv::CascadeClassifier m_clasProfile;	//!< classifier for profile face detection
		//CFaceDetector* m_detector;					//!< its thread is started in constructor, afterwards it is used for retrieving face rectangle from detector.
	};

	/*	\brief		
		\author		Magdalena Pawlyta
		\version	1.0
		\date		17.11.2014
	*/
	class CContours
	{
		public:
			/*	\brief		Constructor with a specific threshold.
				\param		threshold -  threshold for the hysteresis procedure
			*/
			CContours(int threshold):m_threshold(threshold) {};
			~CContours() {};

			/*	\brief		finds contours in a image and show it
				Method		Converts an image from a RGB image to gray; Finds edges in an image using the Canny algorithm; Finds contours in a binary image.
				\param		show - if show == true shows contours in a new window
			*/
			void findContoursCanny(cv::Mat& src, bool show = false);

			/*	\brief		return contours
				\return		vector of contours found	
			*/
			void getContours(std::vector<cv::Point2f> &);

		private:
			int m_threshold;						//!< threshold for the hysteresis procedure (use in findContours)
			std::vector<std::vector<cv::Point>> m_contours;		//!< vector of found contours
	};




	/*	\brief		
		\author		Magdalena Pawlyta
		\version	1.0
		\date		12.11.2014
	*/
	class COpticalFlowPLK
	{

		public:
			COpticalFlowPLK() {};
			~COpticalFlowPLK() {};
			void init(std::vector<cv::Point2f> conturs);
			void trackPosition(cv::Mat img);

		private:
			cv::Mat prevImg;
			std::vector<cv::Point2f> prevPts, nextPts;
			char *status;
	};

	//class  COpticalFlowPLK
	//{
	//public:
	//	COpticalFlowPLK() {};
	//	~COpticalFlowPLK() {};

	//	void init(int maxCount);
	//	int trackPosition(IplImage* gry);
	//	void getCurrentTrackedPosition(CvPoint2D32f **);
	//	void getPreviousTrackedPosition(CvPoint2D32f **);
	//	void getCount(int *);

	//	//int m_maxCount;
	//	//float m_minEigen;

	//	//int count;
	//	//CvPoint2D32f* pts_cur;
	//	//CvPoint2D32f* pts_prv;
	//	//CvPoint2D32f* pts_cur_trk;
	//	//CvPoint2D32f* pts_prv_trk;

	//private:
	///*	IplImage *eig, *tmp, *stmp, *pgry, *pry, *ppyr;
	//	IplImage *t;
	//	int win_size;

	//	CvPoint2D32f *spts, *tpts;
	//	char *stat;

	//	int flags;
	//	double quality;
	//	double min_distance;
	//	int avg_blocksize;
	//	int pcount;
	//	int i,j,k;*/
	//};

}


#endif //Anonimization_h__