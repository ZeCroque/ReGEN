#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <pugixml.hpp>

#include "Singleton.h"

class DataManager : public Singleton<DataManager>
{
	friend class Singleton<DataManager>;

public:
	void init(const char* inTestLayoutName, const char* inContentPath = "./Data/");

private:
	void loadRules(const std::string& inContentPath, bool inLoadRewriteRules = false) const;
	
	pugi::xml_document worldGraph;
	pugi::xml_document testLayout;
	pugi::xml_document initializationRules;
	pugi::xml_document rewriteRules;
};

#endif // DATAMANAGER_H
