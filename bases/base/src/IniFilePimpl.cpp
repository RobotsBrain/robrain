#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <map>
#include <vector>
#include <string>

#include "IniFilePimpl.h"


namespace Base {


int INI_BUF_SIZE = 2048;
const string delim = "\n";

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


IniFilePimpl::IniFilePimpl()
{
    m_flags.push_back("#");
    m_flags.push_back(";");
}

IniFilePimpl::~IniFilePimpl()
{
    release();
}

int IniFilePimpl::load(const string &filename)
{
    Section *section = NULL;
    FILE *fp = fopen(filename.c_str(), "r");

    release();
    m_fname = filename;

    if (fp == NULL) {
        return -1;
    }

    string line;
    string comment;

    //增加默认段
    section = new Section();
    m_sections[""] = section;

    while (getline(line, fp) > 0) {

        trimright(line, '\n');
        trimright(line, '\r');
        trim(line);

        if (!isComment(line)) {
            /* 针对 “value=1 #测试” 这种后面有注释的语句, 需要重新分割line，并添加注释到commnet
             * 这种情况保存后会变成如下形式：
             * #测试
             * value=1
             * */
            string subline;
            string tmp = line;

            for (size_t i = 0; i < m_flags.size(); ++i) {
                subline = line.substr(0, line.find(m_flags[i]));
                line = subline;
            }

            comment += tmp.substr(line.length());
        }

        trim(line);

        if (line.length() <= 0) {
            continue;
        }

        if (line[0] == '[') {
            section = NULL;
            int index = line.find_first_of(']');

            if (index == -1) {
                fclose(fp);
                fprintf(stderr, "not find]\n");
                return -1;
            }

            int len = index - 1;

            if (len <= 0) {
                fprintf(stderr, "is null\n");
                continue;
            }

            string s(line, 1, len);

            if (getSection(s.c_str()) != NULL) {
                fclose(fp);
                fprintf(stderr, "have existed: %s\n", s.c_str());
                return -1;
            }

            section = new Section();
            m_sections[s] = section;

            section->name = s;
            section->comment = comment;
            comment = "";
        } else if (isComment(line)) {
            if (comment != "") {
                comment += delim + line ;
            } else {
                comment = line;
            }
        } else {
            string key, value;

            if (parse(line, key, value)) {
                Item item;
                item.key = key;
                item.value = value;
                item.comment = comment;

                section->items.push_back(item);
            } else {
                fprintf(stderr, "parse fail [%s]\n", line.c_str());
            }

            comment = "";
        }
    }

    fclose(fp);

    return 0;
}

int IniFilePimpl::save(const string &bfname)
{
    string data = "";

    for (map<string, Section *>::iterator sect = m_sections.begin();
            sect != m_sections.end(); ) {

        if (sect->second->comment != "") {
            data += sect->second->comment;
            data += delim;
        }

        if (sect->first != "") {
            data += string("[") + sect->first + string("]");
            data += delim;
        }

        for (vector<Item>::iterator item = sect->second->items.begin();
                item != sect->second->items.end(); ++item) {
            if (item->comment != "") {
                data += item->comment;
                data += delim;
            }

            data += item->key + "=" + item->value;
            data += delim;
        }

        ++sect;

        if(sect != m_sections.end()) {
            data += delim;
        }
    }

    string savefile = bfname;

    if(savefile.empty()) {
        savefile = m_fname;
    }

    FILE *fp = fopen(savefile.c_str(), "w");
    if(fp == NULL) {
        printf("open file fail!\n");
        return -1;
    }

    fwrite(data.c_str(), 1, data.length(), fp);

    fclose(fp);

    return 0;
}

int IniFilePimpl::getValue(const string &section, const string &key, string &value)
{
    Section *sect = getSection(section);

    if (sect == NULL) {
        return -1;
    }

    for (vector<Item>::iterator it = sect->items.begin(); it != sect->items.end(); ++it) {
        if (it->key == key) {
            value = it->value;
            // comment = it->comment;
            return 0;
        }
    }

    return -1;
}

int IniFilePimpl::setValue(const string &section, const string &key,
                           const string &value, const string &comment /*=""*/)
{
    Section *sect = getSection(section);
    string comt = comment;

    if (comt != "") {
        comt = m_flags[0] + comt;
    }

    if (sect == NULL) {
        sect = new Section();

        if (sect == NULL) {
            fprintf(stderr, "no enough memory!\n");
            exit(-1);
        }

        sect->name = section;
        m_sections[section] = sect;
    }

    for (vector<Item>::iterator it = sect->items.begin(); it != sect->items.end(); ++it) {
        if (it->key == key) {
            it->value = value;
            it->comment = comt;
            return 0;
        }
    }

    //not found key
    Item item;
    item.key = key;
    item.value = value;
    item.comment = comt;

    sect->items.push_back(item);

    return 0;
}

bool IniFilePimpl::parse(const string &content, string &key, string &value, char c/*= '='*/)
{
    int i = 0;
    int len = content.length();

    while (i < len && content[i] != c) {
        ++i;
    }

    if (i >= 0 && i < len) {
        key = string(content.c_str(), i);
        value = string(content.c_str() + i + 1, len - i - 1);
        return true;
    }

    return false;
}

int IniFilePimpl::getline(string &str, FILE *fp)
{
    int plen = 0;
    int buf_size = INI_BUF_SIZE * sizeof(char);

    char *buf = (char *) malloc(buf_size);
    char *pbuf = NULL;
    char *p = buf;

    if (buf == NULL) {
        fprintf(stderr, "no enough memory!exit!\n");
        exit(-1);
    }

    memset(buf, 0, buf_size);
    int total_size = buf_size;

    while (fgets(p, buf_size, fp) != NULL) {
        plen = strlen(p);

        if (plen > 0 && p[plen - 1] != '\n' && !feof(fp)) {

            total_size = strlen(buf) + buf_size;
            pbuf = (char *)realloc(buf, total_size);

            if (pbuf == NULL) {
                free(buf);
                fprintf(stderr, "no enough memory!exit!\n");
                exit(-1);
            }

            buf = pbuf;
            p = buf + strlen(buf);

            continue;
        } else {
            break;
        }
    }

    str = buf;

    free(buf);
    buf = NULL;

    return str.length();
}

Section *IniFilePimpl::getSection(const string &section /*=""*/)
{
    map<string, Section *>::iterator it = m_sections.find(section);

    if (it != m_sections.end()) {
        return it->second;
    }

    return NULL;
}

void IniFilePimpl::release()
{
    m_fname = "";

    for (map<string, Section *>::iterator i = m_sections.begin();
            i != m_sections.end(); ++i) {
        delete i->second;
    }

    m_sections.clear();

    return;
}

bool IniFilePimpl::isComment(const string &str)
{
    bool ret = false;

    for (size_t i = 0; i < m_flags.size(); ++i) {
        size_t k = 0;

        if (str.length() < m_flags[i].length()) {
            continue;
        }

        for (k = 0; k < m_flags[i].length(); ++k) {
            if (str[k] != m_flags[i][k]) {
                break;
            }
        }

        if (k == m_flags[i].length()) {
            ret = true;
            break;
        }
    }

    return ret;
}


}   // end namespace Base



