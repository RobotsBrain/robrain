#include <assert.h>
#include <string>

#include "IniFile.h"
#include "IniFilePimpl.h"



namespace Base {


CIniFile::CIniFile()
: m_pImpl(new IniFilePimpl())
{

}

CIniFile::~CIniFile()
{
    delete m_pImpl;
    m_pImpl = NULL;
}

int CIniFile::Load(const string &fname)
{
    assert(m_pImpl != NULL);

    return m_pImpl->load(fname);
}

int CIniFile::Save(const string &bfname)
{
    assert(m_pImpl != NULL);

    return m_pImpl->save(bfname);
}

int CIniFile::SetValue(const string &section, const string &key, const string &value, const string &comment)
{
    assert(m_pImpl != NULL);

    return m_pImpl->setValue(section, key, value, comment);
}

int CIniFile::GetValue(const string &section, const string &key, string &value)
{
    assert(m_pImpl != NULL);

    return m_pImpl->getValue(section, key, value);
}


} // end namespace Base


