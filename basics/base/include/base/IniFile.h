#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <string>



namespace Base {

class IniFilePimpl;

class CIniFile
{
public:
    CIniFile();
    ~CIniFile();
    
    int Load(const std::string &fname);
    int Save(const std::string &bfname = "");

    int SetValue(const std::string &section, const std::string &key,
    			 const std::string &value, const std::string &comment = "");
    int GetValue(const std::string &section, const std::string &key, std::string &value);

private:
    IniFilePimpl   *m_pImpl;
};


} // end namespace Base

#endif
