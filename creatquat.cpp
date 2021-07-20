#include<iostream>
//#include<bits/stdc++.h>
#include <list>
#include <stack>
#include <sstream>
#include <string>
#include <fstream>
#include "creatquat.h"
using namespace std;

quat qt[100];//四元式区
string ttable[100], tnow;//t表,当前t
stack<string>sem;//语义栈

int creatquat::q = 0;
int creatquat::t = 0;

void creatquat::clear()
{
    tnow.clear();
    creatquat::q = 0;
    creatquat::t = 0;
}

void creatquat::action(string top, string temppush)
{
    if (top == "$PUSHi$")PUSHi(temppush);
    else if (top == "$GEQadd$")GEQadd();
    else if (top == "$GEQsub$")GEQsub();
    else if (top == "$GEQmul$")GEQmul();
    else if (top == "$GEQdiv$")GEQdiv();
    else if (top == "$ASSI$")ASSI();
    else if (top == "$IF$")IF();
    else if (top == "$IE$")IE();
    else if (top == "$EL$")EL();
    else if (top == "$WH$")WH();
    else if (top == "$DO$")DO();
    else if (top == "$WE$")WE();
    else if (top == "$LOGIC$")LOGIC();
    else if (top == "$FUN$")FUN();
    else if (top == "$RET$")RET();
    else if (top == "$FUNEND$")FUNEND();
}

void creatquat::GEQadd()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "+";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void creatquat::GEQsub()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "-";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void creatquat::GEQmul()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "*";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void creatquat::GEQdiv()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "/";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void creatquat::PUSHi(string str)//算术表达式常数、标志符进栈，在此之前str保存当前所需压入的nowWord.s
{
    sem.push(str);
}

void creatquat::ASSI()
{
    int qi;
    qi = q;

    qt[qi].op = "=";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = sem.top();
    sem.push(sem.top());
    sem.pop();

    qi++;
    q = qi;
}

void creatquat::IF()
{
    int qi;
    qi = q;

    qt[qi].op = "if";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::EL()
{
    int qi;
    qi = q;

    qt[qi].op = "el";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::IE()
{
    int qi;
    qi = q;

    qt[qi].op = "ie";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::WH()
{
    int qi;
    qi = q;

    qt[qi].op = "wh";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::DO()
{
    int qi;
    qi = q;

    qt[qi].op = "do";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::WE()
{
    int qi;
    qi = q;

    qt[qi].op = "we";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::LOGIC()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].op = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void creatquat::FUN()
{
    int qi;
    qi = q;

    qt[qi].op = "fun";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::FUNEND()
{
    int qi;
    qi = q;

    qt[qi].op = "endfun";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void creatquat::RET()
{
    int qi;
    qi = q;

    qt[qi].op = "ret";
    qt[qi].result = "_";
    qt[qi].arg2 = "_";
    qt[qi].arg1 = sem.top();;
    sem.pop();

    qi++;
    q = qi;
}

void creatquat::PRINTQT(MainWindow* w)
{
    fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\Quaternion.txt", fstream::out);
    int qi=0;
    while(qi!=creatquat::q)
    {
        File << "(" << qi+1 << ")" << "(" << qt[qi].op << ", " << qt[qi].arg1 << ", " << qt[qi].arg2 << ", " << qt[qi].result << ")" << endl;
        qi++;
    }
    File.close();
    w->outInfor("四元式已输出. . .");
}

