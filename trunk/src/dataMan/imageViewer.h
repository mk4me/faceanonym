/*	\brief		
	\author		Micha³ Cogiel
	\version	1.0
	\date		21-01-2015
*/

#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"

namespace ImgViewer
{
	typedef std::vector<cv::Mat> VecFrameType;
	class CImgViewer
	{
	public:
		CImgViewer(const std::string& filename);
		void GetFrames();
	private:
		ImgViewer::VecFrameType frames;
		cv::VideoCapture m_capture;	
		//cv::Mat m_frame;
		//cv::Mat frames[200];
	};

}
