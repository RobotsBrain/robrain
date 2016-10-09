#include <string>



namespace NetWork {

int HostByName(std::string name);

int AdnsAnalysis(std::string host, int looptime = 1);

} // end namespace