#include "qtanonimizationform.h"
#include "ui_qtanonimizationform.h"
#include <QTGUI/QFileDialog>
#include <QTGUI/QStandardItemModel>
#include <QTGUI/QStandardItem>
#include <QTGUI/QMessageBox>
#include <QtCore/QString>


#include "highgui.h"
#include "cxcore.h"


QtAnonimizationForm::QtAnonimizationForm(QWidget *parent) :
    QWidget(0,Qt::Window),
    ui(new Ui::QtAnonimizationForm),
	m_filter(-1)
{
    ui->setupUi(this);
	loadFromFile("anonimization_data.csv");
	writeToTable(false);
}

QtAnonimizationForm::~QtAnonimizationForm()
{
    delete ui;
}

void QtAnonimizationForm::readFolder()
{
	std::string fp=QFileDialog::getExistingDirectory(0, "Pick folder name").toStdString();
	if (fp!="")
	{
		fs::recursive_directory_iterator itrEnd;
		for (fs::recursive_directory_iterator fit(fp);
			fit != itrEnd;
			++fit)
		{
			if (fit->path().extension().generic_string<std::string>().find("avi")!=std::string::npos)
			{
				CSpecifiedFolder newFolder(fit->path().generic_string<std::string>(), 0);
				m_folderForMarking.push_back(newFolder);
			}
		}

		writeToTable();
	}
	
}

void QtAnonimizationForm::runProcess()
{
	for (int i=0; i<m_folderForMarking.size(); i++)
	{
		if (m_folderForMarking[i].Status()==1)
			blurMovie(&m_folderForMarking[i]);
	}
}

void QtAnonimizationForm::faceMarking()
{
	for (int i=0; i<m_folderForMarking.size(); i++)
	{
		if ((m_folderForMarking[i].Status()==0) || (m_folderForMarking[i].Status()==5))
			markFace(&m_folderForMarking[i]);
	}
}

int qtMouseX;
int qtMouseY;
bool qtPressed;

void qtMouseHandler(int event, int x, int y, int flags, void* param)
{
	qtMouseX=x;
	qtMouseY=y;
	switch(event){
	case CV_EVENT_LBUTTONDOWN:

		qtPressed=true;
		break;
	}
}

void QtAnonimizationForm::markFace( CSpecifiedFolder* sf )
{
	size_t found=0;
	std::string fp=sf->Path();
	while (found!=std::string::npos)
	{
		found=fp.find("/");
		if (found!=std::string::npos)
		{
			fp.erase(fp.begin()+found);
			//cout<<fp<<"\n";
			fp.insert(found, "\\");
			//cout<<fp<<"\n";
		}
	}
	

	/*getline(cin, folderPath);
	std::cout<<"Enter file name. \n>";*/
	found=fp.find_last_of("\\");
	std::string movieName;
	if (found!=std::string::npos)
		movieName=fp.substr(found+1, fp.length());

	fp=fp.substr(0, found);

	


	//cout<<fp<<"::"<<movieName<<"\n";

	std::string movieFolder=fp+"\\"+movieName;
	int dotPos=movieName.find_last_of(".");
	std::string movieShort=movieName.substr(0, dotPos);
	std::string outputName=fp+"\\"+movieShort+".an.avi";

	cv::VideoCapture capture(movieFolder);
	int codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
	int fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));
	cv::Mat image;
	capture>>image;
	int skipX=60, skipY=20;
	//writer.open(outputName, codec, fps,cv::Size(image.cols, image.rows) );
	cv::rectangle(image, cv::Rect(0,0,skipX,skipY), cv::Scalar(255,255,255), CV_FILLED);
	cv::putText(image, "Pomin", cv::Point(3,18), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(0,0,0));
	cv::Mat mh (image.rows, image.cols, CV_32FC1);
	
	cv::Mat bcg;

	int step=1;
	cv::Rect refArea;
	cv::Point tl;
	cv::Point middle;
	cv::imshow("Image", image);
	cv::waitKey(40);
	int mouseParam=5;
	cvSetMouseCallback("Image",qtMouseHandler,&mouseParam);
	cv::Rect currArea;
	cv::Point top, bottom;

	int faceWidth, faceHeight;
	while (step<7)
	{

		if (qtPressed)
		{
			if ((qtMouseX<skipX)&&(qtMouseY<skipY))
			{
				step=100;
				sf->Status(4);
			}

			if (step==1)
			{
				tl=cv::Point(qtMouseX, qtMouseY);
				cv::circle(image, cv::Point(qtMouseX, qtMouseY), 1, cv::Scalar(255,0,0), 1);
			}

			if (step==2)
			{
				refArea=cv::Rect(tl, cv::Point(qtMouseX, qtMouseY));
				sf->RefArea(refArea);
				cv::rectangle(image, refArea, cv::Scalar(255,0,0), 1);
			}

			if (step==3)
			{
				tl=cv::Point(qtMouseX, qtMouseY);
				cv::circle(image, cv::Point(qtMouseX, qtMouseY), 1, cv::Scalar(255,0,0), 1);
			}

			if (step==4)
			{
				currArea=cv::Rect(tl, cv::Point(qtMouseX, qtMouseY));
				cv::rectangle(image, currArea, cv::Scalar(255,0,0), 1);
				faceWidth=currArea.width;
				faceHeight=currArea.height;
				sf->FaceArea(currArea);
			}

			if (step==5)
			{
				top=cv::Point(qtMouseX, qtMouseY);
				cv::line(image, cv::Point(0, qtMouseY), cv::Point(image.cols, qtMouseY), cv::Scalar(255,0,0));
				sf->Top(qtMouseY);
			}

			if (step==6)
			{
				bottom=cv::Point(qtMouseX, qtMouseY);
				cv::line(image, cv::Point(0, qtMouseY), cv::Point(image.cols, qtMouseY), cv::Scalar(255,0,0));
				sf->Bottom(qtMouseY);
			}

			qtPressed=false;
			step++;
		}

		cv::imshow("Image", image);
		cv::waitKey(40);
	}
	if (step<100)
		sf->Status(1);
	writeToTable();
	cv::destroyAllWindows();
}

void QtAnonimizationForm::writeToTable(bool save/*=true*/)
{
	QStandardItemModel *model=new QStandardItemModel(m_folderForMarking.size(), 2);
	int k=0;
	for (int i=0; i<m_folderForMarking.size(); i++)
	{
		if ((m_filter==-1) || (m_filter==m_folderForMarking[i].Status()) )
		{
			k++;
			QStandardItem *itemPath = new QStandardItem(QString(m_folderForMarking[i].Path().c_str()));
			model->setItem(k-1, 0, itemPath);
			QStandardItem *itemStatus;
			if (m_folderForMarking[i].Status()==0)
				itemStatus= new QStandardItem(QString("Wczytany"));
			if (m_folderForMarking[i].Status()==1)
				itemStatus= new QStandardItem(QString("Sparametryzowany"));
			if (m_folderForMarking[i].Status()==2)
				itemStatus= new QStandardItem(QString("Przetworzony"));
			if (m_folderForMarking[i].Status()==3)
				itemStatus= new QStandardItem(QString("Zatwierdzony"));
			if (m_folderForMarking[i].Status()==4)
				itemStatus= new QStandardItem(QString("Pominiety"));
			if (m_folderForMarking[i].Status()==5)
				itemStatus= new QStandardItem(QString("Do poprawy"));
			model->setItem(k-1, 1, itemStatus);
		}
	}
	if (save)
		saveToFile("anonimization_data.csv");
	model->setRowCount(k);
	ui->fileTable->setModel(model);
}

void QtAnonimizationForm::blurMovie( CSpecifiedFolder* sf )
{
	int blurMask=25;

	
	std::string fp=sf->Path();
	blurMask=25;
	//std::string folderPath;

	size_t found=0;

	while (found!=std::string::npos)
	{
		found=fp.find("/");
		if (found!=std::string::npos)
		{
			fp.erase(fp.begin()+found);
			//cout<<fp<<"\n";
			fp.insert(found, "\\");
			//cout<<fp<<"\n";
		}
	}
	

	/*getline(cin, folderPath);
	std::cout<<"Enter file name. \n>";*/
	found=fp.find_last_of("\\");
	std::string movieName;
	if (found!=std::string::npos)
		movieName=fp.substr(found+1, fp.length());

	fp=fp.substr(0, found);

	


	//cout<<fp<<"::"<<movieName<<"\n";
	
	//getline(cin, movieName);

	std::string movieFolder=fp+"\\"+movieName;
	int dotPos=movieName.find_last_of(".");
	std::string movieShort=movieName.substr(0, dotPos);
	std::string outputName=fp+"\\"+movieShort+".an.avi";
	//cout<<movieFolder<<";\n"<<outputName<<";\n";
	
	cv::VideoCapture capture(movieFolder);
	int codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
	int fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));
	cv::Mat image;
	capture>>image;
	cv::VideoWriter writer;
	writer.open(outputName, codec, fps,cv::Size(image.cols, image.rows) );

	cv::Mat mh (image.rows, image.cols, CV_32FC1);

	cv::Mat bcg;
	
	int step=1;
	cv::Rect refArea=sf->RefArea();
	
	cv::Point tl;
	tl=refArea.tl();
	cv::Point middle;
	cv::imshow("Image", image);
	
	int mouseParam=5;
	//cvSetMouseCallback("Image",mouseHandler,&mouseParam);
	cv::Rect currArea=sf->FaceArea();
	cv::Point top=cv::Point(0,sf->Top()), bottom=cv::Point(0,sf->Bottom());
	
	int faceWidth, faceHeight;
	faceWidth=currArea.width;
	faceHeight=currArea.height;

	middle=cv::Point(currArea.tl().x+(currArea.width/2), currArea.tl().y+(currArea.height/2) );
	
	cv::cvtColor(image, bcg, CV_RGB2GRAY);
	cv::Mat areaImage=image(refArea);
	//cv::Point mra=cv::Point(refArea.tl().x+(refArea.width/2), refArea.tl().y+(refArea.height/2) );
	cv::Scalar mean;
	cv::Scalar deviation;
	//=cv::mean(areaImage);
	cv::meanStdDev(image, mean, deviation);
	int threshold=mean[0];
	
	int i=0;
	int minArea=refArea.area();
	float constDist=currArea.width;
	if (currArea.height>constDist)
		constDist=currArea.height;

	//cout<<threshold<<", "<<minArea<<";\n";

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
			cv::Mat channels[3];
			cv::split(frame, channels);
			silh=gray;
			silh-=20;

			for (int c=1; c<silh.cols; ++c)
				for (int r=1; r<silh.rows; ++r){

					for (int i=0; i<3; ++i){
						if (channels[i].at<uchar>(r, c)>deviation[i]/3){
							channels[i].at<uchar>(r, c)=255;
							silh.at<uchar>(r, c)=255;
						}
					}
				}
			
			for (int c=1; c<silh.cols; ++c)
				for (int r=1; r<silh.rows; ++r){
					if (silh.at<uchar>(r,c)==0)
						silh.at<uchar>(r,c)=255;
					else
						silh.at<uchar>(r,c)=0;
				}

			
			cv::updateMotionHistory(silh, mh, i, 2);
			std::vector<cv::Rect> boundingRects;
			cv::Mat segMask;
			cv::segmentMotion(mh, segMask, boundingRects, i, 1.5);
			//compactBoundings(boundingRects);
			cv::Rect rectFound(cv::Point(0,2000), cv::Point(0,2000));
			
			float maxY=2000;
			int currDiff=2000000;
			for (int i=0; i<boundingRects.size(); i++)
			{
				
				cv::Rect r=boundingRects[i];
				cv::rectangle(image, r, cv::Scalar(0,255,0), 1);
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
				tlx=1;
			if (tly<0)
				tly=1;
			if (tlx+width>image.cols)
				width=image.cols-tlx;

			if (tly+height>image.rows)
				height=image.rows-tly;

			cv::Rect roi(tlx, tly, width, height);
			cv::Mat maskImage=image(roi);


			//cv::blur(maskImage, maskImage, cv::Size(blurMask, blurMask));

			cv::rectangle(image, roi, cv::Scalar(0,255,255), 3);
			
			cv::imshow("Image", image);
			writer.write(image);
			cv::waitKey(1);

		}
		else
			theEnd=true;

	}
	cv::destroyAllWindows();
	sf->Status(2);
	writeToTable();
}

void QtAnonimizationForm::showMovie(QModelIndex mi)
{
	std::string fp=mi.data().toString().toStdString();
	size_t found=0;
	while (found!=std::string::npos)
	{
		found=fp.find("/");
		if (found!=std::string::npos)
		{
			fp.erase(fp.begin()+found);
			//cout<<fp<<"\n";
			fp.insert(found, "\\");
			//cout<<fp<<"\n";
		}
	}

	found=fp.find_last_of("\\");
	std::string movieName;
	if (found!=std::string::npos)
		movieName=fp.substr(found+1, fp.length());

	fp=fp.substr(0, found);

	std::string movieFolder=fp+"\\"+movieName;
	int dotPos=movieName.find_last_of(".");
	std::string movieShort=movieName.substr(0, dotPos);
	std::string outputName=fp+"\\"+movieShort+".an.avi";
	//cout<<movieFolder<<";\n"<<outputName<<";\n";

	found=0;
	while (found!=std::string::npos)
	{
		found=movieFolder.find("\\");
		if (found!=std::string::npos)
		{
			movieFolder.erase(movieFolder.begin()+found);
			//cout<<fp<<"\n";
			movieFolder.insert(found, "/");
			//cout<<fp<<"\n";
		}
	}


	cv::VideoCapture capture(outputName);

	bool theEnd=false;

	if (capture.isOpened())
	{
		while(!theEnd)
		{
			cv::Mat image;
			capture >> image;
			if (image.cols<100)
				theEnd=true;
			else
			{
				cv::imshow("Movie", image);
				cv::waitKey(40);
			}
			
		}

		cv::waitKey(500);
		
		CSpecifiedFolder* sf=findFolder(movieFolder);
		if (sf!=NULL)
		{
			if (QMessageBox::question(this, "Weryfikacja anonimizacji", "Czy anonimizacja przebiegla poprawnie?", QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
				sf->Status(3);
			else
				sf->Status(5);
		}
		cv::destroyAllWindows();
	}

	writeToTable();
}

CSpecifiedFolder* QtAnonimizationForm::findFolder(const std::string& path )
{
	for (int i=0; i<m_folderForMarking.size(); i++)
	{
		const std::string s=m_folderForMarking[i].Path();
		if (s==path)
			return &m_folderForMarking[i];
	}
	return NULL;
}

void QtAnonimizationForm::setFilter()
{
	/*if (m_folderForMarking[i].Status()==1)
		itemStatus= new QStandardItem(QString("Sparametryzowany"));
	if (m_folderForMarking[i].Status()==2)
		itemStatus= new QStandardItem(QString("Przetworzony"));
	if (m_folderForMarking[i].Status()==3)
		itemStatus= new QStandardItem(QString("Zatwierdzony"));
	if (m_folderForMarking[i].Status()==4)
		itemStatus= new QStandardItem(QString("Pominiety"));
	if (m_folderForMarking[i].Status()==5)
		itemStatus= new QStandardItem(QString("Do poprawy"));*/

	m_filter=ui->FilterCBox->currentIndex()-1;
	writeToTable();
}

void QtAnonimizationForm::clearFilter()
{
	m_filter=-1;
	writeToTable();
}

void QtAnonimizationForm::saveToFile( const std::string& path )
{
	std::stringstream s;
	for (int i=0; i<m_folderForMarking.size(); i++)
		m_folderForMarking[i].saveToStream(s);

	std::ofstream of(path);
	of<<s.str();
	of.close();
}

void QtAnonimizationForm::loadFromFile( const std::string& path )
{
	std::ifstream inf(path);

	if (inf.is_open())
	{
		while (!inf.eof())
		{
			std::string s;
			std::getline(inf, s);
			if (s!="")
			{
				CSpecifiedFolder sf;
				sf.loadFromString(s);
				m_folderForMarking.push_back(sf);
			}
			
		}
	}
}

void QtAnonimizationForm::compactBoundings( std::vector<cv::Rect>& iv )
{
	for (int i=0; i<iv.size();++i){	
		if (iv[i].area()<100){
			iv.erase(iv.begin()+i);
		}
	}

	for (auto it=iv.begin(); it!=iv.end();++it){
		bool found=false;
		for( auto it1=it; !found  && it1!=iv.end();){
			if (it1==it)
				it1+=1;
			if (it1!=iv.end()){
				if ( (it->contains(it1->tl())) || 
						(it->contains(it1->br())) ||
						(distance(it->br(), it1->tl())<30)||
						(distance(it1->br(), it->tl())<30)
						/*(it->tl().x - it1->br().x <20) &&
						(it->tl().y - it1->br().y <20) ||
						(it1->tl().x - it->br().x <20) &&
						(it1->tl().y - it->br().y <20)*/
				){
					cv::Rect r(it->tl(), it1->br());
					found=true;
					iv.erase(it1);
					iv.erase(it);
					iv.push_back(r);
				}

				if (!found)
					++it1;
			}
		}
		if (found)
			it=iv.begin();

	}
}

double QtAnonimizationForm::distance( const cv::Point& p1, const cv::Point& p2 )
{
	return std::sqrt(static_cast<double>(p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

void CSpecifiedFolder::saveToStream( std::stringstream& s )
{
	s<<m_path<<";"<<m_status<<";"<<m_refArea.tl().x<<";"<<m_refArea.tl().y<<";"<<m_refArea.width<<";"<<m_refArea.height<<";";
	s<<m_faceArea.tl().x<<";"<<m_faceArea.tl().y<<";"<<m_faceArea.width<<";"<<m_faceArea.height<<";";
	s<<m_top<<";"<<m_bottom<<";\n";
}

void CSpecifiedFolder::loadFromString( std::string s )
{
	size_t pos;
	pos=s.find(";");
	m_path=s.substr(0, pos);
	s.erase(0, pos+1);

	m_status=getNextInt(s);

	m_refArea=cv::Rect(getNextInt(s), getNextInt(s), getNextInt(s), getNextInt(s));
	m_faceArea=cv::Rect(getNextInt(s), getNextInt(s), getNextInt(s), getNextInt(s));
	m_top=getNextInt(s);
	m_bottom=getNextInt(s);
}

int CSpecifiedFolder::getNextInt( std::string& s )
{
	int x=0;
	std::stringstream ss;
	int pos=s.find(";");
	ss<<s.substr(0, pos);
	ss>>x;
	s.erase(0, pos+1);
	return x;
}
