#ifndef TARGETCODE_H
#define TARGETCODE_H
#include <vector>
#include <string>
#include <map>
#include "mainwindow.h"

using namespace std;

class RDL
{
public:
    static string R;
    static string active;
};

class TargetCode
{
public:
    static int count;
    template <typename T>bool existT(vector<T> v, T t);
    void clear();
    void getVar();
    void activeinf();
    void fillObj();
    void scan(MainWindow* w);
    int print(MainWindow* w);
    string choose(string s, int comma, int lag);	//获取第comma个逗号后的标识符(lag=0)/活跃信息(lag=1)
};

#endif // TARGETCODE_H
