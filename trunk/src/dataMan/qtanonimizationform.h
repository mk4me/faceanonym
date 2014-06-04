#ifndef QTANONIMIZATIONFORM_H
#define QTANONIMIZATIONFORM_H

#include <QTGUI/QWidget>
#include <QtCore/QModelIndex>
#include <QtCore/QObject>
#include "boost/filesystem.hpp"
#include "cv.h"
#include <sstream>
#include <fstream>
namespace fs=boost::filesystem;

class CSpecifiedFolder
{
public:
	CSpecifiedFolder()
	{};

	CSpecifiedFolder(std::string path, int status, cv::Rect refArea=cv::Rect(0,0,0,0), cv::Rect faceArea=cv::Rect(0,0,0,0), int top=0, int bottom=0):m_path(path),
		m_status(status),
		m_refArea(refArea),
		m_faceArea(faceArea),
		m_top(top),
		m_bottom(bottom)
	{};
	void saveToStream(std::stringstream& s);
	void loadFromString(std::string s);

	std::string Path() const { return m_path; }
	void Path(std::string val) { m_path = val; }
	int Status() const { return m_status; }
	void Status(int val) { m_status = val; }
	cv::Rect RefArea() const { return m_refArea; }
	void RefArea(cv::Rect val) { m_refArea = val; }
	cv::Rect FaceArea() const { return m_faceArea; }
	void FaceArea(cv::Rect val) { m_faceArea = val; }
	int Top() const { return m_top; }
	void Top(int val) { m_top = val; }
	int Bottom() const { return m_bottom; }
	void Bottom(int val) { m_bottom = val; }
private:
	std::string m_path;
	
	int m_status;  //0-clear, 1-marked, 2-done, 3-approved; 
	
	cv::Rect m_refArea;
	
	cv::Rect m_faceArea;
	
	int m_top;
	
	int m_bottom;

	int getNextInt(std::string& s);
	
};

namespace Ui {
class QtAnonimizationForm;
}

class QtAnonimizationForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit QtAnonimizationForm(QWidget *parent = 0);
    ~QtAnonimizationForm();
    
	void saveToFile(const std::string& path);

	void loadFromFile(const std::string& path);

public slots:
    void readFolder();

    void runProcess();

	void faceMarking();

	void setFilter();

	void clearFilter();

	void showMovie(QModelIndex mi);

private:
	std::vector<CSpecifiedFolder> m_folderForMarking;
    Ui::QtAnonimizationForm *ui;

	void markFace(CSpecifiedFolder* sf);

	double distance(const cv::Point& p1, const cv::Point& p2);

	void compactBoundings(std::vector<cv::Rect>& iv);

	void blurMovie(CSpecifiedFolder* sf);

	void writeToTable(bool save=true);

	CSpecifiedFolder* findFolder(const std::string& path);

	int m_filter;
};

#endif // QTANONIMIZATIONFORM_H
