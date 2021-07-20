#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <iostream>
#include <cstring>
#include <stdio.h>
#include<stdlib.h>
#include <stack>
#include <vector>
#include <sstream>
#include <iomanip>
#include "mainwindow.h"
//#include <bits/stdc++.h>

using namespace std;

const int maxn = (int)1e2 + 5;

struct Opt {

    struct token {
        string name;
        string type;     //2为临时变量，3为数字，1为非临时变量
    };

    struct Quaternion {      //四元式结构体
        int block;           //基本块标识
        int type;            //类型-0 1 2 3
        string op;           //操作
        token num1;         //操作数1
        token num2;         //操作数2
        token ans;          //结果
    };

    struct ACT_INF //四元式的活跃信息
    {
        string num1;
        string num2;
        string ans;
        ACT_INF(string a = "", string b = "", string c = "")
        {
            num1 = a; num2 = b; ans = c;
        }
    };

    struct ACT_SYNBL
    {
        string name;
        string act_inf;
        ACT_SYNBL(string a = "", string b = "")
        {
            name = a; act_inf = b;
        }
    };

    vector< vector<ACT_INF> >Act_inf;
    vector< vector<ACT_SYNBL> >Act_synbl;
    vector<string>objcode;
    stack<string>SEM; //保存待返填地址

    struct Quaternion qua[maxn];   //四元式数组
    vector<Quaternion> Block[15];
    vector<vector<Quaternion>> ToTarget;
    struct Node {                //DAG节点
        int left;
        int right;
        int id;
        string op;
        token m_sign;                    //主标记
        token sign[maxn];                  //附加标记
    };

    struct DAG {                  //DAG
        int num = 0;
        Node node[maxn];
    }dag;

    void init();     //初始化四元式结构体数组
    void addition(int n, string A, string typ);   //把标记A添加到节点n上，顺便删除无用标记
    int getnode(string B);   //获取标记为B的节点的id
    void makeleaf(string B, string typ);   //构造叶子节点
    void delnode(int n);     //删除id为n的节点
    int findnode(string opl, string B, string C);   //在DAG中查找有无符合A=B op C的节点
    int makenode(string opl, string B, string C);  //构造中间节点
    string coutvalue(string opl, string B, string C);   //计算两个数运算的值
    void makequa(int block, fstream &File, MainWindow* w);   //生成block块中优化后的四元式
    void deletedag();  //对dag清零
    void optimizequa(int block_num, fstream &File, MainWindow* w);    //优化四元式
    int divide();             //划分基本块
    void run(MainWindow* w);
};

#endif // OPTIMIZATION_H
