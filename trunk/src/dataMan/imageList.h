#ifndef imageList_h__
#define imageList_h__
#include "cv.hpp"
#include "cxcore.hpp"
#include "highgui.h"
#include "boost/filesystem.hpp"

#include <fstream>

namespace fs=boost::filesystem;

typedef enum {
	IDT_NOT_SPECIFIED,
	IDT_TRAIN,
	IDT_TEST,
	IDT_TRASH
} IM_DATA_TYPE;

template <class TImageImplementation>
class ImageData_{
public:
	typedef IM_DATA_TYPE ImageKindType;
	ImageData_();
	std::string fileName;
	TImageImplementation image;
	std::vector<cv::Point> centers;
	std::vector<cv::Point> area;
	virtual cv::Point center(){
		if (!centers.empty())
			return centers[0];
		return cv::Point(-1, -1);
	};
	IM_DATA_TYPE type;

	bool isLoaded() const {
		return !image.empty();
	};

	bool isCentered() const {
		return center.x==-1;
	}
};

template <class TImageImplementation>
ImageData_<TImageImplementation>::ImageData_():type(IDT_NOT_SPECIFIED)
{}

template <>
class ImageData_ <cv::Mat> {
public:
	std::string fileName;
	cv::Mat image;
	std::vector<cv::Point> centers;
	std::vector<cv::Point> area;
	virtual cv::Point center(){
		if (!centers.empty())
			return centers[0];
		return cv::Point(-1, -1);
	};

	IM_DATA_TYPE type;
	bool isLoaded() const {
		return image.cols>0;
	};

};

typedef ImageData_ <cv::Mat> CVImage;

template <class TImageData, int loaderType>
class ImageLoader {
	static bool loadImage(const std::string& fn, TImageData& image) {
		image.load(fn);
	};
};

const int _LT_FILE_LOADER = 0;

template <>
class ImageLoader <cv::Mat, _LT_FILE_LOADER> {
public:
	static bool loadImage(const std::string& fn, cv::Mat& image, std::vector<cv::Point> &centers, IM_DATA_TYPE &type, std::vector<cv::Point> &area=std::vector<cv::Point>()) {
		image=cv::imread(fn);
		return loadImageInfo(fn, centers, type, area);
	};

	static bool loadImageInfo(const std::string& fn, std::vector<cv::Point> &centers, IM_DATA_TYPE &type, std::vector<cv::Point> &area=std::vector<cv::Point>()) {
		std::string stfn=fn.substr(0, fn.length()-3)+"yml";
		cv::FileStorage st(stfn, CV_STORAGE_READ);
		short t;
		//std::vector<cv::Point> pts;
		st["centers"]>>centers;
		st["area"]>>area;
		st["type"]>>t;
		if (t==1)
			type=IDT_TRAIN;
		else
			if (t==2)
				type=IDT_TEST;
			else
				if (t==3)
					type=IDT_TRASH;
				else
					type=IDT_NOT_SPECIFIED;
		return true;
	}
};

typedef ImageLoader <cv::Mat, _LT_FILE_LOADER> CVFileLoader;
//template <>
//bool ImageData<cv::Mat>::isLoaded() const 
template <class TImageData, int loaderType>
class ImageList {
public:
	typedef TImageData ImageDataType;
	ImageList(const std::string& fn="");
	bool loadFileNames(const std::string& fn);
	bool loadFromFolder(const std::string& fn, std::string& ext);
	void showImage(int index, int wait=0);
	TImageData* getImage(int index);
	size_t size();
	bool empty() const;
protected:
	typedef std::vector<typename TImageData> ImagesContainerType;
	std::vector<TImageData> images;
	//bool loadImage(std::vector<TImageData>::iterator imIt);
};

template <class TImageData, int loaderType>
bool ImageList<TImageData, loaderType>::loadFromFolder( const std::string& fn, std::string& ext )
{
		const fs::path folderPath(fn);
		if (!fs::exists(folderPath)) {
			std::cout<<"Katalog: "<<fn<<" nie istnieje.";
			return false;
		}

		fs::directory_iterator endItr;
		for ( fs::directory_iterator itr(folderPath); itr != endItr; ++itr ){
			if (itr->path().extension().string()==ext){
				TImageData newImage;
				newImage.fileName=itr->path().string();
				images.push_back(newImage);
			}
		}


}

template <class TImageData, int loaderType>
bool ImageList<TImageData, loaderType>::empty() const
{
	return images.empty();
	
	
}

template <class TImageData, int loaderType>
size_t ImageList<TImageData, loaderType>::size()
{
	return images.size();
}

template <class TImageData, int loaderType>
TImageData* ImageList<TImageData, loaderType>::getImage( int index )
{
	TImageData* im=&(images[index]);
	if (!im->isLoaded())
		CVFileLoader::loadImage(im->fileName, im->image, im->centers, im->type, im->area);

	if (im->isLoaded()){
		CVFileLoader::loadImageInfo(im->fileName, im->centers, im->type, im->area);
		return im;
	}
}

template <class TImageData, int loaderType>
void ImageList<TImageData, loaderType>::showImage( int index, int wait/*=0*/ )
{
	//auto it=images.begin()+index;
	TImageData* im=&(images[index]);
	if (!im->isLoaded())
		CVFileLoader::loadImage(im->fileName, im->image, im->center, im->type, im->area);

	if (im->isLoaded()){
		CVFileLoader::loadImageInfo(im->fileName, im->center, im->type, im->area);
		cv::imshow("Test", im->image);
		cv::waitKey(wait);
	}
}

//template <class TImageData, int loaderType>
//bool ImageList<TImageData, loaderType>::loadImage( std::vector<TImageData>::iterator imIt )
//{
//	CVFileLoader::loadImage(imIt->fileName, imIt->image);
//}

template <class TImageData, int loaderType>
ImageList<TImageData, loaderType>::ImageList( const std::string& fn/*=""*/ )
{
	if (fn!="")
		loadFileNames(fn);
}

template <class TImageData, int loaderType>
bool ImageList<TImageData, loaderType>::loadFileNames( const std::string& fn )
{
	bool fileWasGood=false;
	std::ifstream file (fn);

	while (file.good()){
		TImageData newImage;
		std::getline(file, newImage.fileName);
		images.push_back(newImage);
		fileWasGood=true;
	}

	file.close();
	return fileWasGood;
}

typedef ImageList<CVImage, _LT_FILE_LOADER> CVImageList;

#endif // imageList_h__
