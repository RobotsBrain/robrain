#include <string.h>
#include <stdio.h>
#include <string>

#include "IniFile.h"



int main(int argc, char **argv)
{
	Base::CIniFile ini;
	std::string filepath = "test.ini";
	std::string saveas_path = "test_save.ini";
    FILE *fp = fopen(filepath.c_str(), "w");
    std::string content = "USER=root \r\n [COMMON] \n DB=sys   	\nPASSWD=tt   \n#commit   \n ;--------- \n[DEFINE] \nname=cxy\nvalue=1 #测试";
    
    fwrite(content.c_str(), sizeof(char), content.size(), fp);
    fclose(fp);

    ini.Load(filepath.c_str());

    ini.Save(saveas_path.c_str());

	return 0;
}


