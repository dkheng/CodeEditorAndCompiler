#include "symboltable.h"
#include <sstream>
#include <fstream>

ostream& operator<<(ostream& os, const SYNBLEntry& entry)
{
    os << entry.name << '\t' << entry.type << '\t'
       << entry.category << '\t' << entry.address << endl;
    return os;
}

ostream& operator<<(ostream& os, const TAPELEntry& entry)
{
    os << entry.typeValue << '\t' << entry.typePoint << endl;
    return os;
}

ostream& operator<<(ostream& os, const PFINFLEntry& entry)
{
    os << entry.level << '\t' << entry.offset << '\t'
        << entry.paraNum << '\t' << entry.paraTable << endl;
    return os;
}

map<string, SymbolTable::Act> SymbolTable::ACTIONL =
{
    {"\"函数名\"", &SymbolTable::func}, {"\"int形参名\"", &SymbolTable::formalParaInt},
    {"\"int变量名\"", &SymbolTable::intVariable}, {"\"参数串结束\"", &SymbolTable::formalParaEnd}
};

SymbolTable::SymbolTable()
{
    TAPEL.push_back({ "int", "空" });
    VALL.push_back("OLD SP");
    VALL.push_back("返回地址");
    VALL.push_back("全局display地址");
}

int SymbolTable::callAction(string action,ReToken token)
{
    return (this->*ACTIONL[action])(token);
}

int SymbolTable::printTable(MainWindow* w)
{
    fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\SymbolTable.txt",fstream::out);
    int j = 0;
    File << "符号表总表\n序号\t标识符名\t类型\t种类\t地址指针" << endl;
    for (auto i = SYNBL.begin(); i != SYNBL.end(); i++)
        File << j++ << "\t" << *i;
    j = 0;
    File << "\n\n类型表\n序号\t类型\t类型信息指针" << endl;
    for (auto i = TAPEL.begin(); i != TAPEL.end(); i++)
        File << j++ << "\t" << *i;
    j = 0;
    File << "\n\n函数表\n序号\t层次号\t区距\t参数个数\t参数表指针" << endl;
    for (auto i = PFINFL.begin(); i != PFINFL.end(); i++)
        File << j++ << "\t" << *i;
    j = VALL.size() - 1;
    File << "\n\n活动记录表VALL\n" << endl;
    for (auto i = VALL.rbegin(); i != VALL.rend(); i++)
        File << j-- << "   " << *i << endl;
    File.close();
    w->outInfor("符号表已输出. . .");
    //cout << "符号表已输出。" << endl;
    return 1;
}

int SymbolTable::func(ReToken token)
{
    if (token.c == 'I')
    {
        VALL.push_back("0");   //填写VALL表参数个数
        PFINFL.push_back({ 0,3,0,"空" });   //填写函数表
        stringstream addr;
        addr << "PFINFL[" << PFINFL.size() - 1 << "]";
        SYNBL.push_back({ token.s,"空","f",addr.str() });  //填写符号表总表
    }
    return 1;
}

int SymbolTable::formalParaInt(ReToken token)
{
    if (token.c == 'I')
    {
        VALL.push_back(token.s);   //填写VALL表
        stringstream addr;
        addr << "VALL[" << VALL.size() - 1 << "]";
        SYNBL.push_back({ token.s,"TAPEL[0]","vf",addr.str() });  //填写符号表总表
        (PFINFL.rbegin())->addParaNum();   //递增函数表该函数形参个数
        addr.str(""); addr << (PFINFL.rbegin())->getParaNum();
        VALL[3] = addr.str();  //修改VALL表中参数个数
        addr.str(""); addr << "SYNBL[" << SYNBL.size() - 1 << "]";
        (PFINFL.rbegin())->modParaTable(addr.str());  //修改函数表该函数参数表指针
    }
    return 1;
}

int SymbolTable::formalParaEnd(ReToken token)
{
    VALL.push_back("Display表");
    return 1;
}

int SymbolTable::intVariable(ReToken token)
{
    if (token.c == 'I')
    {
        VALL.push_back(token.s);   //填写VALL表
        stringstream addr;
        addr << "VALL[" << VALL.size() - 1 << "]";
        SYNBL.push_back({ token.s,"TAPEL[0]","v",addr.str() });  //填写符号表总表
    }
    return 1;
}
