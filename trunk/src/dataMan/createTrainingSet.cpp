#pragma once
#include "createTrainingSet.h"

using namespace CreateTrainingSets;

YMLConverter::YMLConverter(std::string s_path)
{
	this->s_path = s_path;
}

void YMLConverter::YMLtoDAT()
{
	fs::path p_path(s_path);

	if(fs::is_directory(p_path))
	{
		std::string dirIMG = s_path+"\\img";		
		CreateDirectoryA(dirIMG.c_str(),NULL);			// positive sample folder
		std::string dirN_IMG = s_path+"\\Nimg";
		CreateDirectoryA(dirN_IMG.c_str(),NULL);		// negative sample folder
		std::string dirYML = s_path+"\\YML";
		CreateDirectoryA(dirYML.c_str(),NULL);		// negative sample folder

		std::fstream fs_dat(s_path+"\\data.dat", std::ios::out);	// .dat file - dir to positive, and position face from .yml  	
		std::fstream fs_neg(s_path+"\\neg.txt", std::ios::out);		// path to negative sample
		std::fstream fs_log(s_path+"\\log.txt", std::ios::out);		// log file

		fs::directory_iterator end_iter;
		fs::directory_iterator dir_iter(p_path);
		for(dir_iter; dir_iter !=end_iter; ++dir_iter)
		{
			if (dir_iter->path().extension().string() == ".yml" )
			{
				cv::FileStorage st(dir_iter->path().string(), cv::FileStorage::READ);
				std::string s_fileName = dir_iter->path().stem().string();
				std::string s_jpg = s_path + "\\" + s_fileName + ".jpg";

				std::vector<cv::Point> area;
				if (!st["area"].isNone() && boost::filesystem::exists(s_jpg))
				{
					st["area"]>>area;

					int x = std::min(area[0].x,area[1].x);
					int y = std::min(area[0].y,area[1].y);
					int w = std::abs(area[0].x-area[1].x);
					int h = std::abs(area[0].y-area[1].y);


					fs_dat<<"img\\"<<s_fileName <<".jpg" << "  1  "  << x <<" "<< y <<" "<< w <<" "<< h <<"\n";
					st.release();

					//tworzenie negatywnego
					cv::Rect r(x,y,w,h);
					cv::Mat img = cv::imread(s_jpg,1);
					cv::Mat dst;
					cv::rectangle(img,r,cv::Scalar(0,0,0),CV_FILLED);
					cv::resize(img,dst,cv::Size(20,20));
					cv::imwrite(dirN_IMG+"\\"+s_fileName+"N.jpg",dst);
					fs_neg<<"Nimg\\"<<s_fileName <<"N.jpg \n";

					//kopiowanie pozytywnego do \IMG
					MoveFileA(s_jpg.c_str(),(dirIMG + "\\" + s_fileName + ".jpg").c_str());
					MoveFileA(dir_iter->path().string().c_str(),(dirYML + "\\" + s_fileName + ".yml").c_str());
				}
				else
				{
					fs_log<<"Nie udalo sie przetworzyc pliku - "<<s_fileName<<"\n";
				}
			}
		}

	}else
	{
		std::cout<<"Bledna sciezka";
	}

}

void YMLConverter::runCreateSamples()
{


}

