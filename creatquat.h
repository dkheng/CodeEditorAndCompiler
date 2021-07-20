#ifndef CREATQUAT_H
#define CREATQUAT_H

#include<iostream>
//#include<bits/stdc++.h>
#include "mainwindow.h"
using namespace std;

class quat//定义四元式
{
public:
    string op;
    string arg1;
    string arg2;
    string result;
};

class creatquat
{
public:
    static int q;
    static int t;
    void clear();
    void action(string top, string temppush);
    void PUSHi(string str);//<因子> <标识符> $PUSHi$ | <无符号整数> $PUSHi$ | ( <表达式> ) #
    void GEQadd();//+-*/
    void GEQsub();
    void GEQmul();
    void GEQdiv();
    void ASSI();//<赋值语句> @ <标识符>=<表达式> assi;
    void IF();//<条件语句> if ( <条件> ) $IF$ <复合语句> <else语句> $IE$#
    void EL();//<else语句> else $EL$ <复合语句> | <空> #
    void IE();
    void WH();//<循环语句> while $WH$ ( <条件> )  $DO$ <复合语句> $WE$ #
    void DO();
    void WE();
    void LOGIC();//<条件> <表达式> <比较语句> $LOGIC$ #
    void RET();//<返回内容> ; | <表达式> ; $RET$#
    void FUN();//<函数> <返回类型> "函数名" $PUSHi$ <标识符> <参数列表> $FUN$ <复合语句> $FUNEND$ #
    void FUNEND();
    void PRINTQT(MainWindow* w);//输出
};


#endif // CREATQUAT_H
