#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <map>
#include <vector>
#include <string>
#include <string.h>

using namespace std;

namespace Config
{
struct Item {
    string key;
    string value;
    string comment;
};

struct Section {
    string name;
    string comment;
    vector<Item> items;
};

class IniFile
{
public:
    IniFile();
    ~IniFile();

public:
    int load(const string &fname);
    int save(const string &bfname = "");

	int setValue(const string &section, const string &key, const string &value, const string &comment = "");
    int getValue(const string &section, const string &key, string &value, string &comment);

private:
    Section *getSection(const string &section = "");
    void release();
    int getline(string &str, FILE *fp);
    bool isComment(const string &str);
    bool parse(const string &content, string &key, string &value, char c = '=');

    //for dubug
    void print();

private:
    string 					m_fname;
    vector<string> 			m_flags;
	map<string, Section *> 	m_sections;
};
}

#endif
