#ifndef _cSceneManager_HG_
#define _cSceneManager_HG_
#include <string>

class cSceneManager
{
public:

	cSceneManager();

	bool saveScene(std::string filename);
	bool loadScene(std::string filename);
	//bool loadScene(std::string filename);
	void setBasePath(std::string basepath);


private:
	std::string m_basePath;

};


















#endif