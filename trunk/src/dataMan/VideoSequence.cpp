#include "VideoSequence.h"

int Anonimizator::CVVideoReader::size()
{
	return static_cast<int> (this->get(CV_CAP_PROP_FRAME_COUNT));
}

Anonimizator::CVVideoReader::CVVideoReader(const std::string& fileName):cv::VideoCapture(fileName)
{

}


void Anonimizator::FrameType::rectangle(int x1, int y1, int w, int h)
{
	cv::rectangle(static_cast<cv::Mat>(*this), cv::Rect(x1, y1, w, h), cv::Scalar(255,0,0), 2);
}

Anonimizator::FrameType& Anonimizator::FrameType::clone()
{
	cv::Mat f=static_cast<cv::Mat>(*this);
	FrameType cloned=f.clone();
	return cloned;
}