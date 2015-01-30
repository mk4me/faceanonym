#include "imageViewer.h"


ImgViewer::CImgViewer::CImgViewer(const std::string& filename) : m_capture(filename)
{
	
}

void ImgViewer::CImgViewer::GetFrames()
{
	bool endVideo=false;
	int i=0;
	while(!endVideo){
		if (m_capture.grab()){ // zwraca false jesli nie z³apie klatki
			if(cv::waitKey(30)>= 0) break;
			if ((i % 60)==0 )
			{
				cv::Mat frame;
				m_capture.retrieve(frame);
//				cv::imshow("",frame);
				frames.push_back(frame.clone());
				frame.release();
			}
			
		} 
		else{
			endVideo=true;
		}
		i++;

	}
//	cv::Mat frame[20];
//	int j=0;
//	for (auto i=frames.begin();i<frames.end();i++)
//	{
		//cv::resize(*i,frame[j++],cv::Size(),0.3,0.3,cv::INTER_LINEAR);
//		cv::imshow("test",*i);
//		cv::waitKey(100);
//		frame[j++]=*i;
//	}
//	cv::Mat dst;
//	cv::hconcat(frame,frames.size(),dst);
//	cv::imshow("NoName",dst);
//	cv::waitKey(0);	
	auto it = frames.begin();
//	cv::Rect roi = cv::Rect(400, 400, 400, 400);
	cv::Mat outputFrames(cv::Size(1920,1200),CV_8UC3);
	cv::Mat roi = outputFrames(cv::Rect(5,0,500,500));
	it++->copyTo(roi);
	roi = outputFrames(cv::Rect(505,500,500,500));
	it++->copyTo(roi);
//	cv::imshow(" ",outputFrames);
	cv::imshow("test ",roi);
	cv::waitKey(100);
}


