#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <string>
#include <vector>
#include <map>
#include "mainwindow.h"
#include "lexicalanalysis.h"

using namespace std;

class SYNBLEntry   //符号表总表表项
{
public:
    SYNBLEntry() = default;
    SYNBLEntry(string n, string t, string c, string a)
                : name(n), type(t), category(c), address(a) {}
    friend ostream& operator<<(ostream& os, const SYNBLEntry& entry);
private:
    string name;   //标识符名字
    string type;   //标识符类型
    string category;   //标识符种类
    string address;   //地址指针
};

class TAPELEntry   //类型表表项
{
public:
    TAPELEntry() = default;
    TAPELEntry(string tv, string tp) : typeValue(tv), typePoint(tp) {}
    friend ostream& operator<<(ostream& os, const TAPELEntry& entry);
private:
    string typeValue;   //类型
    string typePoint;   //类型信息指针
};

class PFINFLEntry   //函数表表项
{
public:
    PFINFLEntry() = default;
    PFINFLEntry(int l, int o, int pn, string pt)
                : level(l), offset(o), paraNum(pn), paraTable(pt) {}
    void addParaNum() { paraNum++; }
    void modParaTable(string s) { paraTable = s; }
    int getParaNum() { return paraNum; }
    friend ostream& operator<<(ostream& os, const PFINFLEntry& entry);
private:
    int level;   //层次号
    int offset;   //区距
    int paraNum;   //参数个数
    string paraTable;   //参数表指针
};

class SymbolTable
{
public:
    typedef int (SymbolTable::* Act)(ReToken token);
    SymbolTable();
    int callAction(string action, ReToken token);
    int printTable(MainWindow* w);

private:
    static map<string, Act> ACTIONL;
    vector<SYNBLEntry> SYNBL;
    vector<TAPELEntry> TAPEL;
    vector<PFINFLEntry> PFINFL;
    vector<string> VALL;

    int func(ReToken token);
    int formalParaInt(ReToken token);
    int intVariable(ReToken token);
    int formalParaEnd(ReToken token);
};

#endif // SYMBOLTABLE_H
