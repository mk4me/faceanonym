#include "folderOrganize.h"
#include "boost/filesystem.hpp"


#include <sstream>
#include <iostream>

namespace fs=boost::filesystem;

CFolderFixer::CFolderFixer( std::string folderName, std::string infoFileName/*=""*/ ): m_ready(true)
{
	fs::path folderPath(folderName);

	if (!folderPath.is_complete()) 
	{
		std::cout<<"ERROR!\nWrong folder name! ("<< folderName <<")\nStopped at is_complete\n";
		m_ready=false;
		return;
	}

	if (folderPath.has_filename()) 
	{
		if (folderPath.filename()!=".")
		{
			folderPath.remove_filename();
			std::cout<<"ERROR!\nWrong folder name! ("<< folderName <<")\nStopped at has_filename\n";
			std::cout<<"FileName=["<< folderPath.filename()<<"]\n";
			m_ready=false;
			return;
		}
	}

	

	if (folderPath.empty()) 
	{
		std::cout<<"ERROR!\nWrong folder name! ("<< folderName <<")\nStopped at empty\n";
		m_ready=false;
		return;
	}

	if (!fs::exists(folderName))
	{
		std::cout<<"ERROR!\nFolder "<< folderName <<" doesn't exists!\n";
		m_ready=false;
		return;
	}

	if (infoFileName=="")
	{
		fs::path::iterator it=folderPath.end();
		it--;
		if (*it==".")
			it--;
		//std::cout<<*it;
		/*std::stringstream fileName;
		fileName<<it->generic_string<std::string>()<<".enf";
		*/
		infoFileName=it->generic_string<std::string>();
		infoFileName.append(".enf");
		folderPath /= infoFileName;

		if (!fs::exists(folderPath))
		{
			std::cout<<"ERROR!\nFile "<< infoFileName <<" conceding as session info file doesn't exists!\nThe file should have the same name as folder with .enf extension.\nYou can also give a file name as a parameter.\n";
			m_ready=false;
			return;
		}
	}
	else
	{
		folderPath /= fs::path(infoFileName);
		if (!fs::exists(folderPath))
		{
			std::cout<<"ERROR!\nFile "<< folderName <<"given as session info file doesn't exists!\n";
			m_ready=false;
			return;
		}
	}

	m_folderName=folderName;
	m_infoFileName=infoFileName;

	std::cout<<"Current main folder structure recognized correctly.\n";
	getSessInfoFromFile();
}

void CFolderFixer::getSessInfoFromFile()
{
	std::ifstream sessInfoFile;
	std::string fileName=m_folderName;
	fileName.append(m_infoFileName);
	sessInfoFile.open(fileName);
	
	while (!sessInfoFile.eof())
	{
		std::string data;
		std::getline(sessInfoFile, data);
		//sessInfoFile>>data;
		//std::cout << data <<"\n";
		int pos;
		if (pos=data.find("CREATIONDATEANDTIME=")!=std::string::npos)
			extractDate(data, pos);

		if (pos=data.find("NAME=")!=std::string::npos)
		{
			m_bodyName=data.substr(pos+4, data.length());
		}
	}
	
	
}

void CFolderFixer::extractDate( std::string data, int pos )
{
	std::string s;
	m_date=data.substr(pos+19, 4);
	data.erase(0, pos+24);
	s=data.substr(0, data.find(","));
	data.erase(0, s.length()+1);
	if (s.length()<2)
		s="0"+s;
	s="-"+s;
	m_date+=s;

	s=data.substr(0, data.find(","));
	if (s.length()<2)
		s="0"+s;
	s="-"+s;
	m_date+=s;
}

void CFolderFixer::printSessInfo()
{
	std::cout << "\nDate        : "<< m_date <<"\n";
	std::cout << "Body name   : " << m_bodyName <<"\n";
	std::cout << "Body id     : " << m_body.Id() <<"\n";
	std::cout << "Session id  : " << m_body.SessionId() <<"\n\n";
	std::cout << "Folder      : " <<m_folderName <<"\n";
	std::cout << "File prefix : " <<m_date<<"-B"<<m_body.StringId()<<"-S"<<m_body.SessionId()<<"\n";
}

void CFolderFixer::setBodyNameAndSession()
{
	CBodyCollection::getInstance()->addBody(m_bodyName);
	CBodyCollection::getInstance()->findBody(m_bodyName, m_body);
	m_body.incSessionId();
}

std::string CFolderFixer::getFilePrefix() const
{
	return m_date+"-B"+m_body.StringId()+"-S"+m_body.SessionId();
}

bool CFolderFixer::fixFolder()
{
	

	if (!m_ready)
		return false;

	fs::path folderPath(m_folderName);

	//fs::recursive_directory_iterator fit(folderPath); //folder iterator
	fs::recursive_directory_iterator itrEnd;
	int i=0;
	std::string commandFull;
	for (fs::recursive_directory_iterator fit(folderPath);
		fit != itrEnd;
		++fit)
	{
		//std::cout<<fit->path().generic_string<std::string>()<<"\nstem="<<fit->path().stem().generic_string<std::string>() << "\nextention="<< fit->path().extension().generic_string<std::string>() <<"\n";
		std::string fileSufix=findFileSufix(fit->path().stem().generic_string<std::string>());
		if ( (fileSufix!="") && (fileSufix[0]!='.') )
			fileSufix="-"+fileSufix;
		fs::path newPath=m_folderName+getFilePrefix()+fileSufix+fit->path().extension().generic_string<std::string>();
		//std::cout<<"rename file: "<<newPath.generic_string<std::string>()<<"\n";
		fs::rename(fit->path(), newPath);
		std::string ext=newPath.extension().generic_string<std::string>();
		if ((ext.find("amc")==std::string::npos) &&
			(ext.find("asf")==std::string::npos) &&
			(ext.find("avi")==std::string::npos) &&
			(ext.find("c3d")==std::string::npos) &&
			(ext.find("enf")==std::string::npos))
		{
			std::cout<<newPath.string()<<"\n[7zip output:\n";
			std::string cmd="7z.exe a "+m_folderName+getFilePrefix()+".zip "+newPath.string()+"\n";
			commandFull=commandFull+" && "+cmd;
			for (;std::system(NULL);)
				std::cout<<"-";
			std::cout<<std::system(cmd.c_str());
			std::cout<<"\nend of 7zip output];\n";
			i++;
			fs::remove(newPath);
		}


	}

	CBody *bodyToChange=CBodyCollection::getInstance()->getBody(m_body.Name());
	if (bodyToChange!=NULL)
		bodyToChange->incSessionId();

	std::cout<<i<<"File(s) zipped\n"<<"\n";
	return true;
}

std::string CFolderFixer::findFileSufix( std::string fileStem )
{
	bool first=true;
	int posT=fileStem.length();
	while(posT!=std::string::npos)
	{
		posT=fileStem.find_last_of("T", posT-1);
	
		if ((posT==std::string::npos) && first )//if this is a first passage of loop and "T" is not found it means that there is no T in file stem so there is no sufix.
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
			if (!nonDigitSignFound)
				return fileStem.substr(posT, fileStem.length());
		}
		else
			return fileStem.substr(posEnd, fileStem.length()); //if "T" not found but "." found we should return everything beginning from dot.
	}
	
	
	return "";
}

void CFolderFixer::BodyName( const std::string& val )
{
	 m_bodyName = val;
	 m_body.clear(val);
}

std::string CBody::SessionId() const
{
	return asString(m_lastSessionId, m_sessIdLength);
}

CBody::CBody( std::string name, int id,int bodyIdLength/*=4*/, int sessionIdLegth/*=2*/ ):m_sessIdLength(sessionIdLegth),
	m_idLength(bodyIdLength),
	m_name(name),
	m_id(id),
	m_lastSessionId(0)
{};

std::string CBody::StringId() const
{
	return asString(m_id, m_idLength);
}

std::string CBody::asString( int value, int length ) const
{
	std::stringstream retValStream;
	retValStream<<value;
	std::string retVal=retValStream.str();
	for (int i=retVal.length(); i<length; i++)
		retVal="0"+retVal;

	return retVal;
}

std::string CBody::toFileNameUsableForm() const
{
	return "B"+StringId()+"-S"+SessionId();
}

void CBody::saveToFile( std::ofstream& fileStream )
{
	fileStream << m_name << "\n"; 
	fileStream << m_id << "\n";
	fileStream << m_lastSessionId << "\n";
}

void CBody::loadFromFile( std::ifstream& fileStream )
{
	std::getline(fileStream, m_name);
	fileStream>>m_id;
	fileStream>> m_lastSessionId;
}

void CBody::clear( std::string name )
{
	m_name=name;
	m_id=0;
	m_idLength=4;
	m_lastSessionId=0;
	m_sessIdLength=2;
}

BodyErrors CBodyCollection::addBody( const std::string& bodyName )
{
	for (int i=0; i<m_bodyVector.size(); i++ )
		if (m_bodyVector[i].Name()==bodyName)
			return BER_BODY_EXISTS;

	CBody newBody(bodyName, maxId());

	m_bodyVector.push_back(newBody);

	return BER_NO;
}

int CBodyCollection::maxId()
{
	int max=0;
	for (int i=0; i<m_bodyVector.size(); i++ )
		if (m_bodyVector[i].Id()>=max)
			max=m_bodyVector[i].Id();

	return max+1;
}

bool CBodyCollection::findBody( const std::string& bodyName, CBody& body ) const
{
	for (int i=0; i<m_bodyVector.size(); i++)
	{
		if (m_bodyVector[i].Name()==bodyName)
		{
			body=m_bodyVector[i];
			return true;
		}
	}
	return false;
}

CBodyCollection* CBodyCollection::getInstance()
{
	if (m_instance==0)
		m_instance=new CBodyCollection();

	return m_instance;
}

void CBodyCollection::saveToFile( const std::string& filePath )
{
	std::ofstream file(filePath);
	for (int i=0; i<m_bodyVector.size(); i++)
		m_bodyVector[i].saveToFile(file);
	file.close();
}

void CBodyCollection::loadFromFile( const std::string& filePath )
{
	std::ifstream file(filePath);
	while (file.good())
	{
		CBody newBody;
		newBody.loadFromFile(file);
		m_bodyVector.push_back(newBody);
	}
	file.close();
}

CBody* CBodyCollection::getBody( const std::string name )
{
	for (int i=0; i<m_bodyVector.size(); i++)
	{
		if (m_bodyVector[i].Name()==name)
			return &(m_bodyVector[i]);
	}
	return NULL;
}

CBodyCollection* CBodyCollection::m_instance=0;

