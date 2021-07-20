//#include <iostream>
#include "lexicalanalysis.h"
#include <cmath>

//using namespace std;

vector <string> K = { "int","void","while","if","else","return","float" };					//关键字表
vector <string> P = { "-","/","(",")","==","<=","<","+","*",">","=",",",";","++",
                    "{","}","\'","\"",">=","--","&&","||","<<",">>","[","]","!","!=" };		//界符表
vector <char> Pdouble = { '=','<','+','>','-','&','|','!' };								//多字符界符开头符号表

vector <string> I;				//标识符表
vector <float> C;				//常数表

vector <string> Token;			//Token序列

int WordAnalysis::count = 0;

int WordAnalysis::delBlank(string s, int i)
{
    int k = 0;
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i++;
        k++;
    }
    return k;
}

bool WordAnalysis::clear()
{
    I.clear();
    C.clear();
    Token.clear();
    WordAnalysis::count = 0;
    return true;
}

bool WordAnalysis::scan(string s, ReToken& To,MainWindow* w)
{
    int i = count;
    if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i += delBlank(s, i);
        if (s[i] == '#')
        {
            To.c = 'O';
            To.number = -1;
            To.s = "#";
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || s[i] == '_')
        {
            i += isI(s, i, To);
        }
        else if (s[i] <= '9' && s[i] >= '0')
        {
            int temp_int=isC(s, i, To, w);
            if(temp_int==-1) return false;
            else i += isC(s, i, To, w);
        }
        else i += isP(s, i, To);
    }
    else
    {
        if (s[i] == '#')
        {
            To.c = 'O';
            To.number = -1;
            To.s = "#";
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || s[i] == '_')
        {
            i += isI(s, i, To);
        }
        else if (s[i] <= '9' && s[i] >= '0')
        {
            int temp_int=isC(s, i, To, w);
            if(temp_int==-1) return false;
            else i += isC(s, i, To, w);
        }
        else i += isP(s, i, To);
    }
    count = i;
    return true;
}

template<typename T> int WordAnalysis::existT(T t, vector<T> v)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i]) return i + 1;
    }
    return 0;
}

int WordAnalysis::isI(string s, int i, ReToken& To)		//标识符判断
{
    int start = i;
    while ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || (s[i] == '_') || (s[i] <= '9' && s[i] >= '0')) i++;
    string str = s.substr(start, i - start);
    if (existT<string>(str, K) != 0)
    {
        string temp = "(K " + to_string(existT<string>(str, K)) + ')';
        Token.push_back(temp);
        To.c = 'K';
        To.number = existT<string>(str, K);
        To.s = str;
    }
    else
    {
        if (existT<string>(str, I) != 0)
        {
            string temp = "(I " + to_string(existT<string>(str, I)) + ')';
            Token.push_back(temp);
            To.c = 'I';
            To.number = existT<string>(str, I);
            To.s = str;
        }
        else
        {
            I.push_back(str);
            string temp = "(I " + to_string(existT<string>(str, I)) + ')';
            Token.push_back(temp);
            To.c = 'I';
            To.number = existT<string>(str, I);
            To.s = str;
        }
    }
    return i - start;
}

int WordAnalysis::isC(string s, int i, ReToken& To, MainWindow* w)
{
    int start = i;
    int flag = 0;
    char ope;
    int location;
    int n_ope = 1;
    while ((s[i] <= '9' && s[i] >= '0') || (s[i] == '.') || s[i] == 'e')
    {
        if (s[i] == '.') flag = 1;
        if (s[i] == '.' && (s[i + 1] >= '9' || s[i + 1] <= '0'))
        {
            w->outInfor("词法分析错误！");
            return -1;
            //cout << "错误";
            //exit(-1);
        }
        if (s[i] == 'e')
        {
            location = i;
            if (s[i + 1] == '+' || s[i + 1] == '-') { ope = s[i + 1]; i += 2; flag = 2; continue; }
            else if (s[i + 1] <= '9' && s[i + 1] >= '0') { ope = '+'; flag = 2; n_ope = 0; }
            else
            {
                w->outInfor("词法分析错误！");
                return -1;
                //cout << "错误";
                //exit(-1);
            }
        }
        i++;
    }
    string temp = s.substr(start, i - start);
    if (flag == 0)
    {
        float check = (float)atoi(temp.c_str());
        if (existT<float>(check, C) == 0)
        {
            C.push_back(check);
            string key = "(C " + to_string(existT<float>(check, C)) + ')';
            Token.push_back(key);
            To.c = 'C';
            To.number = existT<float>(check, C);
            To.s = temp;
        }
        else
        {
            string key = "(C " + to_string(existT<float>(check, C)) + ')';
            Token.push_back(key);
            To.c = 'C';
            To.number = existT<float>(check, C);
            To.s = temp;
        }
    }
    else if (flag == 1 || flag == 2)
    {
        float check = 0;
        if (flag == 1) check = (float)atof(temp.c_str());
        if (flag == 2) check = (float)expop(start, i, location, ope, s, n_ope);
        if (existT<float>(check, C) == 0)
        {
            C.push_back(check);
            string key = "(C " + to_string(existT<float>(check, C)) + ')';
            Token.push_back(key);
            To.c = 'C';
            To.number = existT<float>(check, C);
            To.s = temp;
        }
        else
        {
            string key = "(C " + to_string(existT<float>(check, C)) + ')';
            Token.push_back(key);
            To.c = 'C';
            To.number = existT<float>(check, C);
            To.s = temp;
        }
    }
    return i - start;
}
float WordAnalysis::expop(int start, int end, int loc, char w, string s, int n_ope)
{
    int front = 0, behind = 0;
    int basenumber = 0;
    for (int j = start; ; j++)
    {
        if (s[j] == '.')
        {
            front = j - start;
            behind = loc - j - 1;
            string temp = s.substr(start, loc - start);
            basenumber = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
        if (s[j] == 'e')
        {
            string temp = s.substr(start, loc - start);
            basenumber = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
    }
    string temp = s.substr(loc + 1 + n_ope, end - loc - 1 - n_ope);
    int times = 0;
    if (w == '+') times = atoi(temp.c_str()) - behind;
    if (w == '-') times = atoi(temp.c_str()) * (-1) - behind;
    return (float)(basenumber * pow(10, times));
}

int WordAnalysis::isP(string s, int i, ReToken& To)
{
    int re = 0;
    if (existT<char>(s[i], Pdouble) == 0)
    {
        string str = s.substr(i, 1);
        string key = "(P " + to_string(existT<string>(str, P)) + ')';
        Token.push_back(key);
        To.c = 'P';
        To.number = existT<string>(str, P);
        To.s = str;
        re = 1;
    }
    else
    {
        string st = s.substr(i, 2);
        if (existT<string>(st, P) == 0)
        {
            string str = s.substr(i, 1);
            string key = "(P " + to_string(existT<string>(str, P)) + ')';
            Token.push_back(key);
            To.c = 'P';
            To.number = existT<string>(str, P);
            To.s = str;
            re = 1;
        }
        else
        {
            string key = "(P " + to_string(existT<string>(st, P)) + ')';
            Token.push_back(key);
            To.c = 'P';
            To.number = existT<string>(st, P);
            To.s = st;
            re = 2;
        }
    }
    return re;
}

template<typename T> void WordAnalysis::print(vector<T> v, MainWindow* w)
{
    for (int i = 0; i < v.size(); i++)
    {
        w->outInfor(QString::fromStdString(v[i]));
        //cout << v[i];
        if (i != v.size() - 1 && count != 1)
            w->outInfor(" ");
            //cout << " ";
    }
    count++;
}
