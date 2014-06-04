#ifndef main_h__
#define main_h__
#include <iostream>
#include "cv.h"


class Resolution
{
public:
	Resolution(const std::string& line);
	std::string save();
	std::string CameraId() const { return m_cameraId; }
	void CameraId(std::string val) { m_cameraId = val; }
	std::string Excercise() const { return m_excercise; }
	void Excercise(std::string val) { m_excercise = val; }
	int X1() const { return x1; }
	void X1(int val) { x1 = val; }
	int X2() const { return x2; }
	void X2(int val) { x2 = val; }
	int Y1() const { return y1; }
	void Y1(int val) { y1 = val; }
	int Y2() const { return y2; }
	void Y2(int val) { y2 = val; }
private:
	int x1, y1, x2, y2;
	
	std::string m_cameraId;
	std::string m_excercise;
};

class ResolutionRegister{
public:
	bool trialCoverExcercise(const std::string& fileName, const std::string& inputFolder, const std::string& ex);
	ResolutionRegister(const std::string fp /*file path*/);
	void writeToConsole();
	cv::Rect findROI(const std::string in, bool lookForCamOnly=false, const std::string& inputFolder="");
	cv::Rect findROI(const std::string in, int width, int height, bool lookForCamOnly=false, const std::string& inputFolder="");

private:
	std::vector<Resolution> m_resolutions;
};
#endif // main_h__
