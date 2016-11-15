#include <string>
#include <map>



using namespace std;

namespace Base {

class CComponentManager
{
public:
	static CComponentManager *Instance();

	void RegisterComponent(void *pComponent, string name);
	void UnRegisterComponent(string name);

	void *FindComponent(string name);

private:
	CComponentManager();
	~CComponentManager();
	
private:
	typedef map<string, void *> ComponentMap;

	ComponentMap m_component_map;
};

CComponentManager *CComponentManager::Instance()
{
	CComponentManager ins;

	return &ins;
}

CComponentManager::CComponentManager()
{
}

CComponentManager::~CComponentManager()
{
}

void CComponentManager::RegisterComponent(void *pComponent, string name)
{
	ComponentMap::iterator it = m_component_map.find(name);
    if (it == m_component_map.end()) {
        m_component_map[name] = pComponent;
        return;
    }

	return;
}

void CComponentManager::UnRegisterComponent(string name)
{
	ComponentMap::iterator it = m_component_map.find(name);
    if (it != m_component_map.end()) {
        m_component_map.erase(it);
        return;
    }

	return;
}

void *CComponentManager::FindComponent(string name)
{
	ComponentMap::iterator it = m_component_map.find(name);
    if (it != m_component_map.end()) {
        return it->second;
    }

    return NULL;
}

void RegisterComponent(void *pComponent, string name)
{
	CComponentManager::Instance()->RegisterComponent(pComponent, name);
}

void UnRegisterComponent(string name)
{
	CComponentManager::Instance()->UnRegisterComponent(name);
}

void *FindComponent(string name)
{
	return CComponentManager::Instance()->FindComponent(name);
}

} // end namespace


