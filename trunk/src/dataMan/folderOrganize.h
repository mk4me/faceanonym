#ifndef folderOrganize_h__
#define folderOrganize_h__

#include <fstream>
#include <string>
#include <vector>


class CBody
{
public:

	CBody(std::string name, int id,int bodyIdLength=4, int sessionIdLegth=2);

	void clear(std::string name);

	std::string Name() const { return m_name; }
	void Name(std::string val) { m_name = val; }

	int Id() const { return m_id; }
	void Id(int val) { m_id = val; }
	
	std::string StringId() const;

	std::string toFileNameUsableForm() const; 

	std::string SessionId () const;
	void incSessionId () {m_lastSessionId+=1;};

	CBody():m_id(0),
		m_lastSessionId(0),
		m_idLength(4),
		m_sessIdLength(2)
	{};

	void saveToFile(std::ofstream& fileStream);
	void loadFromFile(std::ifstream& fileStream);

private:

	std::string asString(int value, int length) const;

	std::string m_name;
	
	int m_id;
	
	int m_lastSessionId;
	
	int m_idLength;

	int m_sessIdLength;
};

enum BodyErrors
{
	BER_NO,
	BER_BODY_EXISTS,
	BER_OTHER
};

class CBodyCollection //Singleton
{
public:
	BodyErrors addBody(const std::string& bodyName);
	bool findBody (const std::string& bodyName, CBody& body) const;
	static CBodyCollection* getInstance();

	CBody* getBody (const int index) {return &(m_bodyVector[index]);};
	CBody* getBody (const std::string name);
	void saveToFile (const std::string& filePath);
	void loadFromFile (const std::string& filePath);
private:
	CBodyCollection(){};
	CBodyCollection(CBodyCollection& cpy);
	CBodyCollection& operator = (CBodyCollection& rhs);

	static CBodyCollection *m_instance;

	int maxId();

	std::vector <CBody> m_bodyVector;

};

class CFolderFixer
{
public:
	CFolderFixer (std::string folderName, std::string infoFileName="");

	std::string getSessionPrefix();

	void read(std::string folderName);
	bool isReady() const {return m_ready;};
	void printSessInfo();
	void setBodyNameAndSession();
	std::string getFilePrefix() const;
	bool fixFolder();
	void BodyName(const std::string& val);

private:
	CFolderFixer(); //we cannot create Session info object without folder containing data of this session.

	void getSessInfoFromFile();
	void extractDate(std::string data, int pos);

	std::string findFileSufix(std::string fileStem);

	std::string m_bodyId;
	std::string m_bodyName;
	
	std::string m_date;
	std::string m_folderName;
	std::string m_infoFileName;
	CBody m_body;
	bool m_ready;
};

#endif // folderOrganize_h__
