#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <string.h>
#include <map>
#include <stack>
#include "targetcode.h"
using namespace std;

stack <int> SEM;	//栈

extern vector<string> I;

vector <string> Quadruple;		//待处理的四元式序列
vector <string> Object;			//翻译语句
map <string, char> Var;			//变量
map <string, vector<string>> Operate = { {"+",{"w","ADD","y"}},{"-",{"w","SUB","n"}},{"*",{"w","MUL","y"}},
                                        {"/",{"w","DIV","n"}},{">",{"w","GT","n"}},{"<",{"w","LT","n"}},
                                        {"==",{"w","EQ","y"}},{"!=",{"w","NE","y"}},
                                        {">=",{"w","GE","n"}} ,{"<=",{"w","LE","n"}} };

int TargetCode::count = 1;
string RDL::R = "0";
string RDL::active = "0";

void TargetCode::clear()
{
    while(!SEM.empty()) SEM.pop();
    Quadruple.clear();
    Object.clear();
    Var.clear();
    TargetCode::count = 1;
    RDL::R = "0";
    RDL::active = "0";
}

template<typename T>bool TargetCode::existT(vector<T> v, T t)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i]) return true;
    }
    return false;
}

void TargetCode::getVar()
{
    string temp;
    for (vector<string>::iterator it = Quadruple.begin(); it != Quadruple.end(); it++)
    {
        temp = choose(*it, 1, 0);
        if (existT<string>(I, temp) && Var.find(temp) == Var.end())
            Var.insert(make_pair(temp, 'y'));
        temp = choose(*it, 2, 0);
        if (existT<string>(I, temp) && Var.find(temp) == Var.end())
            Var.insert(make_pair(temp, 'y'));
        temp = choose(*it, 3, 0);
        if (existT<string>(I, temp) && Var.find(temp) == Var.end())
            Var.insert(make_pair(temp, 'y'));
        if (!existT<string>(I, temp) && temp[0] == 't' && Var.find(temp) == Var.end())
            Var.insert(make_pair(temp, 'n'));
    }
}

//逆序对四元式操作
void TargetCode::activeinf()
{
    for (auto it = Quadruple.rbegin(); it != Quadruple.rend(); it++)
    {
        string temp = *it;
        int comma = 0;
        int j, k;
        for (j = (int)temp.size(); temp[j] != '('; j--)
        {
            if (temp[j] == ',')
            {
                comma++;
                for (k = j + 1; temp[k] != ',' && temp[k] != ')'; k++) {}
                string s = temp.substr(j + 1, k - j - 1);
                if (Var.find(s) != Var.end())
                {
                    char symbol = Var[s];
                    string s_temp;
                    stringstream sstream;
                    sstream << symbol;
                    sstream >> s_temp;
                    string sert = '(' + s_temp + ')';
                    temp = temp.substr(0, k) + sert + temp.substr(k, (int)temp.size() - k);
                    if (comma == 1) Var[s] = 'n';
                    else if (comma == 2 || comma == 3) Var[s] = 'y';
                }
            }

        }
        *it = temp;
    }
}


string TargetCode::choose(string s, int comma, int lag)
{
    if (comma > 3 || comma < 0) { cout << "错误！"; exit(-1); }
    int cm = 0;
    for (int i = 0; i < (int)s.size(); i++)
    {
        if (s[i] == ',') cm++;
        if (comma == cm)
        {
            int j;
            if (lag == 0)
            {
                for (j = i + 1; s[j] != '(' && s[j] != ',' && s[j] != ')'; j++) {}
                string temp = s.substr(i + 1, j - i - 1);
                return temp;
            }
            else if (lag == 1)
            {
                for (j = i + 1; s[j] != '(' && s[j] != ',' && s[j] != ')'; j++) {}
                string temp = s.substr(j + 1, 1);
                return temp;
            }
            else { cout << "错误！"; exit(-1); }
        }
    }
    cout << "错误！";
    exit(-1);
}

void TargetCode::fillObj()
{
    for (vector<string>::iterator it = Quadruple.begin(); it != Quadruple.end(); it++)
    {
        if (choose(*it, 0, 0) == "=")
        {
            if (RDL::R == "0")
            {
                string s;
                stringstream stream;
                stream << count-1;
                stream >> s;
                count++;
                s = "lab" + s + " LD R," + choose(*it, 1, 0);
                Object.push_back(s);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " ST R," + choose(*it, 1, 0);
                    Object.push_back(s);
                }
            }
            else if (RDL::R != choose(*it, 1, 0) && RDL::R != choose(*it, 3, 0))
            {
                if (RDL::active == "y")
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " ST R," + RDL::R;
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                }
                else
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " LD R," + choose(*it, 1, 0);
                    Object.push_back(s);
                }
            }
            RDL::R = choose(*it, 3, 0);
            RDL::active = choose(*it, 3, 1);
        }
        else if (Operate.find(choose(*it, 0, 0)) != Operate.end() && Operate[choose(*it, 0, 0)][0] == "w")
        {
            if (RDL::R == "0")
            {
                string s_1, s_2;
                stringstream stream;
                stream << count-1;
                stream >> s_1;
                count++;
                stream.clear();
                stream << count-1;
                stream >> s_2;
                count++;
                s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                s_2 = "lab" + s_2 + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                Object.push_back(s_2);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_2);
                }
                else
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s);
                }
            }
            else if (RDL::R == choose(*it, 2, 0) && Operate[choose(*it, 0, 0)][2] == "y")
            {
                if (choose(*it, 2, 1) == "y")
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + choose(*it, 2, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                }
                else
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s);
                }
            }
            else
            {
                if (RDL::active == "y")
                {
                    string s_1, s_2, s_3;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_3;
                    count++;
                    stream.clear();
                    s_1 = "lab" + s_1 + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    s_3 = "lab" + s_3 + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_3);
                }
                else
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_2);
                }
            }

            RDL::R = choose(*it, 3, 0);
            RDL::active = choose(*it, 3, 1);
        }
        else if (choose(*it, 0, 0) == "if")
        {
            if (RDL::R == "0")
            {
                string s_1, s_2;
                stringstream stream;
                stream << count-1;
                stream >> s_1;
                count++;
                stream.clear();
                stream << count-1;
                stream >> s_2;
                count++;
                s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                s_2 = "lab" + s_2 + " " + "FJ R,";
                SEM.push(count - 1);
                Object.push_back(s_2);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "FJ R,";
                    Object.push_back(s_2);
                }
                else
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " " + "FJ R,";
                    Object.push_back(s);
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
            else
            {
                if (RDL::active == "y")
                {
                    string s_1, s_2, s_3;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_3;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    s_3 = "lab" + s_3 + " " + "FJ R,";
                    Object.push_back(s_3);
                }
                else
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "FJ R,";
                    Object.push_back(s_2);
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
        }
        else if (choose(*it, 0, 0) == "el")
        {
            if (RDL::active == "y")
            {
                string s;
                stringstream stream;
                stream << count-1;
                stream >> s;
                count++;
                s = "lab" + s + " " + "ST R," + RDL::R;
                Object.push_back(s);
            }
            string s_1, s_2;
            stringstream stream;
            stream << count + 1;
            stream >> s_1;
            stream.clear();
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + "lab" + s_1;
            SEM.pop();
            stream << count-1;
            stream >> s_2;
            count++;
            s_2 = "lab" + s_2 + " " + "JMP ";
            Object.push_back(s_2);
            SEM.push(count - 1);
        }
        else if (choose(*it, 0, 0) == "ie")
        {
            if (RDL::active == "y")
            {
                string s;
                stringstream stream;
                stream << count-1;
                stream >> s;
                count++;
                s = "lab" + s + " " + "ST R," + RDL::R;
                Object.push_back(s);
            }
            string s;
            stringstream stream;
            stream << count-1;
            stream >> s;
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + "lab" + s;
            SEM.pop();
        }
        else if (choose(*it, 0, 0) == "wh")
        {
            SEM.push(count);
        }
        else if (choose(*it, 0, 0) == "do")
        {
            if (RDL::R == "0")
            {
                string s_1, s_2;
                stringstream stream;
                stream << count-1;
                stream >> s_1;
                count++;
                stream.clear();
                stream << count-1;
                stream >> s_2;
                count++;
                s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                s_2 = "lab" + s_2 + " " + "FJ R,";
                Object.push_back(s_2);
                SEM.push(count - 1);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "FJ R,";
                    Object.push_back(s_2);
                }
                else
                {
                    string s;
                    stringstream stream;
                    stream << count-1;
                    stream >> s;
                    count++;
                    s = "lab" + s + " " + "FJ R,";
                    Object.push_back(s);
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
            else
            {
                if (RDL::active == "y")
                {
                    string s_1, s_2, s_3;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_3;
                    count++;
                    s_1 = "lab" + s_1 + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    s_3 = "lab" + s_3 + " " + "FJ R,";
                    Object.push_back(s_3);
                }
                else
                {
                    string s_1, s_2;
                    stringstream stream;
                    stream << count-1;
                    stream >> s_1;
                    count++;
                    stream.clear();
                    stream << count-1;
                    stream >> s_2;
                    count++;
                    s_1 = "lab" + s_1 + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    s_2 = "lab" + s_2 + " " + "FJ R,";
                    Object.push_back(s_2);
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
        }
        else if (choose(*it, 0, 0) == "we")
        {
            if (RDL::active == "y")
            {
                string s;
                stringstream stream;
                stream << count-1;
                stream >> s;
                count++;
                s = "lab" + s + " " + "ST R," + RDL::R;
                Object.push_back(s);
            }
            RDL::R = "0";
            RDL::active = "0";
            string s_1, s_2;
            stringstream stream;
            stream << count + 1-1;
            stream >> s_1;
            stream.clear();
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + "lab" + s_1;
            SEM.pop();
            stream << count-1;
            stream >> s_1;
            count++;
            stream.clear();
            stream << SEM.top();
            stream >> s_2;
            s_1 = "lab" + s_1 + " " + "JMP " + "lab" + s_2;
            Object.push_back(s_1);
            SEM.pop();
        }
        else if (choose(*it, 0, 0) == "fun")
        {
            count++;
            Object.push_back(choose(*it,1,0)+" "+"SEGMENT");
        }
        else if (choose(*it, 0, 0) == "endfun")
        {
            Object.push_back("SEGMENT END");
        }
        else if (choose(*it, 0, 0) == "ret")
        {
            string s;
            stringstream stream;
            stream<<count-1;
            stream>>s;
            count++;
            stream.clear();
            s="lab"+s+" LD R,"+choose(*it,1,0);
            Object.push_back(s);
            RDL::R = choose(*it, 1, 0);
            RDL::active = choose(*it, 1, 0);
        }
        else
        {
            cout << "目标代码生成错误！";
            exit(-1);
        }
    }
}

void TargetCode::scan(MainWindow* w)
{
    string str = "$";
    string tem;
    ifstream in("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\optQuat.txt");
    while (!in.eof())
    {
        in >> tem;
        str = str + tem + "$";
    }
    str[str.size() - 1] = '#';

    int flag = 0;

    for (int i = 0; str[i] != '#'; i++)
    {
        int k = str[1];
        if (str[i] == '$' && str[i + 1] == k) flag = 1;
        if (str[i] == '$' && str[i + 1] != k)
        {
            getVar();
            activeinf();
            fillObj();
            Quadruple.clear();
            flag = 1;
            k = str[i + 1];
        }
        if (flag == 1)
        {
            if (str[i] == '(')
            {
                int j;
                for (j = i; str[j] != ')'; j++) {}
                Quadruple.push_back(str.substr(i, j - i + 1));
                i = j;
                flag = 0;
            }
        }
    }
    print(w);
}

int TargetCode::print(MainWindow* w)
{
    fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\ObjectCode.txt", fstream::out);
    for (vector<string>::iterator it = Object.begin(); it != Object.end(); it++)
    {
        File << *it << endl;
        w->outCode(QString::fromStdString(*it));
    }
    File.close();
    w->outInfor("目标代码已输出. . .");
    //cout << "目标代码已输出。" << endl;
    return 0;
}
