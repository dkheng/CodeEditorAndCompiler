#include "optimization.h"
#include "creatquat.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include<stdlib.h>

extern quat qt[1000];
void Opt::init()     //四元式数组转成结构体
{
    for (int i = 0; qt[i].op != ""; i++)
    {
        qua[i].op = qt[i].op;
        qua[i].num1.name = qt[i].arg1;
        qua[i].num2.name = qt[i].arg2;
        qua[i].ans.name = qt[i].result;
        if (qt[i].op == "=") {
            qua[i].type = 0;
        }
        else if (qt[i].op == ">" || qt[i].op == "<" || qt[i].op == "+" || qt[i].op == "-" || qt[i].op == "*" || qt[i].op == "/") {
            qua[i].type = 2;
        }
        else
            qua[i].type = 3;                                    //操作符初始化

        if ((qt[i].arg1[0] >= 'a' && qt[i].arg1[0] <= 'z') || (qt[i].arg1[0] >= 'A' && qt[i].arg1[0] <= 'Z'))
        {
            if (qt[i].arg1[1] >= '1' && qt[i].arg1[1] <= '9')
            {
                qua[i].num1.type = "2";
            }
            else
            {
                qua[i].num1.type = "1";
            }
        }
        else if (qt[i].arg1[0] >= '0' && qt[i].arg1[0] <= '9')
        {
            qua[i].num1.type = "3";
        }
        else if (qt[i].arg1 == "_")
        {
            qua[i].num1.type = "";
        }                                        //arg1初始化


        if ((qt[i].arg2[0] >= 'a' && qt[i].arg2[0] <= 'z') || (qt[i].arg2[0] >= 'A' && qt[i].arg2[0] <= 'Z'))
        {
            if (qt[i].arg2[1] >= '1' && qt[i].arg2[1] <= '9')
            {
                qua[i].num2.type = "2";
            }
            else
            {
                qua[i].num2.type = "1";
            }
        }
        else if (qt[i].arg2[0] >= '0' && qt[i].arg2[0] <= '9')
        {
            qua[i].num2.type = "3";
        }
        else if (qt[i].arg2 == "_")
        {
            qua[i].num2.type = "";
        }                                       //arg2初始化

        if ((qt[i].result[0] >= 'a' && qt[i].result[0] <= 'z') || (qt[i].result[0] >= 'A' && qt[i].result[0] <= 'Z'))
        {
            if (qt[i].result[1] >= '1' && qt[i].result[1] <= '9')
            {
                qua[i].ans.type = "2";
            }
            else
            {
                qua[i].ans.type = "1";
            }
        }
        else if (qt[i].result[0] >= '0' && qt[i].result[0] <= '9')
        {
            qua[i].ans.type = "3";
        }
        else if (qt[i].result == "_")
        {
            qua[i].ans.type = "";
        } // result初始化

    }
}

void Opt::addition(int n, string A, string typ)   //把标记A添加到节点n上，顺便删除无用标记
{
    int j, k;
    k = getnode(A);
    if (k != 0)      //如果getnode(A)不为空
    {
        for (j = 0; j < maxn; j++)       //在节点getnode(A)上删除附标记
        {
            if (dag.node[k - 1].sign[j].name == A)
            {
                dag.node[k - 1].sign[j].name = "";
                dag.node[k - 1].sign[j].type = "";
            }
        }
    }
    if (dag.node[n - 1].m_sign.name != "")
    {
        for (j = 0; j < maxn; j++)     //在节点n上添加附标记
        {
            if (dag.node[n - 1].sign[j].name == "")
            {
                dag.node[n - 1].sign[j].name = A;
                dag.node[n - 1].sign[j].type = typ;
                break;
            }
        }
    }
    else { dag.node[n - 1].m_sign.name = A; dag.node[n - 1].m_sign.type = typ; }

    if (dag.node[n - 1].m_sign.type == "2")    //若主标记为临时变量，附标记为非临时，则互换
    {
        for (j = 0; j < maxn; j++)
        {
            if (dag.node[n - 1].sign[j].type == "1")
            {
                string a;
                a = dag.node[n - 1].sign[j].name;
                dag.node[n - 1].sign[j].name = dag.node[n - 1].m_sign.name;
                dag.node[n - 1].m_sign.name = a;
                dag.node[n - 1].sign[j].type = "2";
                dag.node[n - 1].m_sign.type = "1";
            }
        }
    }
}

int Opt::getnode(string B)   //获取标记为B的节点的id
{
    int n = 0;      //节点id
    for (int i = 0; i < dag.num; i++)
    {
        if (dag.node[i].m_sign.name == B) n = i + 1;
        for (int j = 0; j < maxn; j++)
        {
            if (dag.node[i].sign[j].name == B) n = i + 1;
        }
    }
    return n;
}

void Opt::makeleaf(string B, string typ)   //构造叶子节点
{
    dag.num++;
    dag.node[dag.num - 1].m_sign.name = B;
    dag.node[dag.num - 1].m_sign.type = typ;
    dag.node[dag.num - 1].id = dag.num;
}

void Opt::delnode(int n)     //删除id为n的节点
{
    dag.node[n - 1].m_sign.name = "";
    dag.node[n - 1].id = 0;
    for (int i = n - 1; i < dag.num - 1; i++)
    {
        dag.node[i].id = dag.node[i + 1].id;
        dag.node[i].left = dag.node[i + 1].left;
        dag.node[i].m_sign.name = dag.node[i + 1].m_sign.name;
        dag.node[i].m_sign.type = dag.node[i + 1].m_sign.type;
        dag.node[i].op = dag.node[i + 1].op;
        dag.node[i].right = dag.node[i + 1].right;
        for (int j = 0; j < maxn; j++)
        {
            dag.node[i].sign[j].name = dag.node[i + 1].sign[j].name;
            dag.node[i].sign[j].type = dag.node[i + 1].sign[j].type;
        }
    }
    dag.node[dag.num - 1].id = 0;
    dag.node[dag.num - 1].left = 0;
    dag.node[dag.num - 1].m_sign.name = "";
    dag.node[dag.num - 1].m_sign.type = "";
    dag.node[dag.num - 1].op = "";
    dag.node[dag.num - 1].right = 0;
    for (int j = 0; j < maxn; j++)
    {
        dag.node[dag.num - 1].sign[j].name = "";
        dag.node[dag.num - 1].sign[j].type = "";
    }
    dag.num--;
}

int Opt::findnode(string opl, string B, string C)   //在DAG中查找有无符合A=B op C的节点
{
    int n = 0;
    int l, r, k = 0;
    l = getnode(B);
    r = getnode(C);
    for (int i = 0; i < dag.num; i++)
    {
        k = 0;
        if (dag.node[i].op == opl)  //op满足
        {
            l = dag.node[i].left;
            for (int j = 0; j < maxn; j++)
            {
                if (dag.node[l - 1].sign[j].name == B) k = 1;
            }
            if ((dag.node[l - 1].m_sign.name == B) || k == 1)  //B满足
            {
                k = 0;
                r = dag.node[i].right;
                for (int j = 0; j < maxn; j++)
                {
                    if (dag.node[r - 1].sign[j].name == C) k = 1;
                }
                if ((dag.node[r - 1].m_sign.name == C) || k == 1)  //C满足
                {
                    n = i + 1; break;
                }
            }
        }
    }
    return n;
}

int Opt::makenode(string opl, string B, string C)  //构造中间节点
{
    dag.num++;
    dag.node[dag.num - 1].id = dag.num;
    dag.node[dag.num - 1].left = getnode(B);
    dag.node[dag.num - 1].right = getnode(C);
    dag.node[dag.num - 1].op = opl;
    return dag.num;
}

string Opt::coutvalue(string opl, string B, string C)   //计算两个数运算的值
{
    char a;
    char s[maxn];
    int v;
    a = opl[0];
    switch (a)
    {
    case '+':
        v = atof(B.c_str()) + atof(C.c_str());
        break;
    case '-':
        v = atof(B.c_str()) - atof(C.c_str());
        break;
    case '*':
        v = atof(B.c_str()) * atof(C.c_str());
        break;
    case '/':
        v = atof(B.c_str()) / atof(C.c_str());
        break;
    }
    sprintf_s(s, 105, "%d", v);
    string p(s);
    return p;
}

void Opt::makequa(int block, fstream& File,MainWindow* w)   //生成block块中优化后的四元式
{
    stringstream ss;
    int i, j, k = 0;
    int numB = 0, l, r;
    for (i = 0; i < dag.num; i++)
    {
        k = 0;
        if (dag.node[i].op == "")  //是叶子节点
        {
            for (j = 0; j < maxn; j++)
            {
                if (dag.node[i].sign[j].type == "1") { k = 1; numB = j; }
            }
            if (dag.node[i].sign[numB].name != "") {

                if (dag.node[i].m_sign.type == "3" ||
                    (dag.node[i].m_sign.type == "1" && k == 1))  //主标记为数字，附标记有非临时变量
                {
                    File << block << " (=," << dag.node[i].m_sign.name << ",_," << dag.node[i].sign[numB].name << ")" << endl; //( = B _ A)
                    ss.str("");
                    ss  <<"( =, " << dag.node[i].m_sign.name << ", _, " << dag.node[i].sign[numB].name << " )" << endl;
                    w->outQuat(QString::fromStdString(ss.str()));
                    qua[0].ans.name = dag.node[i].m_sign.name; qua[0].ans.type = dag.node[i].m_sign.type;
                    qua[0].block = block;
                    qua[0].num1.name = dag.node[i].sign[numB].name; qua[0].num1.type = dag.node[i].sign[numB].type;
                    qua[0].num2.name = ""; qua[0].num2.type = "";
                    qua[0].op = "=";
                    qua[0].type = 0;
                    Block[block].push_back(qua[0]);
                }
            }
        }
        else
        {
            l = dag.node[i].left;
            r = dag.node[i].right;
            File << block << " (" << dag.node[i].op << "," << dag.node[l - 1].m_sign.name << "," << dag.node[r - 1].m_sign.name << "," << dag.node[i].m_sign.name << ")" << endl;
            ss.str("");
            ss  << "( " << dag.node[i].op << ", " << dag.node[l - 1].m_sign.name << ", " << dag.node[r - 1].m_sign.name << ", " << dag.node[i].m_sign.name << " )" << endl;
            w->outQuat(QString::fromStdString(ss.str()));
            qua[0].ans.name = dag.node[i].m_sign.name; qua[0].ans.type = dag.node[i].m_sign.type;
            qua[0].block = block;
            qua[0].num1.name = dag.node[l - 1].m_sign.name; qua[0].num1.type = dag.node[l - 1].m_sign.type;
            qua[0].num2.name = dag.node[r - 1].m_sign.name; qua[0].num2.type = dag.node[r - 1].m_sign.type;
            qua[0].op = dag.node[i].op;
            qua[0].type = 2;

            Block[block].push_back(qua[0]);
        }
    }
}

void Opt::deletedag()   //对dag清零
{
    int i, j;
    for (i = 0; i < dag.num; i++)
    {
        dag.node[i].id = 0;
        dag.node[i].left = 0;
        dag.node[i].m_sign.name = "";
        dag.node[i].m_sign.type = "";
        dag.node[i].op = "";
        dag.node[i].right = 0;
        for (j = 0; j < maxn; j++)
        {
            dag.node[i].sign[j].name = "";
            dag.node[i].sign[j].type = "";
        }
    }
    dag.num = 0;
}

int Opt::divide()      //划分基本块
{
    int i = 0;
    int num = 1;
    while (qua[i].op != "")        //当还有未处理的四元式，循环
    {
        if (qua[i].type != 3 || qua[i].op == "lb" || qua[i].op == "wh"||qua[i].op=="fun" || qua[i].op == "ret" || qua[i].op == "endfun")
            qua[i].block = num;
        if (qua[i].op == "if" || qua[i].op == "el" || qua[i].op == "ie" || qua[i].op == "do" || qua[i].op == "we" || qua[i].op == "gt")
        {
            qua[i].block = num; num++;
        }
        i++;
    }
    if (qua[i - 1].op == "if" || qua[i - 1].op == "el" || qua[i - 1].op == "ie" || qua[i - 1].op == "do" || qua[i - 1].op == "we" || qua[i - 1].op == "gt")
        num--;
    return num;
}

void Opt::optimizequa(int block_num, fstream& File,MainWindow* w)    //优化四元式
{
    stringstream ss;
    int newleft, newright;        //新建节点id
    int i = 0, j, k;
    int n;
    string p;                   //若B,C为数字，p=B op C
    string A, B, C;               //（，B，C，A）
    vector<int> type3;
    //init();                      //初始化四元式结构体数组
    for (k = 1; k <= block_num; k++)   //块循环
    {
        type3.clear();
        if (qua[i].type == 3)
        {
            File << qua[i].block << " (" << qua[i].op << "," << qua[i].num1.name << "," << qua[i].num2.name << "," << qua[i].ans.name << ")" << endl;
            ss.str("");
            ss  << "( " << qua[i].op << ", " << qua[i].num1.name << ", " << qua[i].num2.name << ", " << qua[i].ans.name << " )" << endl;
            w->outQuat(QString::fromStdString(ss.str()));
            Block[qua[i].block].push_back(qua[i]);
            i++;
            k = qua[i].block;
        }
        while (qua[i].op != "" && qua[i].block == k)        //当基本块还有未处理的四元式且是同一块时，循环
        {
            if (qua[i].type == 3) type3.push_back(i);
            A = qua[i].ans.name;
            B = qua[i].num1.name;
            C = qua[i].num2.name;
            newleft = 0;
            newright = 0;
            if (getnode(B) == 0 && qua[i].type != 3) { makeleaf(B, qua[i].num1.type); newleft = getnode(B); }
            switch (qua[i].type)
            {
            case 0:   //单目
                n = getnode(B);
                addition(n, A, qua[i].ans.type);
                break;
            case 2:   //双目
                if (getnode(C) == 0) { makeleaf(C, qua[i].num2.type); newright = getnode(C); }
                j = 0;
                if ((qua[i].op == "*") || (qua[i].op == "/") || (qua[i].op == "+") || (qua[i].op == "-"))j = 1;
                if ((qua[i].num1.type == "3") && (qua[i].num2.type == "3") && j == 1)   //两个操作数都为数字
                {
                    p = coutvalue(qua[i].op, B, C);
                    if (newleft != 0) delnode(newleft);  //是新节点，则删除
                    if (newright != 0) delnode(newright);
                    if ((n = getnode(p)) == 0) { makeleaf(p, "3"); n = getnode(p); }
                }
                else
                {
                    if ((n = findnode(qua[i].op, B, C)) == 0)  //在DAG中查找有无符合A=B op C的节点
                        n = makenode(qua[i].op, B, C);
                }
                addition(n, A, qua[i].ans.type);                  //添加附加标记
                break;
            case 3:
                break;
            }
            i++;
        }
        makequa(k,File,w);      //生成优化的四元式
        for (auto j = type3.begin(); j != type3.end(); j++)
        {
            int i = *j;
            File << qua[i].block << " (" << qua[i].op << "," << qua[i].num1.name << "," << qua[i].num2.name << "," << qua[i].ans.name << ")" << endl;
            ss.str("");
            ss  << "( " << qua[i].op << ", " << qua[i].num1.name << ", " << qua[i].num2.name << ", " << qua[i].ans.name << " )" << endl;
            w->outQuat(QString::fromStdString(ss.str()));
            Block[qua[i].block].push_back(qua[i]);
        }
        deletedag();    //对dag清零
    }
}

void  Opt::run(MainWindow* w)
{
    int block_num;  //块的总数
    init();
    block_num = divide();
    fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\optQuat.txt", fstream::out);
    optimizequa(block_num,File,w);
    File.close();
    w->outInfor("优化后四元式已输出. . .");
    //cout << "优化后目标代码已输出。" << endl;
}
