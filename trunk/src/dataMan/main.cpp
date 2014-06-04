#include "main.h"
#include <iostream>
#include "folderOrganize.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "boost/filesystem.hpp"
#include "faceAnonymizer.h"
#include <QTGUI/QApplication>
#include <QTGUI/QFileDialog>
#include <QtCore/QString>
#include "qtanonimizationform.h"
#include "windows.h"
//#include "boost/archive/binary_oarchive.hpp"
//#include "boost/archive/binary_iarchive.hpp"

namespace fs=boost::filesystem;
using namespace std;

void folderFixer()
{
	std::system("cls");
	std::cout<<"Welcome to measurement data folder FIXER\n===============================================\n";
	std::cout<<"Enter folder path:\n> ";
	std::string folderPath;
	getline(cin, folderPath);
	folderPath+="\\";
	std::cout<<"\nEnter info file name. If its the same as the last part of folder just press ENTER:\n>";
	/*char input[256];
	std::cin.getline(input, 256);*/
	std::string infoFileName;
	
	getline(cin, infoFileName);

	//CFolderFixer sessionInfo("f:\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\", "2010-12-15-P02-S01.Session 1.enf");
	//CFolderFixer sessionInfo("f:\cvl\data\user\dataMan\2010-12-15-P02-S01\", "2010-12-15-P02-S01.Session 1.enf");
	
	CBodyCollection::getInstance()->loadFromFile("fixer.vbd");

	CFolderFixer sessionInfo(folderPath, infoFileName);

	

	
	std::string choice;
	
	while (choice!="C")
	{
		sessionInfo.setBodyNameAndSession();
		sessionInfo.printSessInfo();

		cout<<"If this data are correct press C;\nIf Body name is wrong press B;\n>";
		getline (cin, choice);
		if (choice == "B")
		{
			cout<<"Write correct body name:\n>";
			std::string correctBodyName;
			getline(cin, correctBodyName);
			sessionInfo.BodyName(correctBodyName);
		}
	}
	//std::system("pause");
	sessionInfo.fixFolder();

	CBodyCollection::getInstance()->saveToFile("fixer.vbd");
	std::cout<<"done...";
	std::system("pause");
}

void changeRes()
{
	std::system("cls");
	std::cout<<"Welcome to movie resolution CHANGER.\n===============================================\n";
	std::cout<<"Enter folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	std::string folderPath;
	getline(cin, folderPath);
	int x=800;
	int y=600;
	fs::recursive_directory_iterator itrEnd;
	int i=0;
	std::string commandFull;
	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		if (fit->path().extension().generic_string<std::string>().find("avi")!=std::string::npos)
		{
			int codec;
			int fps;
			cv::VideoCapture capture(fit->path().string());
			codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
			fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));

			cv::VideoWriter writer;
			std::string outputFileName=folderPath+fit->path().stem().generic_string<std::string>()+"new"+".avi";
			writer.open(outputFileName, codec, fps,cv::Size(x, y) );
			std::cout<<"Changing resolution for file: "<<fit->path()<<" ...";
			bool end=false;
			while (!end)
			{
				
				cv::Mat frame;
				if (capture.read(frame))
				{
					cv::Mat newFrame(x, y, frame.type());
					cv::resize(frame, newFrame, cv::Size(x, y));
					
					cv::imshow("Src", frame);
					cv::imshow("Dst", newFrame);
					cv::waitKey(1);
					
					writer.write(newFrame);
				}
				else
					end=true;
			}
			
			capture.release();
			writer.~VideoWriter();
			fs::remove(fit->path());
			fs::rename(fs::path(outputFileName), fit->path());
			cout<<"done.\n\n";
		}

	}
}

void faceDetector()
{
	std::system("cls");
	std::cout<<"Welcome to AMC anonymizer. \n===============================================\n";
	std::cout<<"Enter path to movie:\n> ";
	std::string folderPath;
	getline(cin, folderPath);
	/*std::string cascadeFrontalPath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\cascade\\haarcascade_frontalface_alt.xml";
	std::string cascadeProfilePath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\cascade\\haarcascade_profileface.xml";
*/
	std::string cascadeFrontalPath="cascade_frontal.xml";
	std::string cascadeProfilePath="cascade_profilexml";

	std::string movieFrontalPath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.60981847.avi";
	std::string movieFrontalPath1="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T08.60981847.avi";
	std::string movieFrontalPath2="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2011-03-04-B0005-S02\\2011-03-04-B0005-S02-T07.60981847.avi";
	std::string movieProfilePath1="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.59461898.avi";
	std::string movieProfilePath2="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.53875336.avi";
	std::string movieBackPath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.56339527.avi";
	
	CFaceAnonymizer faceAnonymizer(folderPath, cascadeFrontalPath, cascadeProfilePath);
	//CFaceAnonymizer faceAnonymizer(movieFrontalPath1, cascadeFrontalPath, cascadeProfilePath);

	while (1)
	{
		if (faceAnonymizer.isOpended())
		{
			cv::Mat frame=faceAnonymizer.getAnonymizedFrame();
			if (frame.cols>10)
			{
				cv::imshow("Anonymizing", frame);
				cv::waitKey(40);
			}
		}
		
	}
	

	/*cv::CascadeClassifier classifier(cascadeFrontalPath);

	cv::VideoCapture capture(movieFrontalPath);

	while (1)
	{

	cv::Mat frame;
	if (capture.read(frame))
	{
	std::vector <cv::Rect> objects;
	classifier.detectMultiScale(frame, objects);

	for (int i=0; i<objects.size(); i++)
	cv::rectangle(frame, objects[i], cv::Scalar(255,0,0), 3);

	cv::imshow("Face detector", frame);
	cv::waitKey(40);
	}
	}*/
}

void asmAnonymize()
{
	std::system("cls");
	std::cout<<"Welcome to AMC anonymizer. \n===============================================\n";
	std::cout<<"Enter folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	std::string folderPath;
	getline(cin, folderPath);
	fs::recursive_directory_iterator itrEnd;
	int i=0;
	std::string commandFull;
	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		cout<< ".";
		if (fit->path().extension().generic_string<std::string>()==".amc")
		{
			cout<<"\n Anonymizing file ..."<< fit->path().string();
			std::ifstream iFile(fit->path().string());
			
		
			std::stringstream data;
			bool firstLine=true;
			while (!iFile.eof())
			{
				std::string line;
				std::getline(iFile, line);
				
				//std::cout << line<<"\n";
				if (!firstLine)
					data<<line+"\n";
				else
				{
					data<<"# AMC file generated using VICON NEXUS\n";
					firstLine=false;
				}
			}
			iFile.close();
			std::ofstream oFile(fit->path().string());
			oFile << data.rdbuf();
			/*data.seekp(0);
			while (!data.eofbit)
			{
				std::string line;
				std::getline(iFile, line);
				oFile<<line;
			}*/
			oFile.close();
			cout<<" done.\n";
		}

	}
}

void classifierTraining()
{

}

int mouseX;
int mouseY;
bool bPressed;

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	mouseX=x;
	mouseY=y;
	switch(event){
	case CV_EVENT_LBUTTONDOWN:

		bPressed=true;
		break;
	}
}

void markFace(cv::VideoWriter& writer)
{
	int blurMask=25;

	
	std::string fp=QFileDialog::getOpenFileName(0, "Pick folder name").toStdString();
	blurMask=25;
	//std::string folderPath;

	size_t found=0;

	while (found!=std::string::npos)
	{
		found=fp.find("/");
		if (found!=std::string::npos)
		{
			fp.erase(fp.begin()+found);
			cout<<fp<<"\n";
			fp.insert(found, "\\");
			cout<<fp<<"\n";
		}
	}
	

	/*getline(cin, folderPath);
	std::cout<<"Enter file name. \n>";*/
	found=fp.find_last_of("\\");
	std::string movieName;
	if (found!=std::string::npos)
		movieName=fp.substr(found+1, fp.length());

	fp=fp.substr(0, found);

	


	cout<<fp<<"::"<<movieName<<"\n";
	
	//getline(cin, movieName);

	std::string movieFolder=fp+"\\"+movieName;
	int dotPos=movieName.find_last_of(".");
	std::string movieShort=movieName.substr(0, dotPos);
	std::string outputName=fp+"\\"+movieShort+".an.avi";
	cout<<movieFolder<<";\n"<<outputName<<";\n";
	
	cv::VideoCapture capture(movieFolder);
	int codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
	int fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));
	cv::Mat image;
	capture>>image;

	writer.open(outputName, codec, fps,cv::Size(image.cols, image.rows) );

	cv::Mat mh (image.rows, image.cols, CV_32FC1);

	cv::Mat bcg;
	
	int step=1;
	cv::Rect refArea;
	cv::Point tl;
	cv::Point middle;
	cv::imshow("Image", image);
	cv::waitKey(40);
	int mouseParam=5;
	//cvSetMouseCallback("Image",mouseHandler,&mouseParam);
	cv::Rect currArea;
	cv::Point top, bottom;

	int faceWidth, faceHeight;
	while (step<7)
	{

		if (bPressed)
		{
			if (step==1)
			{
				tl=cv::Point(mouseX, mouseY);
				cv::circle(image, cv::Point(mouseX, mouseY), 1, cv::Scalar(255,0,0), 1);
			}

			if (step==2)
			{
				refArea=cv::Rect(tl, cv::Point(mouseX, mouseY));
				cv::rectangle(image, refArea, cv::Scalar(255,0,0), 1);
			}

			if (step==3)
			{
				tl=cv::Point(mouseX, mouseY);
				cv::circle(image, cv::Point(mouseX, mouseY), 1, cv::Scalar(255,0,0), 1);
			}

			if (step==4)
			{
				currArea=cv::Rect(tl, cv::Point(mouseX, mouseY));
				cv::rectangle(image, currArea, cv::Scalar(255,0,0), 1);
				faceWidth=currArea.width;
				faceHeight=currArea.height;
			}

			if (step==5)
			{
				top=cv::Point(mouseX, mouseY);
				cv::line(image, cv::Point(0, mouseY), cv::Point(image.cols, mouseY), cv::Scalar(255,0,0));
				
			}

			if (step==6)
			{
				bottom=cv::Point(mouseX, mouseY);
				cv::line(image, cv::Point(0, mouseY), cv::Point(image.cols, mouseY), cv::Scalar(255,0,0));
			}

			bPressed=false;
			step++;
		}

		cv::imshow("Image", image);
		cv::waitKey(40);
	}

	middle=cv::Point(currArea.tl().x+(currArea.width/2), currArea.tl().y+(currArea.height/2) );

	cv::cvtColor(image, bcg, CV_RGB2GRAY);
	cv::Mat areaImage=bcg(refArea);
	
	cv::Scalar mean=cv::mean(areaImage);
	int threshold=mean[0];
	cv::threshold(bcg, bcg, threshold, 255, cv::THRESH_BINARY);
	int i=0;
	int minArea=refArea.area();
	float constDist=currArea.width;
	if (currArea.height>constDist)
		constDist=currArea.height;

	cout<<threshold<<", "<<minArea<<";\n";

	cv::Rect lastFound=refArea;
	cv::Point constr=middle;
	bool theEnd=false;
	while (capture.isOpened() && !theEnd)
	{
		i++;
		cv::Mat frame;
		capture >> frame;
		if (frame.cols>500)
		{
			cv::Mat gray;
			cv::Mat silh;

			cv::cvtColor(frame, gray, CV_RGB2GRAY);
			silh=gray;
			cv::threshold(silh, silh, threshold, 255, cv::THRESH_BINARY);
			cv::absdiff(silh, bcg, silh);

			cv::updateMotionHistory(silh, mh, i, 2);
			std::vector<cv::Rect> boundingRects;
			cv::Mat segMask;
			cv::segmentMotion(mh, segMask, boundingRects, i, 1.5);
			cv::Rect rectFound(cv::Point(0,2000), cv::Point(0,2000));
			
			float maxY=2000;
			int currDiff=2000000;
			for (int i=0; i<boundingRects.size(); i++)
			{
				
				cv::Rect r=boundingRects[i];
				//cv::rectangle(image, r, cv::Scalar(0,255,0), 1);
				if ((r.area()>minArea) && (r.tl().y+(r.height/2)>top.y) && (r.tl().y+(r.height/2)<bottom.y))
				{
					double dist=((double)r.tl().y+(r.height/2)-(double)constr.y)*((double)r.tl().y+(r.height/2)-(double)constr.y);
					dist+=((double)r.tl().x+(r.width/2)-(double)constr.x)*((double)r.tl().x+(r.width/2)-(double)constr.x);
				
					int diff=r.area()-currArea.area();
					if (diff<0)
						diff*=-1;
					dist=sqrt(dist);
					if (dist<constDist)
					{
						if (diff<currDiff)
						{
							rectFound=r;
							currDiff=diff;
							maxY=0;
						}
					}

				}
			}
			if (maxY==2000)
			{
				rectFound=currArea;
			}
			else
			{
				constr=cv::Point(rectFound.tl().x+(rectFound.width/2), rectFound.tl().y+(rectFound.height/2) );
				currArea=rectFound;
				constDist=currArea.width;
				if (currArea.height>constDist)
					constDist=currArea.height;
			}

			int tlx=constr.x-(constDist), tly=constr.y-(constDist), width=constDist*2, height=constDist*2;

			if (tlx<0)
				tlx=0;
			if (tly<0)
				tly=0;
			if (tlx+width>image.cols)
				width=image.cols-tlx;

			if (tly+width>image.rows)
				width=image.rows-tly;

			cv::Rect roi(tlx, tly, width, height);
			cv::Mat maskImage=image(roi);


			cv::blur(maskImage, maskImage, cv::Size(blurMask, blurMask));

			
			cv::imshow("Image", image);
			writer.write(image);
			cv::waitKey(1);

		}
		else
			theEnd=true;

	}


}
class Anonimization
{
public:
	Anonimization()
	{
		QtAnonimizationForm* anmForm=new QtAnonimizationForm(0);
		anmForm->show();
	}
};
void runAnonimizerGUI()
{
	Anonimization anm;
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	int argc;
	char** argv;
	//QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	
	QtAnonimizationForm* anmForm=new QtAnonimizationForm(0);
	anmForm->show();
	app.exec();
	return 0;
}
	//QFileDialog::getOpenFileName(0, "Pick folder name");
	//anmForm->show();
	//while(1);

	//bool end=false;
	//while(!end)
	//{
	//	std::system("cls");
	//	std::cout<<"What do you want to do today? ;-)\nWell, maybe I'll ask what do you HAVE TO do today?\n\n\n";
	//	std::cout<<"1- fix folder\n2- change resolution\n3-face detection\n4-ACM anonymization\n0- exit\n> ";
	//	int i;
	//	cin>>i;
	//	std::cin.ignore();
	//	switch (i)
	//	{
	//	case 1: {
	//		folderFixer();
	//		break;
	//			}
	//	case 2: {
	//		changeRes();
	//		break;
	//			}
	//	case 3: {

	//		runAnonimizerGUI();
	//		//cv::VideoWriter writer;
	//		//markFace(writer);
	//		break;
	//			}
	//	case 4: {
	//		asmAnonymize();
	//		break;
	//			}
	//	case 5: {
	//		classifierTraining();
	//		break;
	//			}
	//	case 0: {
	//		end=true;
	//		break;
	//			}
	//	default: {
	//		std::cout<<"Option not recognized...\n";
	//		std::system("pause");
	//			 }

	//	}
	//}
