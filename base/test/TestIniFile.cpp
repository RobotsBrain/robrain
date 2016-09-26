#include <string.h>
#include <stdio.h>
#include <string>

#include "IniFile.h"


void Save(Base::CIniFile *pIni)
{
	std::string saveas_path = "test_save.ini";

	pIni->Save();
	pIni->Save(saveas_path.c_str());

	return;
}

void SetValue(Base::CIniFile *pIni)
{
	std::string saveas_path = "test_set_value.ini";

	pIni->SetValue("COMMON", "DB", "sys", "数据库");
    pIni->SetValue("COMMON", "PASSWD", "root", "数据库密码");
    pIni->SetValue("", "NAME", "cxy", "");
    pIni->SetValue("", "USER", "wm", "");

    pIni->Save(saveas_path.c_str());

	return;
}

void GetValue(Base::CIniFile *pIni)
{
	std::string value;

	if(pIni->GetValue("", "USER", value) == 0) {
		if(value == "root") {
			printf("get user success!\n");
		}
	}

    if(pIni->GetValue("COMMON", "DB", value) == 0) {
    	if(value == "sys") {
    		printf("get common db success!\n");
    	}
    }

    if(pIni->GetValue("COMMON", "PASSWD", value) == 0) {
    	if(value == "tt") {
    		printf("get common passwd success!\n");
    	}
    }

   	if(pIni->GetValue("DEFINE", "name", value) == 0) {
   		if(value == "cxy") {
   			printf("get define name success!\n");
   		}
   	}

    if(pIni->GetValue("DEFINE", "value", value) == 0) {
    	if(value == "1") {
    		printf("get define value success!\n");
    	}
    }
    
    if(pIni->GetValue("DEFINE", "key", value) < 0) {
    	printf("test define no key success!\n");
    }

    if(pIni->GetValue("NO", "value", value) < 0) {
    	printf("test no section success!\n");
    }

    return;
}

void InitIniFile(const char *filename)
{
	FILE *fp = fopen(filename, "w");
    std::string content = "USER=root \r\n [COMMON] \n DB=sys   	\nPASSWD=tt   \n#commit   \n ;--------- \n[DEFINE] \nname=cxy\nvalue=1 #测试";
    
    fwrite(content.c_str(), sizeof(char), content.size(), fp);
    fclose(fp);

	return;
}

int main(int argc, char **argv)
{
	Base::CIniFile ini;
	std::string filepath = "test.ini";
    
    InitIniFile(filepath.c_str());

    ini.Load(filepath.c_str());

    // Save(&ini);

    // SetValue(&ini);

    GetValue(&ini);

	return 0;
}


