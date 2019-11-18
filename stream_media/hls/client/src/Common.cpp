#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>



namespace Hls {


bool startWith(const std::string& str, const std::string& subStr)
{
    if(strncmp(str.c_str(), subStr.c_str(), subStr.length()) == 0) {
        return true;
    }

    return false;
}

std::string getLine(const char* data, int *offsetCur, int size)
{
    int offset = *offsetCur;
    int offsetLF = offset;
    std::string line = "";

    if(offset < size) {
        while(offsetLF < size && data[offsetLF] != '\n') {
            offsetLF++;
        }

        if(offset < offsetLF && data[offsetLF - 1] == '\r') {
            line.assign(&data[offset], offsetLF - offset - 1);
        } else {
            line.assign(&data[offset], offsetLF - offset);
        }
    }

    *offsetCur = offsetLF + 1;

    return line;
}

std::map<std::string, std::string>* parseAttr(std::string line)
{
    std::map<std::string, std::string> *attrMap = new std::map<std::string, std::string>();
    char attr[1024];

    memset(attr, 0, sizeof(attr));
    strncpy(attr, line.c_str(), sizeof(attr) - 1);

    char *p = strtok(attr, ":");
    while(p) {
        p = strtok(NULL, ",");
        if(p) {
            printf("attr is %s\n", p);
            p = strtok(NULL, "=");
        }
    }

    return attrMap;
}

bool parseDouble(const char* s, double *x)
{
    char *end = NULL;
    double val = strtod(s, &end);

    if(end == s || (*end != '\0' && *end != ',')) {
        printf("parse double error \n");
        return false;
    }

    *x = val;

    return true;
}

bool parseInt32(const char* s, int *x)
{
    char *end = NULL;
    int val = strtol(s, &end, 10);

    if(end == s || (*end != '\0' && *end != ',')) {
        printf("parse double error \n");
        return false;
    }

    *x = val;
    
    return true;
}

} // end namespace