#include "faceAnonymizer.h"
#include <iostream>

void CBlurShield::drawShield( cv::Mat& image, const cv::Rect& area,  const float scaleFactor/*=1.5*/  )
{
	//cv::rectangle(image, area, cv::Scalar(255,0,0), 3);
	cv::Size zoomScale(area.width*scaleFactor-area.width, area.height*scaleFactor-area.height);
	cv::Point2i tl=cv::Point(zoomScale.width/2, zoomScale.height/2);
	cv::Rect subImRect=area+zoomScale-tl;

	if (subImRect.x<0) subImRect.x=0;
	if (subImRect.y<0) subImRect.y=0;
	if (subImRect.x>image.cols) subImRect.x=image.cols-1;
	if (subImRect.y>image.rows) subImRect.y=image.rows-1;

	cv::Mat maskImage=image(subImRect);


	cv::blur(maskImage, maskImage, cv::Size(14,14));
}



void CFaceDetector::run()
{
	int count=0;
	int found=0;
	while(1)
	{
		count++;
		//critical section (CS) used for creating image because of possibility of try to store new image in _m_image by other thread by method storeNewImage.
		//I didn't use one long CS because of comparatively long time of detectMultiScale working.
		bool isGot;
		bool isStor;		
		cv::Mat image;
		std::vector<cv::Rect> areas;
		//CS:
		m_mutex.lock();
		image=m_image;
		areas=m_detectedArea;
		isGot = m_got;
		isStor = m_stored;
		m_mutex.unlock();
		//eof CS

		//cv::Mat imageToDraw=image;
		
		std::vector <cv::Rect> objects;
		int neightbor = 12;

		if (isStor == true & isGot == false)
		{
			if (areas.size()>0)
			{
				for (int i=0; i<areas.size(); i++)
				{
					std::vector<cv::Rect> localObjects;
					const int scaleFactor=5;
					cv::Size zoomScale(areas[i].width*scaleFactor-areas[i].width, areas[i].height*scaleFactor-areas[i].height);
					cv::Point2i tl=cv::Point(zoomScale.width/2, zoomScale.height/2);
					cv::Rect subImRect=areas[i]+zoomScale-tl;
					//cv::rectangle(imageToDraw, subImRect, cv::Scalar(255,255,0), 3);

					//std::cout<<subImRect.x<<"," <<subImRect.y<<";\n";
					if (subImRect.x<0) subImRect.x=0;
					if (subImRect.y<0) subImRect.y=0;
					if (subImRect.br().x>image.cols) subImRect.width=image.cols-1-subImRect.x;
					if (subImRect.br().y>image.rows) subImRect.height=image.rows-1-subImRect.y;

					cv::Mat areaImage=image(subImRect);
					if (areaImage.cols>10)
						m_clasFrontal.detectMultiScale(areaImage, localObjects,1.2,neightbor);  //face detection

					if (localObjects.size()==0)
					{
						m_clasFrontal.detectMultiScale(image,objects,1.2,neightbor);
					}
					
					for (int j=0; j<localObjects.size(); j++)
						localObjects[j]=localObjects[j]+(subImRect.tl());

					objects.assign(localObjects.begin(), localObjects.end());

				}
			}
			else
			{
				if (image.cols>10)
					m_clasFrontal.detectMultiScale(image, objects,1.2,neightbor);  //face detection
			}
		}
			

		if (objects.size()>0)
		{
			//detected faces stored in m_detectedArea in critical section due to possibility of read this variable by other thread.
			//CS
			m_mutex.lock();
			m_detectedArea=objects;
			m_got = true;
			m_stored = false;
			m_mutex.unlock();
			//eof CS
		}
		//std::cout<<"R";
		//cv::waitKey(40);
	}

}


void CFaceDetector::storeNewImage( const cv::Mat& image )
{

	bool canStor = true;

	while (canStor)
	{
		m_mutex.lock();
		canStor = m_stored;
		if (!canStor)
		{
			m_image=cv::Mat(image);
		}
		m_mutex.unlock();

		cv::waitKey(10);
	}

	m_mutex.lock();
	m_stored = true;
	m_mutex.unlock();

	////critical section used because of possibility of m_image retrieving in method run.
	//m_count++;
	////CS
	//m_mutex.lock();
	//m_image=cv::Mat(image);
	//
	////for debug:
	///*std::stringstream s;
	//s<<"F:\\dev\\k.wereszczynski\\cvl\\tmp\\c"<<m_count<<".jpg";
	//cv::imwrite(s.str(), m_image);*/
	////end for debug
	//m_mutex.unlock();
	////std::cout<<"N";
	////cv::waitKey(20);
	////eof CS
}

std::vector<cv::Rect> CFaceDetector::getArea() //not const because of mutex usage.
{
	//CS used because of possibility storing detecting faces in method run().

	bool canGot = false;
	std::vector<cv::Rect> rv;
	

	while(!canGot)
	{
		m_mutex.lock();
		canGot = m_got;
		
		if (canGot)
		{
			rv = m_detectedArea;
		}
		m_mutex.unlock();

		cv::waitKey(10);

	}

	m_mutex.lock();
	m_got = false;
	m_mutex.unlock();


	return rv;
}

CFaceAnonymizer::CFaceAnonymizer( const std::string& videoName, const std::string& frontalClassifier, const std::string& profileClassifier, IFaceShield* shield/*=NULL*/ ): m_capture(videoName),
	m_created(false)
{
	m_detector= new CFaceDetector(frontalClassifier, profileClassifier);

	if (shield==NULL)
	{
		m_maskShield=new CBlurShield();
		m_created=true;  //information about place of shield creation. If shield is inside this object created it should be also deleted, if it's created outside it should be deleted outside this object!
	}

	m_detector->start();
}

CFaceAnonymizer::~CFaceAnonymizer()
{
	delete m_detector;
	if (m_created)	//if shield was created in constructor we have to clean it up, else we wouldn't dare to do it. If user created object he should also clean it up.
		delete m_maskShield;
}

CFaceAnonymizer::CFaceAnonymizer( const CFaceAnonymizer& cpy )
{
	//Co zrobic z shield w obiekcie lhs? Czy m_created powinno byæ kopiowane, czy te¿ ustawiane zawsze na "false"? 
	//Nad tym trzeba chwilkê pomyœleæ, ¿eby nie narobiæ ba³aganu z przydzia³em pamiêci.
}

CFaceAnonymizer& CFaceAnonymizer::operator=( const CFaceAnonymizer& rhs )
{
	//Co zrobic z shield w obiekcie lhs? Czy m_created powinno byæ kopiowane, czy te¿ ustawiane zawsze na "false"? 
	//Nad tym trzeba chwilkê pomyœleæ, ¿eby nie narobiæ ba³aganu z przydzia³em pamiêci.
	return CFaceAnonymizer();
}

bool CFaceAnonymizer::load( const std::string& videoName )
{
	m_capture.open(videoName);
	return true;
}

bool CFaceAnonymizer::isOpended() const
{
	return m_capture.isOpened();
}

cv::Mat CFaceAnonymizer::getAnonymizedFrame()
{
	cv::Mat frame;
	if (m_capture.read(frame))
	{
		m_detector->storeNewImage(frame);
		std::vector<cv::Rect> areaVec=m_detector->getArea();
		for (int i=0; i<areaVec.size(); i++)
			m_maskShield->drawShield(frame, areaVec[i], 1.5);
		//std::cout<<"F";
		//cv::waitKey(20);
		//cv::rectangle(frame, areaVec[i], cv::Scalar(255,0,0), 3);
	}
	return frame;
}


//////////////////

			//const int wh=150, ww=150, sx=25, sy=25; //wh window height, ww window width, sxshift x, sy shift y;
			//cv::Rect startWindow(0,0,ww,wh);
			//if (image.cols>10)
			//{
			//	for (int x=0; x<image.cols-ww; x+=sx)
			//		for (int y=0; y<image.rows-wh; y+=sy)
			//		{
			//			std::vector<cv::Rect> localObjects;
			//			cv::Mat areaImage=image(cv::Rect(x, y, ww, wh));
			//			//cv::resize(areaImage, areaImage, cv::Size(450, 450));
			//			

			//			m_clasFrontal.detectMultiScale(areaImage, objects);
			//			if (localObjects.size()>0)
			//			{
			//				x+=ww;
			//				y+=wh;
			//			}

			//			for (int j=0; j<localObjects.size(); j++)
			//			{
			//				cv::rectangle(areaImage, localObjects[j], cv::Scalar(255,0,0), 3);
			//				localObjects[j]=localObjects[j]+cv::Point(x,y);
			//			}
			//			//for debug:
			//			/*std::stringstream s;
			//			s<<"F:\\dev\\k.wereszczynski\\cvl\\tmp\\c"<<count<<"x"<<x<<"y"<<y<<".jpg";
			//			cv::imwrite(s.str(), areaImage);*/
			//			//end for debug

			//			objects.assign(localObjects.begin(), localObjects.end());
			//		}
			//	

			//}