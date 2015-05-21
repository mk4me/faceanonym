#ifndef AnonimizedSequence_h__
#define AnonimizedSequence_h__
#include "faceAnonymizer.h"
#include "VideoSequence.h"
#include "faceAnonymizer.h"
#include "VideoSequence.h"
#include <iostream>
#include <sstream>
#include <string>
#include <list>

namespace Anonimizator{
	typedef std::vector<cv::Rect> ROIContainerType;

	template <class VideoSequence, class ROIContainerType>
	void operator<<(VideoSequence& lhs, const ROIContainerType& rhs);

	template <class VideoSequence, class Anonimizer>
	class AnonimizedSequence:public std::vector<ROIContainerType> {
	public:
		AnonimizedSequence(const std::string& fileName, Anonimizer* anonimizer):m_sequence(fileName),
			m_anonimizer(anonimizer),
			m_filePath(fileName)
		{
				m_sequence.read();
		};

		void operator() ();

		typename VideoSequence::VSFrameType getAnonimizedFrame(int i);
		int size();
	private:

		cv::Vec2f meanVector(const std::list<cv::Vec2f>& flowQueue);

		VideoSequence m_sequence;
		Anonimizer* m_anonimizer;
		std::string m_filePath;
	};

	template <class VideoSequence, class Anonimizer>
	cv::Vec2f Anonimizator::AnonimizedSequence<VideoSequence, Anonimizer>::meanVector(const std::list<cv::Vec2f>& flowQueue)
	{
		float x=0, y=0;
		for (auto it=flowQueue.begin(); it!=flowQueue.end(); ++it){
			x+=(*it)[0];
			y+=(*it)[1];
		}

		return cv::Vec2f(x/flowQueue.size(), y/flowQueue.size());
	}

	template <class VideoSequence, class Anonimizer>
	int Anonimizator::AnonimizedSequence<VideoSequence, Anonimizer>::size()
	{
		return m_sequence.size();
	}

	template <class VideoSequence, class Anonimizer>
	typename VideoSequence::VSFrameType Anonimizator::AnonimizedSequence<VideoSequence, Anonimizer>::getAnonimizedFrame(int i)
	{
		VideoSequence::VSFrameType frame(m_sequence[i]);
		for (auto it=(*this)[i].begin(); it!=(*this)[i].end(); ++it){
			//cv::rectangle(frame, cv::Rect(it->x, it->y, it->width,it->height), cv::Scalar(255,0,0), 2);
			cv::Mat roi(frame, cv::Rect(it->x, it->y, it->width,it->height));
			cv::blur(roi, roi, cv::Size(25, 25));
		}
		return frame;
	}

	template <class VideoSequence, class Anonimizer>
	void Anonimizator::AnonimizedSequence<VideoSequence, Anonimizer>::operator()()
	{
		m_anonimizer->start();
		int i=0;
		std::map<int, ROIContainerType> newRectangles;
		//cv::Mat prevFlow;
		bool start=true;
		std::list<cv::Vec2f> flowQueueV1;
		std::list<cv::Vec2f> flowQueueV2;
		int queueThreshold=5;
		float flowThreshold=1.0f;
		cv::Mat firstFrame=(*m_sequence.begin());
		cv::VideoWriter vwriter(m_filePath.substr(0, m_filePath.length()-4)+"_anm.avi", -1/*CV_FOURCC('2','V','U','Y')*/, 25, cv::Size(firstFrame.cols, firstFrame.rows) );
		for (auto it=m_sequence.begin(); it!=m_sequence.end(); ++it, ++i){
			
			(*m_anonimizer)(*it);
			cv::waitKey(40);
			this->push_back((*m_anonimizer)());
			
			cv::Mat frame=getAnonimizedFrame(i);
			if (i>0){
				if (i==114){
					int dummy=900;
					int dummy1=900;
					int dummy2=900;
					int dummy3=900;
					int dummy4=900;
					int dummy5=900;
					int dummy6=900;
					int dummy7=900;
					int dummy8=900;
					int dummy9=900;
				}

				cv::Mat framePrev, frameGray, flowMat;
				cv::cvtColor(m_sequence[i], frameGray, CV_RGB2GRAY);
				cv::cvtColor(m_sequence[i-1], framePrev, CV_RGB2GRAY);
				/*cv::equalizeHist(frameGray, frameGray);
				cv::equalizeHist(framePrev, framePrev);*/
				
				cv::calcOpticalFlowFarneback(framePrev, frameGray, flowMat, 0.5, 1, 25, 20, 1, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN);
				
				
				for (auto it=(*this)[i-1].begin(); it!=(*this)[i-1].end(); ++it){
					//cv::rectangle(frame, cv::Rect(it->x, it->y, it->width,it->height), cv::Scalar(0,255,0), 1);
					cv::Vec2f v1(0,0);//=flowMat.at<cv::Vec2f>(it->y, it->x);
					cv::Vec2f v2(0,0);//=flowMat.at<cv::Vec2f>(it->y+static_cast<double>(it->height)/2, it->x+static_cast<double>(it->width)/2);
					int count=0;
					for (int x=it->width/4; x<it->width/2; ++x)
						for (int y=it->height/4; y<it->height/2; ++y){
							double norm=cv::norm(cv::Point(it->width/2-x, it->height/2-y));
							double thresholdF=(static_cast<double>(it->height)/4+static_cast<double>(it->width)/4)/2;
							//cout<<"norm="<<norm<<", threshold="<<thresholdF<<";\n";
							if (norm< thresholdF){

								v1+=flowMat.at<cv::Vec2f>(it->y+y, it->x+x);
								v2+=flowMat.at<cv::Vec2f>(it->y+it->height-y, it->x+it->width-x);
								count+=1;
							}
						}
					std::cout<<"count="<<count<<std::endl;
					v1=cv::Vec2f(v1[0]/count, v1[1]/count);
					v2=cv::Vec2f(v2[0]/count, v2[1]/count);

					std::cout<<"value=("<<v1[0]<<", "<<v1[1]<<"); ("<<v2[0]<<", "<<v2[1]<<")"<<std::endl;
					float flowNorm=cv::norm(v1);
					std::cout<<"flowNorm1="<<flowNorm;
					if (flowNorm<flowThreshold){
						v1=cv::Vec2f(0.0f, 0.0f);
					}
					flowNorm=cv::norm(v2);
					std::cout<<" flowNorm2="<<flowNorm<<std::endl;
					if (flowNorm<flowThreshold){
						v2=cv::Vec2f(0.0f, 0.0f);
					}
					std::cout<<"valnr=("<<v1[0]<<", "<<v1[1]<<"); ("<<v2[0]<<", "<<v2[1]<<")"<<std::endl;
					//std::cout<<"mV0.1 ";
					flowQueueV1.push_back(v1);
					if (flowQueueV1.size()>20)
						flowQueueV1.erase(flowQueueV1.begin(), flowQueueV1.begin());
					//std::cout<<"mV0.2 ";
					flowQueueV2.push_back(v2);
					if (flowQueueV2.size()>20)
						flowQueueV2.erase(flowQueueV2.begin(), flowQueueV2.begin());
					//std::cout<<"mV1 ";
					v1=meanVector(flowQueueV1);
					//std::cout<<"mV2 ";
					v2=meanVector(flowQueueV2);
					//std::cout<<"mV3 ";
					std::cout<<"mean= ("<<v1[0]<<", "<<v1[1]<<"); ("<<v2[0]<<", "<<v2[1]<<")"<<std::endl;
					//cv::line(frame, cv::Point(it->x, it->y), cv::Point(it->x, it->y)+cv::Point(v[0], v[1]), cv::Scalar(0,255,255), 2);
					std::cout<<"("<<it->x<<", "<<it->y<<"), ("<<it->x+it->width<<", "<<it->y+it->height<<")"<<std::endl;
					cv::Point p1(static_cast<int>(static_cast<float>(it->x)+v1[0]), static_cast<int>(static_cast<float>(it->y)+v1[1]));
					cv::Point p2(static_cast<int>(static_cast<float>(it->x)+static_cast<float>(it->width)+v2[0]), static_cast<int>(static_cast<float>(it->y)+static_cast<float>(it->height)+v2[1]));
					std::cout<<"("<<p1.x<<", "<<p1.y<<"), ";
					std::cout<<"("<<p2.x<<", "<<p2.y<<") "<<std::endl;
					//cv::rectangle(frame,p1, p2, cv::Scalar(0,0,255), 1);
					if (p1.x>=0 && p1.y>=0 && p2.x>=0 && p2.y>=0){
						cv::Mat roi(frame, cv::Rect(p1, p2));
						cv::blur(roi, roi, cv::Size(25,25));
					}

					//std::cout<<"mV4 ";
					bool found=false;
					
					for (auto itC=(*this)[i].begin(); itC!=(*this)[i].end(); ++itC){
						cv::Point r1(itC->x, itC->y), r2(itC->x+itC->width, itC->y+itC->height+v2[1]);
						if (cv::norm(r1-p1)<30 && cv::norm(r2-p2)<30){
							found=true;
							break;
						}
					}
					//std::cout<<"mV5 ";
					if (!found)
						(*this)[i].push_back(cv::Rect(p1, p2));


				}
			}

			vwriter<<frame;
			std::cout<<"\nFrame "<< i<<" of "<<m_sequence.size()<<" anonimized\n";
			cv::imshow("test", frame);
			/*std::string s=std::to_string(static_cast<long long>(i));
			s="E:\\res\\Anionimizacja\\data\\testLabO7\\"+s+".jpg";*/
			/*std::stringstream s;
			s<<m_filePath.substr(0, m_filePath.length()-4)<<"_"<<i<<".jpg";*/
			//cv::imwrite(s, frame);
			//cv::waitKey(40);
		}

		/*for (auto it=newRectangles.begin(); it!=newRectangles.end(); ++it){
			int frameId=it->first;
			for (auto itRec=it->second.begin(); itRec!=it->second.end(); ++itRec){
				(*this)[frameId].push_back(*itRec);
			}
		}*/
		std::cout<<" <LE> ";
	}

	typedef AnonimizedSequence<VideoSequenceTypeCV, CFaceDetector> AnonimizedSequenceDefaultType;
}




#endif // AnonimizedSequence_h__
