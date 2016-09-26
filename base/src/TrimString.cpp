#include <string>

using namespace std;



void trimleft(string &str, char c/*=' '*/)
{
    //trim head
	int i = 0;
    int len = str.length();

    while (str[i] == c && str[i] != '\0') {
        i++;
    }

    if (i != 0) {
        str = string(str, i, len - i);
    }

	return;
}

void trimright(string &str, char c/*=' '*/)
{
    //trim tail
    int i = 0;
    int len = str.length();

    for (i = len - 1; i >= 0; --i) {
        if (str[i] != c) {
            break;
        }
    }

    str = string(str, 0, i + 1);

	return;
}

void trim(string &str)
{
    //trim head
	int i = 0;
    int len = str.length();
    
    while (isspace(str[i]) && str[i] != '\0') {
        i++;
    }

    if (i != 0) {
        str = string(str, i, len - i);
    }

    //trim tail
    len = str.length();

    for (i = len - 1; i >= 0; --i) {
        if (!isspace(str[i])) {
            break;
        }
    }

    str = string(str, 0, i + 1);

	return;
}



