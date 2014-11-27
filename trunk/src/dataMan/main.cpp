#include "main.h"
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
#include <QtCore/QTextCodec>
#include <QTGUI/QListView>
#include <time.h>
#include "createTrainingSet.h"
#include "Anonymization.h"
//#include "boost/archive/binary_oarchive.hpp"
//#include "boost/archive/binary_iarchive.hpp"

namespace fs=boost::filesystem;
namespace an=Anonimizator;
namespace cts= CreateTrainingSets;
using namespace std;
std::ofstream logFile;

void set_color(int c) {
	HANDLE uchwyt;
	uchwyt = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(uchwyt,10);
	
}

void coloredText(const std::string& t){
	int c=0;
	for (int i=0; i<t.size(); ++i){
		++c;
		if (c>15)
			c=1;
		set_color(c);
		cout<<t[i];

	}
};

std::string ToUTF8(const std::string& x) {
	QString qs(x.c_str());
	QString qb(qs.toUtf8());
	return qb.toStdString();
}

std::vector<std::string> loadFile(const std::string& fn){
	std::vector<std::string> data;

	std::ifstream f;
	f.open(fn);

	while (!f.eof() && f.good())
	{
		std::string line;
		std::getline(f, line);
		data.push_back(line);
	}

	return data;
}

std::pair<std::string, std::string> parseENFLine(const std::string& line){

	size_t pos=line.find("=");
	if (pos==std::string::npos)
		return std::pair<std::string, std::string>("", "");
	
	return std::pair<std::string, std::string>(line.substr(0, pos), line.substr(pos+1, line.length()-pos));
}

void asmAnonymize(std::string fileName)
{
	/*std::system("cls");
	std::cout<<"Welcome to AMC anonymizer. \n===============================================\n";
	std::cout<<"Enter folder path:\n> ";*/
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	
	//getline(cin, folderPath);
	std::cout<<"\n Anonimizing file ..."<< fileName;
	std::ifstream iFile(fileName);
			
		
	std::stringstream data;
	bool firstLine=true;
	while (!iFile.eof() && iFile.good())
	{
		std::string line;
		std::getline(iFile, line);
				
		//std::cout << line<<"\n";
		if (!firstLine)
			data<<line+"\n";
		else
		{
			string fp=fs::path(fileName).filename().string();
			data<<"#!OML:ASF "<<fp.substr(0, fp.length()-8)<<".asf\n";
			firstLine=false;
		}
	}
	iFile.close();
	std::ofstream oFile(fileName);
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

std::string  extractDate( std::string data, int pos, bool withTime=false, int start=19 )
{
	std::string out;
	std::string s;
	out=data.substr(pos+start, 4);
	data.erase(0, pos+start+5);
	s=data.substr(0, data.find(","));
	data.erase(0, s.length()+1);
	if (s.length()<2)
		s="0"+s;
	s="-"+s;
	out+=s;

	s=data.substr(0, data.find(","));
	data.erase(0, s.length()+1);
	if (s.length()<2)
		s="0"+s;
	s="-"+s;
	out+=s;

	
	if (withTime){
		s=data.substr(0, data.find(","));
		data.erase(0, s.length()+1);
		if (s.length()<2)
			s="0"+s;
		s=" "+s;
		out+=s;

		s=data.substr(0, data.find(","));
		data.erase(0, s.length()+1);
		if (s.length()<2)
			s="0"+s;
		s=":"+s;
		out+=s;

		s=data.substr(0, data.find(","));
		//data.erase(0, s.length()+1);
		if (s.length()<2)
			s="0"+s;
		s=":"+s;
		out+=s;
	}
	return out;
}

void getSessInfoFromFile(const std::string& fn, std::string& bodyName, std::string& date, std::string& bodyId)
{
	std::ifstream sessInfoFile;
	sessInfoFile.open(fn);

	while (!sessInfoFile.eof() && sessInfoFile.good())
	{
		std::string data;
		std::getline(sessInfoFile, data);
		//sessInfoFile>>data;
		//std::cout << data <<"\n";
		int pos;
		if (pos=data.find("CREATIONDATEANDTIME=")!=std::string::npos)
			date=extractDate(data, pos);

		if (pos=data.find("NAME=")!=std::string::npos)
		{
			bodyName=data.substr(pos+4, data.length());
		}

		if (pos=data.find("ID=")!=std::string::npos)
		{
			bodyId=data.substr(pos+2, data.length());
		}
	}

}

std::string findEPart(const std::string& fileStem){
	size_t pos=fileStem.find("E");
	if (pos==string::npos)
		return "";

	return fileStem.substr(pos, 7);
}

std::string findTPart(const std::string& fileStem){
	size_t pos=fileStem.find_first_of("T");
	if (pos==string::npos)
		return "";

	return fileStem.substr(pos, 3);
}

std::string findFileSufix(const fs::path& fp , int& lastTrial, std::string& lastEPart )
{
	//cout<<"\nfp="<<fp<<";\nlastTrial="<<lastTrial<<";\nlastEPart="<<lastEPart<<";\n";
	std::string fileStem=fp.stem().string();

	std::string TPart;
	
	std::string EPart=findEPart(fileStem);
	//cout<<"ffs:1";
	if (EPart=="")
		TPart=findTPart(fileStem);
	else{
		if (EPart!=lastEPart)
			lastTrial+=1;
		std::stringstream s;
		s<<lastTrial;
		TPart=s.str();
		if (TPart.length()<2)
			TPart="0"+TPart;
		TPart="T"+TPart;
		lastEPart=EPart;
	}
	//cout<<"2";
	bool first=true;
	int posT=fileStem.length();
	//cout<<"|posT="<<posT<<"|";
	while(posT!=std::string::npos)
	{
		posT=fileStem.find_last_of("T", posT-1);

		if ( ((posT==std::string::npos) || (posT==0)) && first )//if this is a first passage of loop and "T" is not found it means that there is no T in file stem so there is no sufix.
		{										//if this is not a first passage it means that loop exit condition is fulfilled and sufix will be returned.
			if (fileStem.find(".")!=std::string::npos) 
				posT=-1;
			else
				return "";						
		}

		first=false;

		int posEnd=fileStem.find(".", posT+1); //if after T is dot we should only find out if between T and . are digits only

		if (posEnd==std::string::npos) //if after T there is no dot we should find out if after T are digits only
			posEnd=fileStem.length();
		bool nonDigitSignFound=false;
		if (posT>-1)	//if posT==-1 that means that no "T" was found so we shouldn't chack if after T are digits only.
			for (int i=posT+1; i<posEnd-1; i++)
				if ((fileStem[i]<48) || (fileStem[i]>57)) //ascii code from [48, 57] are digits [0, 9].
				{
					nonDigitSignFound=true; //this is not (T)rial part.
					break;
				}

				//if we are here it means that fileStem contains TrialPart - we should save them
				if (posT>-1)
				{
					if (!nonDigitSignFound){
						std::string x;
						if ( (fp.extension().string().find("avi")!=string::npos) || (fp.extension().string().find("AVI")!=string::npos) )
							x=TPart+fileStem.substr(posEnd, fileStem.length());
						else
							x=TPart;
						return x;
					}
				}
				else
				{
					
					return TPart;//if "T" not found but "." found we should return everything beginning from dot.
				}
	}
	//cout<<"3:ffs ";

	return "";
}

std::string sessionId(const std::string& sf){
	std::string out;
	size_t p=sf.find("S");
	if (p==string::npos)
		return "";
	size_t pk=sf.find("-", p);
	if (pk==string::npos)
		pk=sf.length()-p;
	else
		pk=pk-p+1;
	return sf.substr(p+1, pk);
}

bool fileExists(const fs::path& fp){

	if ( (fp.extension().string()!=".c3d") && (fp.extension().string()!=".AVI") && (fp.extension().string()!=".avi") )
		return true;

	fs::directory_iterator ite;
	for (fs::directory_iterator it(fp.branch_path()); it!=ite; ++it){
		std::string toFind=fp.stem().filename().string();
		if ((fp.extension().string()==".avi") || (fp.extension().string()==".AVI")){
			toFind=toFind.substr(0, toFind.find("."));
			if ( (it->path().extension()==".c3d") && (it->path().string().find(toFind)!=string::npos) )
				return true;
		}
		
		if (fp.extension().string()==".c3d") {
			if ( ( (it->path().extension()==".avi") || (it->path().extension()==".AVI")) && (it->path().string().find(toFind)!=string::npos) )
				return true;
		}
	}

	return false;
}

bool isTrash(std::vector<std::string> trashes, const fs::path& fp, const std::string& date){
	//std::string ext=fp.extension().string();
	for (auto it=trashes.begin(); it!=trashes.end(); ++it){
		if (fp.string().find((*it))!=string::npos)
			return true;
		if (fp.extension().string().find(*it)!=string::npos)
			return true;
	}
	std::string ext=fp.extension().string();

	if (fp.filename().string().find(date)==string::npos)
	{
		
		if ((ext.find("mkr")==std::string::npos) &&
			(ext.find("mp")==std::string::npos) &&
			(ext.find("vsk")==std::string::npos) &&
			(ext.find("system")==std::string::npos) &&
			(ext.find("xcp")==std::string::npos) &&
			(ext.find("mod")==std::string::npos)&&
			(ext.find("asf")==std::string::npos)){
			logFile<<"\nTRASHData:"<<fp.string()<<"\n";
			return true;
		}
	}
	/*if (fp=="F:/dev/k.wereszczynski/cvl/data/user/dataMan/Parkinson/Brygida Dziewiecka\\2012-08-24-B0062-S01\\PlugInGait.mkr")
		int dummy=5;*/
	size_t p=fp.string().find("Trial");
	if ( (p!=string::npos)&&(fp.string().find("Trial", p)!=string::npos) )
		return true;

	if ( (ext.find("c3d")==std::string::npos) && (!fileExists(fp)) )
		return true;

	if ( (ext.find("avi")==std::string::npos) && (!fileExists(fp)) )
		return true;

	if ( (ext.find("AVI")==std::string::npos) && (!fileExists(fp)) )
		return true;

	return false;
}

std::string getValueFromENF(const std::string& key, std::vector<std::string> d, const std::string fn, int offset=0, string returnIfEmpty=""){
	for (auto it=d.begin(); it!=d.end(); ++it)
		if (size_t p=it->find(key)!=string::npos){
			std::string rv=it->substr(p+key.length()+offset, it->size());
			if (rv=="")
				rv=returnIfEmpty;
			return rv;
		}

	logFile<<"Warning: [Couldn't find key <"<<key<<"> in ENF file: "<<fn<<"]\n";
	return "0";
}

void writeENFfieldsToXML(std::ofstream& file, std::vector<std::string> data){
	for (auto it = data.begin(); it!=data.end(); ++it){
		std::pair<std::string, std::string> v=parseENFLine(*it);
		if ( (v.first!="") && (v.first!="TRIAL")&&(v.first!="NAME") &&(v.first!="Reference") && (v.first!="HASCHILD") && (v.first!="PARENT") ){
			if (v.first=="CREATIONDATEANDTIME")
				v.second=extractDate(v.second, 0, true, 0);
			if ((v.first!="SESSION") || (v.second.length()>2))
				file<<"	<"<<v.first<<">"<<ToUTF8(v.second)<<"</"<<v.first<<">\n";
		}
	}
}

void copyPatientInfo(const std::string& inputFolder, const std::string& output, const std::string& bid){
	std::string cpyPath=output+"\\medyczne";
	if (!fs::exists(cpyPath))
		fs::create_directory(cpyPath);

	fs::directory_iterator itEnd;
	for (fs::directory_iterator fit(inputFolder);fit != itEnd;++fit){
		if (fs::is_regular_file(fit->path())){
			fs::copy_file(fit->path(), fs::path(cpyPath+"\\B"+bid+fit->path().extension().string()));
			if (fit->path().extension()==".enf"){
				std::vector<std::string> d=loadFile(fit->path().string());
				bool exists=(fs::exists(output+"\\xml_pacjenci.xml"));
				std::ofstream f;
				f.open(output+"\\xml_pacjenci.xml", std::ios::app);
				if (f.is_open()){
					if (!exists)
						f<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
					f<<"<PATIENT ID=\""<<bid<<"\">\n";
					f<<"	<PATH>"<<cpyPath+"\\B"+bid+".enf</PATH>\n";
					writeENFfieldsToXML(f, d);
					/*f<<"	<BIRTH_DATE>"<<getValueFromENF("BIRTH_DATE", d, fit->path().string())<<"</BIRTH DATE>\n";
					f<<"	<CREATIONDATEANDTIME>"<<""<<"</CREATIONDATEANDTIME>\n";
					f<<"	<DIABETES>"<<getValueFromENF("DIABETES", d, fit->path().string())<<"</DIABETES>\n";
					f<<"	<DISEASE>"<<getValueFromENF("DISEASE", d, fit->path().string())<<"</DISEASE>\n";
					f<<"	<FIRST_NAME>"<<getValueFromENF("Adam", d, fit->path().string())<<"</FIRST_NAME>\n";
					f<<"	<HIGH_BLOOD_PRESSURE>"<<getValueFromENF("HIGH_BLOOD_PRESSURE", d, fit->path().string())<<"</HIGH_BLOOD_PRESSURE>\n";
					f<<"	<LAST_NAME>"<<getValueFromENF("LAST_NAME", d, fit->path().string())<<"</LAST_NAME>\n";
					f<<"	<SEX>"<<getValueFromENF("SEX", d, fit->path().string())<<"</SEX>\n";
					f<<"	<SICK>"<<getValueFromENF("SICK", d, fit->path().string())<<"</SICK>\n";*/
					f<<"</PATIENT>\n";
					f.close();
				}
				else
					logFile<<"!!!!Error: [Cannot access or create file <xml_pacjenci.xml>]\n";
			}
		}
	}
}

void saveSessionToXML(const std::string& xmlName, std::vector<std::string> d, const std::string& enfName, const std::string& fileStem, const std::string& of){
	//std::vector<std::string> d=loadFile(enfName);
	bool exists=(fs::exists(xmlName));
	std::ofstream f;
	f.open(xmlName, std::ios::app);
	if (f.is_open()){
		if (!exists)
			f<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		f<<"<SESSION=\""<<fileStem<<"\">\n";
		f<<"	<PATH>"<<of<<"</PATH>\n";
		writeENFfieldsToXML(f, d);
		/*f<<"	<ACCLAIM>"<<getValueFromENF("ACCLAIM", d, enfName)<<"</ACCLAIM>\n";
		f<<"	<CREATIONDATEANDTIME>"<<extractDate(getValueFromENF("CREATIONDATEANDTIME", d, enfName), 0, true, 0)<<"</CREATIONDATEANDTIME>\n";
		f<<"	<DESCRIPTION>"<<getValueFromENF("DESCRIPTION", d, enfName)<<"</DESCRIPTION>\n";
		f<<"	<EMG_SET>"<<getValueFromENF("EMG_SET", d, enfName)<<"</EMG_SET>\n";
		f<<"	<EXCEL_REPORT>"<<getValueFromENF("EXCEL_REPORT", d, enfName)<<"</EXCEL_REPORT>\n";
		f<<"	<GRF_SET>"<<getValueFromENF("GRF_SET", d, enfName)<<"</GRF_SET>\n";
		f<<"	<LAB_SET>"<<getValueFromENF("LAB_SET", d, enfName)<<"</LAB_SET>\n";
		f<<"	<LIGHT_SET>"<<getValueFromENF("LIGHT_SET", d, enfName)<<"</LIGHT_SET>\n";
		f<<"	<MARKER_SET>"<<getValueFromENF("MARKER_SET", d, enfName)<<"</MARKER_SET>\n";
		f<<"	<NIR_SET>"<<getValueFromENF("NIR_SET", d, enfName)<<"</NIR_SET>\n";
		f<<"	<POLYGON_REPORT>"<<getValueFromENF("POLYGON_REPORT", d, enfName)<<"</POLYGON_REPORT>\n";
		f<<"	<STATUS>"<<getValueFromENF("STATUS", d, enfName)<<"</STATUS>\n";
		f<<"	<VIDEO_SET>"<<getValueFromENF("VIDEO_SET", d, enfName)<<"</VIDEO_SET>\n";*/
		f<<"</SESSION>\n";
		f.close();
	}
	else
		logFile<<"!!!!Error: [Cannot access or create file <"<<xmlName<<">]\n";
}

std::string extractBody(const std::string& fileStem){
	size_t p=fileStem.find_last_of("B");
	if (p==string::npos)
		return "";

	return fileStem.substr(p, 5);
}



void saveTrialToXML(const std::string& xmlName, std::vector<std::string> d, const std::string& enfName, const std::string& fileStem, const std::string& of){
	//std::vector<std::string> d=loadFile(enfName);
	std::ofstream f;
	bool exists=(fs::exists(xmlName));
	f.open(xmlName, std::ios::app);
	if (f.is_open()){
		if (!exists)
			f<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		f<<"<TRIAL=\""<<fileStem<<"\">\n";
		f<<"	<PATH>"<<of<<"</PATH>\n";
		writeENFfieldsToXML(f, d);
		/*f<<"	<CREATIONDATEANDTIME>"<<extractDate(getValueFromENF("CREATIONDATEANDTIME", d, enfName), 0, true, 0)<<"</CREATIONDATEANDTIME>\n";
		f<<"	<DESCRIPTION>"<<getValueFromENF("DESCRIPTION", d, enfName)<<"</DESCRIPTION>\n";
		f<<"	<FP1>"<<getValueFromENF("FP1", d, enfName)<<"</FP1>\n";
		f<<"	<FP2>"<<getValueFromENF("FP2", d, enfName)<<"</FP2>\n";
		f<<"	<NOTES>"<<getValueFromENF("NOTES", d, enfName)<<"</NOTES>\n";
		f<<"	<STAGES>"<<getValueFromENF("STAGES", d, enfName)<<"</STAGES>\n";
		f<<"	<SUBJECTS>"<<extractBody(fileStem)<<"</SUBJECTS>\n";*/
		f<<"</TRIAL>\n";
		f.close();
	}
	else
		logFile<<"!!!!Error: [Cannot access or create file <"<<xmlName<<">]\n";
}

void saveENFToXML(const std::string& enfName, const std::string& outputFolder, const std::string& of){
	std::vector<std::string> d=loadFile(enfName);
	if (getValueFromENF("TYPE", d, enfName)=="TRIAL")
		saveTrialToXML(outputFolder+"\\xml_triale.xml", d, enfName, fs::path(of).stem().string(), of);

	if (getValueFromENF("TYPE", d, enfName)=="SESSION")
		saveSessionToXML(outputFolder+"\\xml_sesje.xml", d, enfName, fs::path(of).stem().string(), of);
}

void saveAntropometricData(const std::string& outputFolder, const std::string& mpName, const std::string& sessionId){
	bool writeHeader=!(fs::exists(outputFolder+"\\dane antropometryczne.csv"));
	std::vector<std::string> d=loadFile(mpName);
	std::ofstream f;
	f.open(outputFolder+"\\dane antropometryczne.csv", std::ios::app);

	if (writeHeader){
		f<<"Patient ID and Session number;Body Mass;Height;Inter Asis Distance;Left Leg Length;Right Leg Lenght;Left Knee Width;Right Knee Width;Left Ankle Width;Right Ankle Width;";
		f<<"Left Circuit Thigh;Right Circuit Thigh;Left Circuit Shank;Right Circuit Shank;Left Shoulder Offset;Right Shoulder Offset;Left Elbow Width;Right Elbow Width;";
		f<<"Left Wrist Width;Right Wrist Width;Left Wrist Thickness;Right Wrist Thickness;Left Hand Width;Right Hand Width;Left Hand Thickness;Right Hand Thickness\n";
	}
	f<<sessionId<<";";
	f<<std::stof(getValueFromENF("$Bodymass", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$Height", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$InterAsisDistance", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LLegLength", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RLegLength", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LKneeWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RKneeWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LAnkleWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RAnkleWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LCircuitThigh", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RCircuitThigh", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LCircuitShank", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RCircuitShank", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LShoulderOffset", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RShoulderOffset", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LElbowWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RElbowWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LWristWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RWristWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LWristThickness", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RWristThickness", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LHandWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RHandWidth", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$LHandThickness", d, mpName, 2, "0"))<<";";
	f<<std::stof(getValueFromENF("$RHandThickness", d, mpName, 2, "0"))<<"\n";
	f.close();
}

void createOutputData(const std::string& inputFolder, const std::string& outputFolder, const std::string& bodyId, const std::string& sessionFolder, const std::string& date, std::vector<std::string> trashes){
	std::srand(std::time(NULL));
	//tworzenie xmla z list¹ æwiczeñ dla ka¿dej sesji.

	std::ofstream xmlIndex;
	xmlIndex.open(outputFolder+"\\index_"+sessionFolder+".xml", std::ios::app);
	xmlIndex<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	xmlIndex<<"<session name=\""<<sessionFolder<<"\" date=\""<<date<<"\" subjectId=\"B"<<bodyId<<"\" shortName=\"S"<<sessionId(sessionFolder)<<"\">\n";
	//xmlIndex<<"  <measurement date=\""<<date<<"\" id=\"B"<<bodyId<<"\" session=\""<<sessionId(sessionFolder)<<"\">\n";
	fs::path folderPath(inputFolder);

	//fs::recursive_directory_iterator fit(folderPath); //folder iterator
	fs::directory_iterator itrEnd;
	int i=0;
	std::string commandFull;
	std::string filePrefix=date+"-B"+bodyId+"-S"+sessionId(sessionFolder);
	std::string sof=outputFolder+"\\"+sessionFolder;
	if (!fs::exists(sof))
		fs::create_directory(sof);

	std::vector<fs::path> files;
	

	for (fs::directory_iterator fit(folderPath);fit != itrEnd;++fit)
		files.push_back(fit->path());
	sort(files.begin(), files.end());
	int trial=0;
	std::string lastEPart;
	
	for (auto it=files.begin(); it!=files.end(); ++it)
	{
		if (!isTrash(trashes, *it, date))
		{
			//std::cout<<fit->path().generic_string<std::string>()<<"\nstem="<<fit->path().stem().generic_string<std::string>() << "\nextention="<< fit->path().extension().generic_string<std::string>() <<"\n";
			//logFile<<"1";
			int tbefore=trial;
			std::string fileSufix=findFileSufix(*it, trial, lastEPart);
			//logFile<<"a";
			if (tbefore!=trial){
				std::stringstream ts;
				ts<<trial;
				std::string tst=ts.str();
				if (tst.length()<2)
				  tst="0"+tst;

				xmlIndex<<"    <trial name=\"T"<<tst<<"\">";
				tst=lastEPart.substr(1, 2);
				if (tst.substr(0,1)=="0")
					tst=tst.substr(1,1);
				xmlIndex<<"<ExerciseNo>"<<tst<<"</ExerciseNo></trial>\n";
			}
			//logFile<<"2";
			if ( (fileSufix!="") && (fileSufix[0]!='.') )
				fileSufix="-"+fileSufix;
		
			fs::path newPath=sof+"\\"+filePrefix+fileSufix+it->extension().generic_string<std::string>();
			//std::cout<<"rename file: "<<newPath.generic_string<std::string>()<<"\n";
		
			std::string ext=newPath.extension().generic_string<std::string>();
		
			if (ext.find("mp")!=std::string::npos){
				saveAntropometricData(outputFolder, it->string(), sessionFolder);
			}
			///logFile<<"3";
			if (ext.find("enf")!=std::string::npos){
				saveENFToXML(it->string(), outputFolder,newPath.string());
			}
			else{
				try{
					set_color(std::rand() % 255);
					fs::copy_file(*it, fs::path(newPath));
					cout<<"Copied file from:\n"<<*it<<"\nto:\n"<<newPath<<"\n";
					logFile<<"OK: [copy from: "<<*it<<" to: "<<newPath<<"\n";
					//cout<<"O";
					///logFile<<"4";
				}
				catch(...)
				{
					set_color(std::rand() % 255);
					cout<<"Copying FAILED from:\n"<<*it<<"\nto:\n"<<newPath<<"\n";
					logFile<<"!!!!Error: [copy from: "<<*it<<" to: "<<newPath<<"]\n";
				}
				
				if (ext.find("amc")!=std::string::npos)
					asmAnonymize(newPath.string());
				//logFile<<"5";

				if ((ext.find("mkr")!=std::string::npos) ||
					(ext.find("mp")!=std::string::npos) ||
					(ext.find("vsk")!=std::string::npos) ||
					(ext.find("system")!=std::string::npos) ||
					(ext.find("xcp")!=std::string::npos) ||
					(ext.find("mod")!=std::string::npos))
				{
					set_color(std::rand() % 255);
					std::cout<<newPath.string()<<"\n[7zip output:\n";
					std::string cmd="7z.exe a \""+sof+"\\"+filePrefix+".zip\" \""+newPath.string()+"\"\n";
					commandFull=commandFull+" && "+cmd;
					/*for (;std::system(NULL);)
						std::cout<<"-";*/
					set_color(std::rand() % 255);
					std::cout<<std::system(cmd.c_str());
					set_color(std::rand() % 255);
					std::cout<<"\nend of 7zip output];\n";
					i++;
					fs::remove(newPath);
					//logFile<<"6";
					logFile<<"7zip: [file: "<<*it<<" added to archive: "<<newPath<<"]\n";
				}
			}
		}
		else{
			set_color(std::rand() % 255);
			cout<<"This is trash: "<<*it<<"\n";
			logFile<<"TRASH: [file: "<<*it<<"]\n";
		}
	}
	xmlIndex<<"  </session>\n";
	xmlIndex.close();
	set_color(std::rand() % 255);
	std::cout<<"Files copied.\n";
	//for (fs::directory_iterator fit(sof);
	//	fit != itrEnd;
	//	++fit){

	//		std::string fileSufix=findFileSufix(fit->path().stem().generic_string<std::string>(), trial, lastEPart);
	//		if ( (fileSufix!="") && (fileSufix[0]!='.') )
	//			fileSufix="-"+fileSufix;

	//		fs::path newPath=fit->path();
	//		//std::cout<<"rename file: "<<newPath.generic_string<std::string>()<<"\n";

	//		std::string ext=newPath.extension().generic_string<std::string>();
	//		if ((ext.find("amc")==std::string::npos) &&
	//			(ext.find("asf")==std::string::npos) &&
	//			//(ext.find("avi")==std::string::npos) &&
	//			(ext.find("c3d")==std::string::npos) &&
	//			(ext.find("enf")==std::string::npos))
	//		{

	//			std::cout<<newPath.string()<<"\n[7zip output:\n";
	//			std::string cmd="7z.exe a "+sof+"\\"+filePrefix+".zip "+newPath.string()+"\n";
	//			commandFull=commandFull+" && "+cmd;
	//			for (;std::system(NULL);)
	//				std::cout<<"-";
	//			std::cout<<std::system(cmd.c_str());
	//			std::cout<<"\nend of 7zip output];\n";
	//			i++;
	//			fs::remove(newPath);
	//		}
	//}
	//std::cout<<i<<"File(s) zipped\n"<<"\n";
}

std::vector<std::string> loadTrashes(const std::string& fn){

	std::vector<std::string> t;
	std::ifstream f;
	f.open(fn);

	while (!f.eof() && f.good())
	{
		std::string data;
		std::getline(f, data);
		t.push_back(data);
	}

	return t;
}

std::vector<std::string> normalizeDirList(QStringList l){
	std::vector<std::string> v;
	for (auto it=l.begin(); it!=l.end(); ++it){
		bool found=false;
		for (auto cit=l.begin(); cit!=l.end(); ++cit){
			if (cit!=it){
				if (cit->toStdString().find(it->toStdString())!=std::string::npos)
					found=true;
			}
		}
		if (!found)
			v.push_back(it->toStdString());
	}

	return v;
}

void folderOrganizer(){
	std::system("cls");
	std::cout<<"Welcome to measurement data folder ORGANIZER\n===============================================\n";
	std::cout<<"Enter input and output folder path:\n> ";
	//std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick input folders").toStdString();
	QFileDialog d(0);
	d.setDirectory("");
	d.setFileMode(QFileDialog::DirectoryOnly);
	d.setOption(QFileDialog::DontUseNativeDialog);
	d.setWindowTitle("Pick Folder containing PATIENT data. You can pick more then one folder.");
	QListView *l=d.findChild<QListView*>("listView");
	l->setSelectionMode(QAbstractItemView::MultiSelection);

	d.exec();
	//QStringList names=d.selectedFiles();
	std::vector<std::string> names=normalizeDirList(d.selectedFiles());
	
	std::string outputfolder=QFileDialog::getExistingDirectory(0, "Pick output folder").toStdString();
	std::string trashFile=QFileDialog::getOpenFileName(0, "Pick file containing extensions of trashes:", "", "Text file with trashes (*.txt);All Files (*.*)").toStdString();
	std::vector<std::string> trashes=loadTrashes(trashFile);
	fs::recursive_directory_iterator itrEnd;
	fs::directory_iterator iEnd;
	std::string body="BXXX";
	std::string date, name;
	for (auto qit=names.begin(); qit!=names.end(); ++qit){
		std::string folderPath=*qit;
		
		for (fs::directory_iterator fit(folderPath);
			fit != iEnd;
			++fit)
		{
			if (fit->path().extension()==".enf"){
				std::string bn, sd, bi;
				getSessInfoFromFile(fit->path().string(), bn, sd, bi);
				if (fit->path().string().find("Patient")!=string::npos){
					body=bi;
					copyPatientInfo(folderPath, outputfolder, bi);
				}
			}
		}

		for (fs::recursive_directory_iterator fit(folderPath);
			fit != itrEnd;
			++fit)
		{
			if (fs::is_directory(fit->path())/*fit->status().type()==fs::file_type::directory_file*/){
				for (fs::directory_iterator pit(fit->path());
					pit != iEnd;
					++pit)
				{
					if (pit->path().string().find("Session")!=string::npos){
						std::string bn, sd, bi;
						getSessInfoFromFile(pit->path().string(), bn, sd, bi);
						createOutputData(fit->path().string(), outputfolder, body, bn, sd, trashes);
						break;
					}
				}
			}
		}
	}
	//delete l;
}

void folderFixer()
{
	std::system("cls");
	std::cout<<"Welcome to measurement data folder FIXER\n===============================================\n";
	std::cout<<"Enter folder path:\n> ";
	std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick Folder for one body (patient)").toStdString();
	//getline(cin, folderPath);
	folderPath+="\\";
	std::cout<<"\nEnter info file name. If its the same as the last part of folder just press ENTER:\n>";
	/*char input[256];
	std::cin.getline(input, 256);*/
	std::string infoFileName=QFileDialog::getOpenFileName(0, "Pick file with body information (ENF)", QString::fromStdString(folderPath), "body information file (*.enf)").toStdString();
	
	//getline(cin, infoFileName);

	//CFolderFixer sessionInfo("f:\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\", "2010-12-15-P02-S01.Session 1.enf");
	//CFolderFixer sessionInfo("f:\cvl\data\user\dataMan\2010-12-15-P02-S01\", "2010-12-15-P02-S01.Session 1.enf");
	
	asmAnonymize(folderPath);

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
	int x=800;
	int y=600;
	std::string folderPath;
	getline(cin, folderPath);
	std::cout<<"x res: >";
	cin>>x;
	std::cout<<"y res: >";
	cin>>y;
	std::string suffix;
	std::cout<<"file suffix: >";
	cin>>suffix;
	fs::recursive_directory_iterator itrEnd;
	int i=0;
	std::string commandFull;
	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		if (!logFile.is_open())
			logFile.open("log.txt");
		bool processed=false;
		bool shown=false;
		std::string fileSuffix=fit->path().stem().generic_string<std::string>().substr(fit->path().stem().generic_string<std::string>().length()-suffix.length(), suffix.length());
		processed=(fileSuffix==suffix);
		if (processed)
		{
			std::cout<<"File: "<<fit->path().generic_string<std::string>()<<" is actually output file!\n";
			logFile<<"File: "<<fit->path().generic_string<std::string>()<<" is actually output file!\n";
			shown=true;
		}
		std::string branchPath=fit->path().branch_path().generic_string<std::string>();
		std::string outputFileName=branchPath+"\\"+fit->path().stem().generic_string<std::string>()+suffix+".avi";
		if (!processed)
			processed=(fs::exists(outputFileName));
		if (processed && !shown)
		{
			std::cout<<"File: "<<fit->path().generic_string<std::string>()<<" already processed!\n";
			logFile<<"File: "<<fit->path().generic_string<std::string>()<<" already processed!\n";
		}
		if (fit->path().extension().generic_string<std::string>().find("avi")!=std::string::npos && !processed)
		{
			logFile<<"=================================================================\nStart processing file: \""<<fit->path().generic_string<std::string>()<<"\"\n";
			int codec;
			int fps;
			cv::VideoCapture capture(fit->path().string());
			codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
			fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));

			cv::VideoWriter writer;
			
			writer.open(outputFileName,CV_FOURCC('X','V','I','D'), fps,cv::Size(x, y) );
			std::cout<<"Changing resolution for file: "<<fit->path()<<" ... \nNr of frame processed\n";
			bool end=false;
			int fc=0;
			while (!end)
			{
				++fc;
				cv::Mat frame;
				if (capture.read(frame))
				{
					cv::Mat newFrame(x, y, frame.type());
					cv::resize(frame, newFrame, cv::Size(x, y));
					
					/*cv::imshow("Src", frame);
					cv::imshow("Dst", newFrame);
					cv::waitKey(1);*/
					
					writer.write(newFrame);
					std::cout<<" "<<fc;
				}
				else
					end=true;

			}
			
			capture.release();
			writer.~VideoWriter();
			/*fs::remove(fit->path());
			fs::rename(fs::path(outputFileName), fit->path());*/
			cout<<"done.\n\n";
			logFile<<"Output file created: \""<<outputFileName<<"\"\n=================================================================\n\n";
		}

		//logFile.close();
	}
}

cv::Mat detectFacesd(cv::Mat image, std::string& cascade)
{
	double min_face = 20, max_face = 200;
	cv::CascadeClassifier face_cascade(cascade);
	std::vector<cv::Rect> faces;

	face_cascade.detectMultiScale(image,faces,1.2,2,0|CV_HAAR_SCALE_IMAGE, cv::Size(min_face,min_face),cv::Size(max_face,max_face));

	for(int i=0; i<faces.size(); i++)
	{
		min_face = faces[0].width*0.8;
		max_face = faces[0].width*1.2;
		cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		cv::ellipse(image,center,cv::Size(faces[i].width*0.5,faces[i].height*0.5),0,0,360,cv::Scalar(255,0,255),4,8,0);
	}
	return image;
}



const int N = 2;
std::vector<cv::Mat> buf(N);
int last = 0;

cv::Mat mhi;

// img - input video frame
// dts - result motion picture
void update_mhi(const cv::Mat& img, cv::Mat& dst, int diff_treshold, std::vector<cv::Rect> brect)
{
	double timestamp = (double)clock()/CLOCKS_PER_SEC; // aktualny czas w sekundach
	int idx1 = last, idx2;
	cv::Mat silh, orient, mask, segmask;

	cvtColor(img, buf[last],CV_BGR2GRAY); // do czarno-bioa³ego

	idx2 = (last+1) % N; // index od ostatniej minus (n-1) ramki
	last = idx2;

	if (buf[idx1].size() != buf[idx2].size())
		silh = cv::Mat::ones(img.size(),CV_8U)*255;
	else
		absdiff(buf[idx1], buf[idx2], silh); // roznica pomiedzy rramkami

	cv::threshold(silh, silh, diff_treshold, 1, CV_THRESH_BINARY); // tresholduj


	if(mhi.empty())
		mhi = cv::Mat::zeros(silh.size(),CV_32F);
	cv::updateMotionHistory(silh, mhi, timestamp, 1);


	mhi.convertTo(mask, CV_8U, 255./1,(1-timestamp)*255./1);
	dst = cv::Mat::zeros(mask.size(),CV_8UC3);
	cv::insertChannel(mask,dst,0);

	// liczymy motiongradient orientatnion 
	cv::calcMotionGradient(mhi, mask, orient, 0.5, 0.05, 3);

	
	std::vector<cv::Rect> brects = brect;
	//segmentMotion(mhi,segmask,brects,timestamp,0.5);


	cv::Mat image = img;
	for (int i=-1; i<(int)brects.size(); i++)
	{
		cv::Rect roi;
		cv::Scalar color;
		double magnitude;

		cv::Mat maski = mask;
		
		if (i<0) // dla ca³ego obrazka
		{
			roi = cv::Rect(0,0,img.cols, img.rows);
			color = cv::Scalar::all(255);
			magnitude = 100;
		}
		else
		{
			roi = brects[i];
			color = cv::Scalar(0,0,255);
			magnitude = 30;
			maski = mask(roi);
		}

		// policzyc orientacje
//		cv::imshow("orient", orient(roi));
	//	cv::imshow("maski", maski);
		//cv::imshow("mhi",mhi(roi));
	//	cv::waitKey(1);


		double angel = cv::calcGlobalOrientation(orient(roi), maski, mhi(roi),timestamp,1);
		angel = 360.0-angel;  // bo mamy 0,0 w lewym górnym

		int count = norm(silh, cv::NORM_L1); 

		if(count < roi.area()*0.05)
			continue;
		
		//rysujemy
		cv::Point center(roi.x + roi.width/2, roi.y + roi.height/2);
		cv::circle(dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0);
		cv::line(dst, center, cv::Point(cvRound(center.x+magnitude*cos(angel*CV_PI/180)), cvRound(center.y-magnitude*sin(angel*CV_PI/180))),color, 3, CV_AA, 0);

		cv::circle(image, center, cvRound(magnitude*1.2), color, 1, CV_AA, 0);
		cv::line(image, center, cv::Point(cvRound(center.x+magnitude*cos(angel*CV_PI/180)), cvRound(center.y-magnitude*sin(angel*CV_PI/180))),color, 1, CV_AA, 0);
	}

//	imshow("Image", image);
//	cv::waitKey(1);
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
	
	std::string cascadeFrontalPath="C:\\Users\\dev\\Desktop\\faces\\mov\\cascade2000N.xml";
	std::string cascadeProfilePath="C:\\Users\\dev\\Desktop\\faces\\mov\\cascade2000N.xml";
	std::string movieFrontalPath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.60981847.avi";
	std::string movieFrontalPath1="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T08.60981847.avi";
	std::string movieFrontalPath2="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2011-03-04-B0005-S02\\2011-03-04-B0005-S02-T07.60981847.avi";
	std::string movieProfilePath1="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.59461898.avi";
	std::string movieProfilePath2="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.53875336.avi";
	std::string movieBackPath="F:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01.cpy\\Aleksander Spalek_LB_T01.56339527.avi";
	
	//CFaceAnonymizer faceAnonymizer(movieFrontalPath, cascadeFrontalPath, cascadeProfilePath);


	an::CFaceFinder facefind(folderPath, cascadeFrontalPath, cascadeProfilePath);
	std::vector<an::pairFrameFace> faces = facefind.getAllDetectedFaces();

	an::CContours conturs(100);

	for (int i=0; i< faces.size(); i++)
	{
		cv::Mat src = faces[i].first.clone();
		for (int j = 0; j< faces[i].second.size(); j++)
		{
			cv::rectangle(src,faces[i].second[j],cv::Scalar(0,255,0),3);
		}
		cv::imshow("src",src);
		cv::waitKey(1);
	//	cv::Rect re = faces[i].second[0];
	//	cv::Mat src1 = faces[i].first(re);
	//	cv::imshow("cut", src1);
	///*	conturs.findContoursCanny(src1,true);
	//	std::vector<cv::Point2f> point;
	//	conturs.getContours(point);*/
	//	cv::waitKey(10);
	/*	an::COpticalFlowPLK ofPLK;
		ofPLK.init(point);
		ofPLK.trackPosition(src);*/
	}

	return;

	//COpticalFlowPLK ofPLK;
	//ofPLK.init(255);
	//CvCapture* capture = cvCreateFileCapture(folderPath.c_str());
	//IplImage* image,imaA;

	//cv::Rect face = m_faces[0][0];
	//int x = face.x;
	//int y = face.y;
	//CvPoint2D32f* pts_cur(x,y);
	//
	//while(1)
	//{

	//	if (cvGrabFrame(capture)==0)
	//		break;
	//	image = cvRetrieveFrame(capture);
	//	IplImage* gray = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
	//	cvCvtColor(image,gray, CV_RGB2GRAY);
	//	int i = ofPLK.trackPosition(gray);
	//	CvPoint2D32f* ctrk;
	//	ofPLK.getCurrentTrackedPosition(&ctrk);
	//	CvPoint2D32f* ptrk;
	//	ofPLK.getPreviousTrackedPosition(&ptrk);

	//	cv::Mat imgMat(image);
	//	int c;
	//	ofPLK.getCount(&c);
	//	

	//	for (int i=0; i<c;i++)
	//	{
	//		//cv::circle(imgMat,cvPointFrom32f(ctrk[i]),5, cv::Scalar(0,0,255),2);
	//		cv::circle(imgMat,cvPointFrom32f(ptrk[i]),2, cv::Scalar(0,255,0),1);
	//		cv::line(imgMat,cvPointFrom32f(ctrk[i]),cvPointFrom32f(ptrk[i]),cv::Scalar(0,0,255),2);
	//	}

	//	cv::imshow("mat",imgMat);
	//	cv::waitKey(1);
	//}

	//cv::Mat frame;
	//cv::Mat back;
	//cv::Mat fore;
	//cv::VideoCapture cap(folderPath);
	//cv::BackgroundSubtractorMOG2 bg;
	//bg.nmixtures = 3;
	//bg.bShadowDetection = false;

	//std::vector<std::vector<cv::Point>> conturs;

	//cv::namedWindow("Frame");
	//
	//bool ifend = false;
	//while (!ifend)
	//{
	//	cap >> frame;
	//	if (cap.read(frame))
	//	{
	//		bg.operator()(frame, fore);
	//		bg.getBackgroundImage(back);
	//		cv::erode(fore,fore,cv::Mat());
	//		cv::dilate(fore,fore,cv::Mat());
	//		cv::findContours(fore,conturs,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	//		cv::drawContours(frame,conturs,-1,cv::Scalar(0,0,255),2);


	//		std::vector<cv::Rect> boundRect( conturs.size() );
	//		for (int i=0; i<conturs.size(); i++)
	//		{
	//			boundRect[i]=boundingRect(cv::Mat(conturs[i]));
	//		}

	//		for (int i=0; i<conturs.size(); i++)
	//		{
	//			cv::rectangle(frame,boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0,255,0),2);
	//		}

	//		cv::Size size(frame.size().width/2,frame.size().height/2);
	//		cv::resize(frame,frame,size);
	//		cv::imshow("Frame", frame);
	//		//cv::imshow("Background", back);
	//		cv::waitKey(40);
	//	}
	//	else
	//		ifend = true;
	//}
	//cv::imshow("Background", back);
	//cv::waitKey(1);
	//cv::imwrite("C:\\Users\\dev\\Desktop\\faces\\mov\\testy.jpg",back);
	//cap.release();

	////ramka po ramce wolniej
	//cv::VideoCapture cap(folderPath);
	//cv::namedWindow("tets",1);

	//cv::CascadeClassifier m_clasFrontal(cascadeFrontalPath);
	//std::vector <std::vector<cv::Rect>> objects;

	//while (1)
	//{
	//	std::vector <cv::Rect> area;
	//	cv::Mat frame;
	//	cap >> frame;
	//	m_clasFrontal.detectMultiScale(frame, area);
	//	objects.push_back(area);
	//	frame=detectFacesd(frame,cascadeProfilePath);
	//	imshow("tets",frame);
	//	cv::waitKey(10);
	//}

	//cv::waitKey(0);
	



	
	//CFaceAnonymizer faceAnonymizer(folderPath, cascadeFrontalPath, cascadeProfilePath);
	//while (1)
	//{
	//	if (faceAnonymizer.isOpended())
	//	{
	//		cv::Mat frame=faceAnonymizer.getAnonymizedFrame();
	//		if (frame.cols>10)
	//		{
	//			cv::imshow("Anonymizing", frame);
	//			cv::waitKey(40);
	//		}
	//	}

	//}
	

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





	//optical flow testy
	


	/*cv::Mat motion;
	cv::VideoCapture cap;
	cap.open(folderPath);

	if(!cap.isOpened())
		return;

	CFaceFinder facefind(folderPath, cascadeFrontalPath, cascadeProfilePath);
	std::vector<std::vector<cv::Rect>> m_faces = facefind.getAllDetectedFaces();
	int i = 0;
	while (1)
	{
		cv::Mat frame;
		cap >> frame;
		
		if(frame.empty())
			break;

		update_mhi(frame, motion, 30, m_faces[1]);
		imshow("Calc global orient", motion);
		cv::waitKey(400);
		i++;
	}*/

	
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

//int WINAPI WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine,
//	int nCmdShow)
//{};
	//int argc;
	//char** argv;
	////QApplication::setDesktopSettingsAware(false);
	//QApplication app(argc, argv);
	//
	//QtAnonimizationForm* anmForm=new QtAnonimizationForm(0);
	//anmForm->show();
	//app.exec();
	//return 0;

	//QFileDialog::getOpenFileName(0, "Pick folder name");
	//anmForm->show();
	//while(1);



int cropMovie(const std::string& inputFileName, const std::string& outputFileName, ResolutionRegister r, bool provideExercice, bool preview, int codec){
			int fps;
			cv::VideoCapture capture(inputFileName);
			//codec=static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
			fps=static_cast<int>(capture.get(CV_CAP_PROP_FPS));
			
			int height=capture.get(CV_CAP_PROP_FRAME_HEIGHT);
			int width=capture.get(CV_CAP_PROP_FRAME_WIDTH);
			
			cv::VideoWriter writer;
			cv::Rect roiRect=r.findROI(inputFileName, width, height, !provideExercice);
			writer.open(outputFileName, -1/*CV_FOURCC('X','V','I','D')*/, fps,cv::Size(roiRect.width, roiRect.height) );
			std::cout<<"Cropping file: "<<inputFileName<<"\n";
			bool end=false;
			int fc=0;
			while (!end)
			{
				++fc;
				cv::Mat frame;
				if (capture.read(frame))
				{
					cv::Mat newFrame;
					frame(roiRect).copyTo(newFrame);
					//cv::copyMakeBorder(frame, newFrame, roiRect.x, roiRect.y, roiRect.x+roiRect.width, roiRect.y+roiRect.height, cv::BORDER_DEFAULT);
					if (preview){
						cv::imshow("Src", frame);
						cv::imshow("Dst", newFrame);
						cv::waitKey(1);
					}
					writer.write(newFrame);
					std::cout<<" "<<fc;
				}
				else
					end=true;

			}
			capture.release();
			writer.~VideoWriter();
			if (codec==-1){
				cv::VideoCapture c(outputFileName);
				codec=c.get(CV_CAP_PROP_FOURCC);
				c.release();
			}
			return codec;
}

void cropping(){
	std::system("cls");
	std::cout<<"Welcome to movie CROPPER-DROPPER.\n===============================================\n";
	std::cout<<"Enter resolution file...> ";
	std::string resFilePath=QFileDialog::getOpenFileName(0,"Pick file with cropping schema definitions", "","Text files (*.txt)").toStdString();
	std::cout<<"done.\n";
	//getline(cin, resFilePath);
	ResolutionRegister resolutions(resFilePath);
	resolutions.writeToConsole();
	std::cout<<"Enter input folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick folder with input data").toStdString();
	std::cout<<"done.\n";
	//getline(cin, folderPath);
	std::cout<<"Enter output folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	std::string outputFolderPath=QFileDialog::getExistingDirectory(0, "Pick (or create) folder, where output data should be placed").toStdString();
	std::cout<<"done.\n";
	if (!fs::exists(outputFolderPath))
		fs::create_directory(outputFolderPath);

	bool provideExercise;
	std::cout<<"Are Information about exercises in file name? [Y/N] >";
	std::string ec;
	getline(cin, ec);
	
	provideExercise=(ec=="Y" || ec=="y");

	bool preview;
	std::cout<<"Preview? [Y/N] >";
	ec="";
	getline(cin, ec);

	preview=(ec=="Y" || ec=="y");

	fs::recursive_directory_iterator itrEnd;
	
	int i=0;
	std::string commandFull;
	int codec=-1;
	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		std::string branchPath=fit->path().branch_path().generic_string<std::string>();
		branchPath=branchPath.substr(folderPath.size(), branchPath.size()-folderPath.size());
		std::string outputFileName=outputFolderPath+"\\"+branchPath+"\\";
		if (!fs::exists(outputFileName))
			fs::create_directory(outputFileName);

		outputFileName=outputFileName+fit->path().stem().generic_string<std::string>()+".avi";
		if (!fs::exists(outputFileName))
		{
			if (fit->path().extension().string()==".avi" || fit->path().extension().string()==".AVI")
				codec=cropMovie(fit->path().string(), outputFileName, resolutions, provideExercise, preview, codec);
		}
	}
}

std::string  normalizeFilename(const std::string& in){
	std::string out;
	

	if (in.size()==0)
		return std::string();

	std::string x=in.substr(0,1);

	for (int i=1; i<in.length(); ++i){
		std::string sa=in.substr(i,1);
		std::string sp=in.substr(i-1,1);

		if ( !((( sp=="\\") || ( sp=="/")) && (( sa=="\\") || ( sa=="/"))) )
			x+=sa;
	}

	for (int i=0; i<x.length(); ++i){
		std::string s=x.substr(i,1);
		if ( ( s=="\\") || ( s=="/"))
			out+="\\\\";
		else
			out+=s;
	}
	return out;
}

void prepareVDJobList(){

	std::system("cls");
	std::cout<<"Welcome to video CROPPER.\n===============================================\n";
	std::cout<<"Enter resolution file...> ";
	std::string resFilePath=QFileDialog::getOpenFileName(0,"Pick file with cropping schema definitions", "","Text files (*.txt)").toStdString();
	std::cout<<"done.\n";
	//getline(cin, resFilePath);
	ResolutionRegister resolutions(resFilePath);
	resolutions.writeToConsole();
	std::cout<<"Enter folders containing AVI files intended to be cropped:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	//std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick folder containing AVI files intended to be cropped").toStdString();
	QFileDialog d(0);
	d.setDirectory("");
	d.setFileMode(QFileDialog::DirectoryOnly);
	d.setOption(QFileDialog::DontUseNativeDialog);
	QListView *l=d.findChild<QListView*>("listView");
	l->setSelectionMode(QAbstractItemView::MultiSelection);
	d.exec();
	//QStringList names=d.selectedFiles();
	std::vector<std::string> names=normalizeDirList(d.selectedFiles());
	std::cout<<"done.\n";
	//getline(cin, folderPath);
	//std::cout<<"Enter output folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	
	/*std::string outputFolderPath=QFileDialog::getExistingDirectory(0, "Pick (or create) folder, where output data should be placed").toStdString();
	std::cout<<"done.\n";
	std::cout<<outputFolderPath<<"\n";
	if (!fs::exists(outputFolderPath))
		fs::create_directory(outputFolderPath);*/
	//std::cout<<"Enter VD script file path:\n> ";
	////std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	//std::string jobFilePath=QFileDialog::getSaveFileName(0, "Job list file", "", "Virtual Dub job scripts (*.jobs)").toStdString();
	//std::cout<<"done.\n";

	//std::ofstream jf(jobFilePath); //Job file

	bool provideExercise;
	std::cout<<"Are Information about exercises in file name? [Y/N] >";
	std::string ec;
	getline(cin, ec);

	provideExercise=(ec=="Y" || ec=="y");

	fs::recursive_directory_iterator itrEnd;

	int i=0;
	std::string commandFull;

	std::cout<<"Compression level [0-256, best: 15-20]: >";
	int clev; //compression level;
	cin>>clev;


	std::cout<<"\nI'm writing script for you. Do you know what doesn't mean????\n";
	for (auto qit=names.begin(); qit!=names.end(); ++qit)
	{
		/*std::string branchPath=fit->path().branch_path().generic_string<std::string>();
		branchPath=branchPath.substr(folderPath.size(), branchPath.size()-folderPath.size());
		std::string outputFileName=folderPath+"\\"+branchPath+"\\";
		if (!fs::exists(outputFileName))
			fs::create_directory(outputFileName);

		outputFileName=outputFileName+fit->path().stem().string()+".avi";*/
		//outputFileName=normalizeFilename(outputFileName);
		//if (!fs::exists(outputFileName))
		std::string folderPath = *qit;
		for (fs::recursive_directory_iterator fit(folderPath);
			fit != itrEnd;
			++fit)
		{
			if (fit->path().extension().string()==".avi" || fit->path().extension().string()==".AVI"){
				++i;
				std::string inputFileName=normalizeFilename(fit->path().string());
				cv::Rect r=resolutions.findROI(fit->path().string(), 1920, 1080, !provideExercise, folderPath);
				std::string newPath=fit->path().branch_path().string()+"\\_orig_"+fit->path().filename().string();
				fs::rename(fit->path(), fs::path(newPath));
				
				std::string inputFilePath=fit->path().parent_path().string()+"\\"+fit->path().stem().string()+".txt";
				std::ifstream file(inputFilePath);
				std::string p, k;
				std::getline(file, p);
				std::getline(file, k);
				file.close();

				std::stringstream cmd;
				cmd<<"ffmpeg -i \""<<newPath<<"\" -c:v libxvid -q:v "<<clev<<" -vf crop="<<r.width<<":"<<r.height<<":"<<r.x<<":"<<r.y<<" \""<<fit->path().string()<<"\"";
				std::cout<<std::system(cmd.str().c_str());
				fs::remove(newPath);
				//jf<<"// \"VirtualDub job list (Sylia script format)\"\n";
				//jf<<"// This is a program generated file -- edit at your own risk. Edited by dataMan. DataMan is only an application - it doesn't know what risk is. So... risk is still yours :D\n";
				//jf<<"//\n";
				//jf<<"// $numjobs "<<i<<"\n";
				//jf<<"//\n";

				//jf<<"// $job \"Job "<<i<<"\"\n";
				//jf<<"// $input \""<<inputFileName<<"\"\n";
				//jf<<"// $output \""<<outputFileName<<"\"\n";
				//jf<<"// $state 0\n";
				//jf<<"// $id "<<i<<"\n";
				//jf<<"// $start_time 00000000 00000000\n";
				//jf<<"// $end_time 00000000 00000000\n";
				//jf<<"// $script\n";

				//jf<<"VirtualDub.Open(\""<<inputFileName<<"\",\"\",0);\n";
				//jf<<"VirtualDub.audio.SetSource(1);\n";
				//jf<<"VirtualDub.audio.SetMode(1);\n";
				//jf<<"VirtualDub.audio.SetInterleave(1,500,1,0,0);\n";
				//jf<<"VirtualDub.audio.SetClipMode(1,1);\n";
				//jf<<"VirtualDub.audio.SetConversion(0,0,0,0,0);\n";
				//jf<<"VirtualDub.audio.SetVolume();\n";
				//jf<<"VirtualDub.audio.SetCompression();\n";
				//jf<<"VirtualDub.audio.EnableFilterGraph(0);\n";
				//jf<<"VirtualDub.video.SetInputFormat(0);\n";
				//jf<<"VirtualDub.video.SetOutputFormat(7);\n";
				//jf<<"VirtualDub.video.SetMode(3);\n";
				//jf<<"VirtualDub.video.SetSmartRendering(0);\n";
				//jf<<"VirtualDub.video.SetPreserveEmptyFrames(0);\n";
				//jf<<"VirtualDub.video.SetFrameRate2(0,0,1);\n";
				//jf<<"VirtualDub.video.SetIVTC(0, 0, 0, 0);\n";
				//jf<<"VirtualDub.video.SetCompression(0x64697678,0,10000,0);\n";
				//jf<<"VirtualDub.video.SetCompData(10436,\"AAAAAEAGAAA04ggALlx2aWRlby5wYXNzAAAuAHAAYQBzAHMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAZAAAAE1QRUc0IFNpbXBsZSBAIEwzAHAAbABlACAAQAAgAEwAMwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAChVc2VyIGRlZmluZWQpAABmAGkAbgBlAGQAKQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAIERITFRcZGxESExUXGRscFBUWFxgaHB4VFhcYGhweIBYXGBocHiAjFxgaHB4gIyYZGhweICMmKRscHiAjJiktEBESExQVFhcREhMUFRYXGBITFBUWFxgZExQVFhcYGhsUFRYXGRobHBUWFxgaGxweFhcYGhscHh8XGBkbHB4fIQAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAACWAAAAZAAAAAEAAAABAAAAAAAAAAQAAAADAAAAAQAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAGQAAAD0AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAZAAAAGQAAAABAAAACgAAAAEAAAAUAAAAAAAAAAAAAAAFAAAABQAAAAUAAAAAKAoAAAAAAAEAAAABAAAAHgAAAAAAAAACAAAAAAAAAAAAAACAAAAAl0kBAAAAAAAGAAAAAQAAAAAAAAABAAAAAQAAAAAAAAAAAAAAGQAAAAAAAAABAAAAHwAAAAEAAAAfAAAAAQAAAB8AAAABAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAM8DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAAA=\");\n";
				//jf<<"VirtualDub.video.filters.Clear();\n";
				//jf<<"VirtualDub.video.filters.Add(\"null transform\");\n";
				//jf<<"VirtualDub.video.filters.instance[0].SetClipping("<<r.tl().x<<","<<r.tl().y<<","<<r.br().x<<","<<r.br().y<<");\n";
				//jf<<"VirtualDub.SaveAVI(\""<<outputFileName<<"\");\n";
				//jf<<"VirtualDub.Close();\n";
				//jf<<"// $endjob\n";
				//jf<<"\n";
				//std::cout<<"+";
				//jf<<s.str();
			}
			else
				std::cout<<"-";
		}
		/*else
			std::cout<<".";*/
	}
	//jf.close();
	std::cout<<"\nYeap. done.\nSay: \"Thank you my computer\"\n";
	Sleep(3000);
	std::cout<<"SAY IT!\n";
	Sleep(3000);
	std::cout<<"I'm sad\n";
	Sleep(2000);
	std::cout<<"I will format all your discs. You are not a good man!\n";
	Sleep(4000);
	std::cout<<"By the way - do you know, what this \"+\", \"-\" and \".\" means? He?\n";
	Sleep(5000);
	std::cout<<"You didn't thank so I will NOT say you, you computer head!\n";
	//std::system("pause");
}

void cropFromStartToEnd(){

	std::system("cls");
	std::cout<<"Welcome to video CROPPER (start, end).\n===============================================\n";
	//std::cout<<"Enter resolution file...> ";
	//std::string resFilePath=QFileDialog::getOpenFileName(0,"Pick file with cropping schema definitions", "","Text files (*.txt)").toStdString();
	//std::cout<<"done.\n";
	////getline(cin, resFilePath);
	//ResolutionRegister resolutions(resFilePath);
	//resolutions.writeToConsole();
	std::cout<<"Enter folders containing AVI files intended to be cropped:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	//std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick folder containing AVI files intended to be cropped").toStdString();
	QFileDialog d(0);
	d.setDirectory("");
	d.setFileMode(QFileDialog::DirectoryOnly);
	d.setOption(QFileDialog::DontUseNativeDialog);
	QListView *l=d.findChild<QListView*>("listView");
	l->setSelectionMode(QAbstractItemView::MultiSelection);
	d.exec();
	//QStringList names=d.selectedFiles();
	std::vector<std::string> names=normalizeDirList(d.selectedFiles());
	std::cout<<"done.\n";
	//getline(cin, folderPath);
	//std::cout<<"Enter output folder path:\n> ";
	//std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	
	/*std::string outputFolderPath=QFileDialog::getExistingDirectory(0, "Pick (or create) folder, where output data should be placed").toStdString();
	std::cout<<"done.\n";
	std::cout<<outputFolderPath<<"\n";
	if (!fs::exists(outputFolderPath))
		fs::create_directory(outputFolderPath);*/
	//std::cout<<"Enter VD script file path:\n> ";
	////std::string folderPath="f:\\dev\\k.wereszczynski\\cvl\\data\\user\\dataMan\\2010-12-15-P02-S01\\";
	//std::string jobFilePath=QFileDialog::getSaveFileName(0, "Job list file", "", "Virtual Dub job scripts (*.jobs)").toStdString();
	//std::cout<<"done.\n";

	//std::ofstream jf(jobFilePath); //Job file

	/*bool provideExercise;
	std::cout<<"Are Information about exercises in file name? [Y/N] >";
	std::string ec;
	getline(cin, ec);

	provideExercise=(ec=="Y" || ec=="y");*/

	fs::recursive_directory_iterator itrEnd;

	int i=0;
	std::string commandFull;

	std::cout<<"Compression level [0-256, best: 15-20]: >";
	int clev; //compression level;
	cin>>clev;


	std::cout<<"\nCropping movies\n";
	for (auto qit=names.begin(); qit!=names.end(); ++qit)
	{
		/*std::string branchPath=fit->path().branch_path().generic_string<std::string>();
		branchPath=branchPath.substr(folderPath.size(), branchPath.size()-folderPath.size());
		std::string outputFileName=folderPath+"\\"+branchPath+"\\";
		if (!fs::exists(outputFileName))
			fs::create_directory(outputFileName);

		outputFileName=outputFileName+fit->path().stem().string()+".avi";*/
		//outputFileName=normalizeFilename(outputFileName);
		//if (!fs::exists(outputFileName))
		std::string folderPath = *qit;
		for (fs::recursive_directory_iterator fit(folderPath);
			fit != itrEnd;
			++fit)
		{
			if (fit->path().extension().string()==".avi" || fit->path().extension().string()==".AVI"){
				++i;
				std::string inputFileName=normalizeFilename(fit->path().string());
				//cv::Rect r=resolutions.findROI(fit->path().string(), 1920, 1080, !provideExercise, folderPath);
				std::string newPath=fit->path().branch_path().string()+"\\_orig_"+fit->path().filename().string();
				fs::rename(fit->path(), fs::path(newPath));
				std::string fileStem=fit->path().stem().string();
				fileStem=fileStem.substr(0, fileStem.find_last_of("."));
				std::string inputFilePath=normalizeFilename(fit->path().parent_path().string()+"\\"+fileStem+".txt");
				std::ifstream file(inputFilePath, std::ifstream::in);
				std::cout<<"g="<<file.good()<<"; eof="<<file.eof()<<";\n";
				int p, k;
				file>>p;
				std::cout<<"g="<<file.good()<<"; eof="<<file.eof()<<";\n";
				float pf, kf;
				file>>k;
				std::cout<<"g="<<file.good()<<"; eof="<<file.eof()<<";\n";
				pf=(float)p/100;
				int pm=pf/60;
				float ps=(pf-pm*60);
				kf=(float)(k-p)/100;
				int km=kf/60;
				float ks=(kf-km*60);
				/*std::getline(file, p);
				std::getline(file, k);*/
				file.close();
				//p.insert(p.length()-2, ".");
				//k.insert(k.length()-2, ".");
				std::stringstream cmd;
				cmd.precision(2);
				cmd.fill('0');
				cmd.width(2);
				std::string spm, sps;
				if (pm<10)
					spm="0";
				if (ps<10)
					sps="0";

				std::string skm, sks;
				if (km<10)
					skm="0";
				if (ks<10)
					sks="0";

				cmd<<"ffmpeg -i \""<<newPath<<"\" -ss 00:"<<spm<<pm<<":"<<sps<<ps<<" -t 00:"<<skm<<km<<":"<<sks<<ks<<" -c:v libxvid -q:v "<<clev<<" \""<<fit->path().string()<<"\"";

				std::cout<<std::system(cmd.str().c_str());
				fs::remove(newPath);
				//jf<<"// \"VirtualDub job list (Sylia script format)\"\n";
				//jf<<"// This is a program generated file -- edit at your own risk. Edited by dataMan. DataMan is only an application - it doesn't know what risk is. So... risk is still yours :D\n";
				//jf<<"//\n";
				//jf<<"// $numjobs "<<i<<"\n";
				//jf<<"//\n";

				//jf<<"// $job \"Job "<<i<<"\"\n";
				//jf<<"// $input \""<<inputFileName<<"\"\n";
				//jf<<"// $output \""<<outputFileName<<"\"\n";
				//jf<<"// $state 0\n";
				//jf<<"// $id "<<i<<"\n";
				//jf<<"// $start_time 00000000 00000000\n";
				//jf<<"// $end_time 00000000 00000000\n";
				//jf<<"// $script\n";

				//jf<<"VirtualDub.Open(\""<<inputFileName<<"\",\"\",0);\n";
				//jf<<"VirtualDub.audio.SetSource(1);\n";
				//jf<<"VirtualDub.audio.SetMode(1);\n";
				//jf<<"VirtualDub.audio.SetInterleave(1,500,1,0,0);\n";
				//jf<<"VirtualDub.audio.SetClipMode(1,1);\n";
				//jf<<"VirtualDub.audio.SetConversion(0,0,0,0,0);\n";
				//jf<<"VirtualDub.audio.SetVolume();\n";
				//jf<<"VirtualDub.audio.SetCompression();\n";
				//jf<<"VirtualDub.audio.EnableFilterGraph(0);\n";
				//jf<<"VirtualDub.video.SetInputFormat(0);\n";
				//jf<<"VirtualDub.video.SetOutputFormat(7);\n";
				//jf<<"VirtualDub.video.SetMode(3);\n";
				//jf<<"VirtualDub.video.SetSmartRendering(0);\n";
				//jf<<"VirtualDub.video.SetPreserveEmptyFrames(0);\n";
				//jf<<"VirtualDub.video.SetFrameRate2(0,0,1);\n";
				//jf<<"VirtualDub.video.SetIVTC(0, 0, 0, 0);\n";
				//jf<<"VirtualDub.video.SetCompression(0x64697678,0,10000,0);\n";
				//jf<<"VirtualDub.video.SetCompData(10436,\"AAAAAEAGAAA04ggALlx2aWRlby5wYXNzAAAuAHAAYQBzAHMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAZAAAAE1QRUc0IFNpbXBsZSBAIEwzAHAAbABlACAAQAAgAEwAMwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAChVc2VyIGRlZmluZWQpAABmAGkAbgBlAGQAKQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAIERITFRcZGxESExUXGRscFBUWFxgaHB4VFhcYGhweIBYXGBocHiAjFxgaHB4gIyYZGhweICMmKRscHiAjJiktEBESExQVFhcREhMUFRYXGBITFBUWFxgZExQVFhcYGhsUFRYXGRobHBUWFxgaGxweFhcYGhscHh8XGBkbHB4fIQAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAACWAAAAZAAAAAEAAAABAAAAAAAAAAQAAAADAAAAAQAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAGQAAAD0AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAZAAAAGQAAAABAAAACgAAAAEAAAAUAAAAAAAAAAAAAAAFAAAABQAAAAUAAAAAKAoAAAAAAAEAAAABAAAAHgAAAAAAAAACAAAAAAAAAAAAAACAAAAAl0kBAAAAAAAGAAAAAQAAAAAAAAABAAAAAQAAAAAAAAAAAAAAGQAAAAAAAAABAAAAHwAAAAEAAAAfAAAAAQAAAB8AAAABAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAM8DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAAA=\");\n";
				//jf<<"VirtualDub.video.filters.Clear();\n";
				//jf<<"VirtualDub.video.filters.Add(\"null transform\");\n";
				//jf<<"VirtualDub.video.filters.instance[0].SetClipping("<<r.tl().x<<","<<r.tl().y<<","<<r.br().x<<","<<r.br().y<<");\n";
				//jf<<"VirtualDub.SaveAVI(\""<<outputFileName<<"\");\n";
				//jf<<"VirtualDub.Close();\n";
				//jf<<"// $endjob\n";
				//jf<<"\n";
				//std::cout<<"+";
				//jf<<s.str();
			}
			else
				std::cout<<"-";
		}
		/*else
			std::cout<<".";*/
	}
	//jf.close();
	std::cout<<"\nYeap. done.\nSay: \"Thank you my computer\"\n";
	Sleep(3000);
	std::cout<<"SAY IT!\n";
	Sleep(3000);
	std::cout<<"I'm sad\n";
	Sleep(2000);
	std::cout<<"I will format all your discs. You are not a good man!\n";
	Sleep(4000);
	std::cout<<"By the way - do you know, what this \"+\", \"-\" and \".\" means? He?\n";
	Sleep(5000);
	std::cout<<"You didn't thank so I will NOT say you, you computer head!\n";
	//std::system("pause");
}

void openLog()
{
	/*if (!fs::exists("logs"))
	fs::create_directory("logs");

	fs::directory_iterator itEnd;
	int logNr=0;
	for (fs::directory_iterator fit(folderPath); fit != itrEnd; ++fit){
	if (!is_directory(fit))
	}*/

}

void removeOriginals(){
	std::system("cls");
	std::cout<<"Welcome to video CROPPER.\n===============================================\n";
	
	std::cout<<"Enter input folder path:\n> ";
	
	std::string folderPath=QFileDialog::getExistingDirectory(0, "Pick folder with input data").toStdString();
	std::cout<<"done.\n";

	fs::recursive_directory_iterator itrEnd;

	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		if (fit->path().extension().string()==".avi" || fit->path().extension().string()==".AVI"){
			if (fit->path().extension().string()==".avi" || fit->path().extension().string()==".AVI")
				if (fit->path().filename().string().find("_orig_")!=std::string::npos){
					fs::remove(fit->path());
					cout<<"-";
				}
		}
	}

	cout<<"done.\n";
}

//void detectorAnalysis (const std::string& folder, const std::string& subFolder, int cellW, int cellH) {
//	fs::directory_iterator itrEnd;
//
//	for (fs::directory_iterator fit(folder);fit != itrEnd;++fit){
//		std::string fn=fit->path().stem().string();
//		cv::Mat image=cv::imread(fit->path().string());
//		for (fs::directory_iterator dit(fs::path(folder)/fs::path(subFolder)); dit!=itrEnd; ++dit){
//			cv::FileStorage st(dit->path().string(), cv::FileStorage::READ);
//			std::vector<cv::KeyPoint> keyPoints;
//			st["keyPoints"]>>keyPoints;
//			for (auto it=keyPoints.begin(); it!=keyPoints.end(); ++it){
//				cv::circle(image, it->pt, 2, cv::Scalar(255,0,0), 2);
//			}
//		}
//	}
//}

int main( int argc, const char** argv )
{
// 	detectorAnalysis ("F:\\ds\\polnor", "results", 30, 30);
// 	exit(0);
	logFile.open("J:/log.txt");
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, (char**)argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1250"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1250"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1250"));
	/*std::string outputFolderPath=QFileDialog::getExistingDirectory(0, "Pick (or create) folder, where output data should be placed").toStdString();
	std::cout<<"done.\n";
	std::cout<<outputFolderPath<<"\n";*/
	app.setStyle("windows");
	bool end=false;

	while(!end)
	{
		std::system("cls");
		//std::cout<<ToUTF8("Œ¥æñó³")<<"\n";
		//std::system("dir");
		
		coloredText("                        **** WELCOME TO DB PROCESSOR ****\n");
		coloredText("                     THAT IS IN FACT HML<--->BDR Data Bridge!              \n");
		coloredText("                 but was known (in miserable part of its life) as\n");
		coloredText("                                  D A T A M A N\n\n");
		set_color(11);
		std::cout<<"           ^ --------------------------^-------------------------^         \n";
		std::cout<<"          / \\            ____         / \\                       / \\    \n";
		std::cout<<"         /   \\ brum     /__|_\\_      /   \\                     /   \\   \n";
		std::cout<<" HML >>>/>>>>>\\  brum  | o |  o \\ >>/>>>>>\\>>>>>>>>>>>>>>>>>>>/>>>>>\\>>> BDR \n";
		std::cout<<"=======/=======\\===================/=======\\=================/=======\\========\n";
		std::cout<<"     |/         \\                 /         \\               /         \\|  Uff:D\n";
		std::cout<<"     ||         ||               ||         ||             ||         ||\n";
		std::cout<<"     ||         ||               ||         ||             ||         ||\n";
		set_color(2);
		std::cout<<"================================================================================\n";
		set_color(15);
		//coloredText("Mowisz - masz Dawid! Chciales kolorowane - masz kolorowane!\n");

		coloredText("What do you want to do today? ;-)\nWell, maybe I'll ask what do you HAVE TO do today?\n\n\n");
		coloredText("1- folder organize\n2- video processing\n3-extras\n4-crop from start to end\n0- exit\n> ");
		set_color(3);
		int i;
		cin>>i;
		std::cin.ignore();
		switch (i)
		{
		case 1: {
			folderOrganizer();
			break;
				}
		case 2: {
			prepareVDJobList();
			break;
				}
		case 4: {
			cropFromStartToEnd();
			break;
				}
				/*case 3: {
				removeOriginals();
				break;
				}*/
				/*case 4: {
				asmAnonymize();
				break;
				}
				case 5: {
				classifierTraining();
				break;
				}
				case 6: {
				cropping();
				break;
				}
				case 7: {
				prepareVDJobList();
				break;
				}*/
		case 0: {
			end=true;
			break;
				}
		case 3: {
			//fa::YMLConverter ymcC = fa::YMLConverter("C:\\Users\\dev\\Desktop\\test");
		//	ymcC.YMLtoDAT();

			faceDetector();
			break;
				}
	default: {
			std::cout<<"Option not recognized..\n";
			std::system("pause");
				 }

		}
	}
	std::cout<<"Stopping QT... ";
	app.exit();
	std::cout<<"done.\n";
	std::system("pause");
}

Resolution::Resolution( const std::string& line )
{
	std::string x=static_cast<std::string>(line);
	
	bool change=true;
	while (change){
		change=false;
		int p=x.find(",");
		if (p!=string::npos){
			x=x.replace(p, 1, "\n");
			change=true;
		}
	}

	std::stringstream s;
	s<<x;
	s>>m_cameraId;
	s>>m_excercise;
	s>>x1>>y1>>x2>>y2;
}

ResolutionRegister::ResolutionRegister( const std::string fp /*file path*/ )
{
	std::ifstream f;
	f.open(fp);
	if (f.is_open())
	{
		while (!f.eof() && f.good()){
			std::string line;
			f>>line;
			m_resolutions.push_back(Resolution(line));
		}
	}
}

void ResolutionRegister::writeToConsole()
{
	for (auto it=m_resolutions.begin(); it!=m_resolutions.end(); ++it){
		cout<<it->CameraId()<<"    "<<it->Excercise()<<"    "<<it->X1()<<"    "<<it->Y1()<<"    "<<it->X2()<<"    "<<it->Y2()<<"\n";
	}
}

cv::Rect ResolutionRegister::findROI( const std::string in, int width, int height, bool lookForCamOnly/*=false*/, const std::string& inputFolder/*=""*/  )
{
	for (auto it=m_resolutions.begin(); it!=m_resolutions.end(); ++it){
		if  (in.find(it->CameraId())!=string::npos){
			if (lookForCamOnly || trialCoverExcercise(in, inputFolder, it->Excercise())){
				return cv::Rect(it->X1(), it->Y1(), width-it->X1()-it->X2(), height-it->Y1()-it->Y2());
			}
		}
	}
}

cv::Rect ResolutionRegister::findROI( const std::string in, bool lookForCamOnly/*=false*/, const std::string& inputFolder/*=""*/ )
{
	for (auto it=m_resolutions.begin(); it!=m_resolutions.end(); ++it){
		if  (in.find(it->CameraId())!=string::npos){
			if (lookForCamOnly || trialCoverExcercise(in, inputFolder, it->Excercise())){
				return cv::Rect(it->X1(), it->Y1(), it->X2(), it->Y2());
			}
		}
	}
}

bool ResolutionRegister::trialCoverExcercise( const std::string& fileName, const std::string& inputFolder, const std::string& ex )
{
	std::string e=ex;
	if (e.length()==3)
		e=e.substr(1,2);
	if (e.substr(0,1)=="0")
		e=e.substr(1,1);
	fs::path fp(fileName);
	std::string xmlName=fs::path(inputFolder).branch_path().string()+"\\index_"+fp.filename().string().substr(0, fp.filename().string().length()-17)+".xml";
	std::ifstream xml(xmlName);
	size_t pos=fileName.find_last_of("T");
	std::string trial="";
	if (pos!=std::string::npos)
		trial=fileName.substr(pos, 3);
	while (!xml.eof() && xml.good())
	{
		std::string line;
		std::getline(xml, line);
		if ( (line.find(trial)!=std::string::npos) && (line.find("<ExerciseNo>"+e+"</ExerciseNo>")!=std::string::npos) )
			return true;
	}
	return false;
}

