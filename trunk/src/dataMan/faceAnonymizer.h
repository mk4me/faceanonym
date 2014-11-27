/*	\brief		files contains classes responsible for HAAR classifier face detection.
				CFaceAninymizer is class which creates rectangular shield on face. Shield is implemented in IShield interface by which user can create its own shields. Class CFaceAnonymizer uses class
				derived	from IFaceShield
	\author		Kamil Wereszczynski
	\version	1.0
	\date		12.01.2011
*/
#ifndef faceAnonymizer_h__
#define faceAnonymizer_h__
#include "cv.h"
#include "cxcore.hpp"
#include "highgui.h"
#include <Thread>
#include <Mutex>
using namespace OpenThreads;

class IFaceShield
{
public:
	virtual void drawShield(cv::Mat& image, const cv::Rect& area, const float scaleFactor=1.5)=0;
	virtual ~IFaceShield(){};
};

class CBlurShield: public IFaceShield
{
public:
	virtual void drawShield( cv::Mat& image, const cv::Rect& area, const float scaleFactor=1.5 );
	virtual ~CBlurShield(){};
};

/*	\brief		Class responsible for detecting faces on given image.
				This detector uses HAAR cascade method for detecting faces. So you need to have two different .XML files containing classifiers for frontal and profile view of face separately.
	\attention	Do NOT derive this class - it has no virtual destructor.
	\author		Kamil Wereszczynski
	\version	1.0
	\date		13.01.2011
*/
class CFaceDetector: public Thread
{
public:
	/*	\brief	Creates new instance of CFaceDetector loading and creating frontal and profile cascade classifier
		\param	frontalClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for frontal face detection.
		\param	profileClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for profile face detection.
	*/
	CFaceDetector::CFaceDetector( const std::string& frontalClassifier, const std::string& profileClassifier ):m_clasFrontal(frontalClassifier),
		m_clasProfile(profileClassifier), 
		m_count(0),
		m_stored(false),
		m_got(false)
	{}

	/*	\brief Method derived from OpenThreads::Thread class executed in separate thread.
	*/
	virtual void run();

	/*	\brief		Method storing new image. Use it for adding new image to recognize face.
		\attention	Remember that not all images will be recognized. After ending the actual detect process the new one on last stored image is started. This method overrides existing image, 
		so all images stored during persisting process will be not recognized. 
	*/
	void storeNewImage(const cv::Mat& image);

	/*	\brief		Method retrieving counted rectangle hiding face
		\attention	Remember that not all images will be recognized. After ending the actual detect process the new one on last stored image is started. This method overrides existing image, 
		so all images stored during persisting process will be not recognized. 
		\returns	rectangles that hides faces on screen
	*/
	std::vector<cv::Rect> getArea();

private:

	CFaceDetector();						//!< object without classifiers should not be created!

	Mutex m_mutex;							//!< mutex for locking
	cv::Mat m_image;						//!< image where face is beeing detected
	cv::CascadeClassifier m_clasFrontal;	//!< classifier for frontal face detection
	cv::CascadeClassifier m_clasProfile;	//!< classifier for profile face detection
	std::vector<cv::Rect> m_detectedArea;	//!< last area (understood as set of rectangles)  where face was detected.
	int m_count; //debug
	bool m_stored;
	bool m_got;
};


/*	\brief		Class responsible for drawing anonymizing mask on given image.
				Theoretically for creating shield could be used Builder or AbstractFactory GoF design pattern, but it would be using a sledgehammer to crack a nut I think. So I made it in other way.
				If user make his own shield derived from IFaceShield he can get it over constructor by shield param, in opposite case default CBlurShield is created in this constructor. 
	\attention	Remember to clean up if you get your shield over this class. But if default blur shield is created its destructor is in destructor of this class called. 
	\attention	Do NOT derive this class - it has no virtual destructor.
	\author		Kamil Wereszczynski
	\version	1.0
	\date		12.01.2011
*/
class CFaceAnonymizer
{
public:
	CFaceAnonymizer(const CFaceAnonymizer& cpy);						//!< copy-constructor becouse we have dynamically allocated private field(s);

	/*	\brief		Constructor, that should be used in case of inclination using other shield then blur, and with specific videoName.
		\param		videoName path to a file containing video; if videoName="" no file will be loaded - in that case load method should be used.
		\param		frontalClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for frontal face detection.
		\param		profileClassifier is the path to a XML file containing HAAR (or LBP but it's worse than HAAR) cascade for profile face detection.
		\param		shield class derived from interface IShield responsible for drawing mask on face. If NULL default CBlurShield is created.
		\attention	File with frontal and profile classifiers is available on SVN in cascade folder.
	*/
	CFaceAnonymizer(const std::string& videoName, const std::string& frontalClassifier, 
			const std::string& profileClassifier, IFaceShield* shield=NULL);

	~CFaceAnonymizer();
	
	CFaceAnonymizer& operator= (const CFaceAnonymizer& rhs);	//!< assignement operator becouse we have dynamically allocated private field(s);
	
	/*	\brief		Loads a stream from file;
		\param		videoName path to a file containing video;
		\returns	true if video loaded, false in opposite case.
	*/
	bool load(const std::string& videoName);

	/*	\brief		retrieves and anonymizes next frame from video stream;
		Method		grabs frame from video capture and returns new image in cv::Mat format with shield drawn.
		\returns	new image with shield drawn on it.
	*/
	cv::Mat getAnonymizedFrame();

	/*	\brief		Informs about status of the stream
		\returns	true if video stream is ready to get frame, false in opp. case.
	*/
	bool isOpended() const;


private:

	CFaceAnonymizer(){};		//!< Face anonymizer runs CFaceDetector thread, so it have to create new instance of CFaceDetector. The default constructor wouldn't know how to create it without classifiers!

	IFaceShield* m_maskShield;	//!< shield that will be drawn on image;

	cv::VideoCapture m_capture;	//!< capture, where movie is stored.

	CFaceDetector* m_detector;	//!< its thread is started in constructor, afterwards it is used for retrieving face rectangle from detector.

	bool m_created;				//!< informs aboud shield creation inside this object (in case shield parameter in constuctor != NULL);
};



#endif // faceAnonymizer_h__
