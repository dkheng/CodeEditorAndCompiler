#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include "mainwindow.h"

using namespace std;

class ReToken
{
public:
    char c;
    int number;
    string s;
};

class WordAnalysis
{
public:
    static int count;
    bool clear();
    bool scan(string s, ReToken& To, MainWindow* w);		//词法分析Token序列
    template<typename T>void print(vector<T> v, MainWindow* w);
    template<typename T>int existT(T t, vector<T> v);
    int isI(string s, int i, ReToken& To);		//标识符判断
    int isC(string s, int i, ReToken& To, MainWindow* w);
    int isP(string s, int i, ReToken& To);
    int delBlank(string s, int i);
    float expop(int start, int end, int loc, char w, string s, int n_ope);			//指数处理
};



#endif // LEXICALANALYSIS_H
