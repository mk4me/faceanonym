#pragma once
#ifndef createTrainingSet_h__
#define createTrainingSet_h__
#include "opencv.hpp"
#include "highgui.h"
#include <iostream>
#include "imageList.h"
#include <fstream> 

namespace CreateTrainingSets
{

	class YMLConverter
	{
	private:
		std::string s_path;

	public:
		YMLConverter(std::string path);
		
		// odczytuje pliki YML i tworzy .dat ze œcierzka i koordynatami twarzy
		void YMLtoDAT();

		// uruchamia funkcje opencv_createsamples
		void runCreateSamples();
				
		// uruchamia opencv_cascadeTrain
		void cascadeTrain();
		
		// uruchamia opencv_haadrtrain
		void haadrTrain();
	};
}

#endif // createTrainingSet_h__
