#ifndef VideoSequence_h__
#define VideoSequence_h__

#include <string>
#include <vector>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

namespace Anonimizator{

	class CVVideoReader:public cv::VideoCapture {
	public:
		CVVideoReader(const std::string& fileName);

		int size();
	};

	class FrameType:public cv::Mat {
	public:
		FrameType():cv::Mat(){};
		FrameType(const FrameType& frame):cv::Mat(static_cast<cv::Mat>(*this)){}
		FrameType(const cv::Mat& mat):cv::Mat(mat){};
		void rectangle(int x1, int y1, int w, int h);
		FrameType& clone();
	};

	/*void operator >> (const CVVideoReader& lhs, const FrameType& rhs){
		lhs>>static_cast<cv::Mat>(rhs);
	}*/

	template <class FrameType, class VideoReader>
	class VideoSequence
		: public std::vector<FrameType>{
	public:
		typedef FrameType VSFrameType;
		VideoSequence(const std::string& filePath);
		void read();
	private:
		VideoSequence(){};
		std::shared_ptr<VideoReader> m_videoReader;
		
	};

	template <class FrameType, class VideoReader>
	void Anonimizator::VideoSequence<FrameType, VideoReader>::read()
	{
		for (int i=0; i<m_videoReader->size(); ++i){
			FrameType frame;
			*m_videoReader>>frame;
			push_back(frame.clone());
		}
	}

	template <class FrameType, class VideoReader>
	Anonimizator::VideoSequence<FrameType, VideoReader>::VideoSequence(const std::string& filePath)
	{
		m_videoReader=std::shared_ptr<VideoReader>(new VideoReader(filePath));
	}

	typedef Anonimizator::VideoSequence<cv::Mat, CVVideoReader> VideoSequenceTypeCV;
}


#endif // VideoSequence_h__
