#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "syntaxanalysis.h"
#include "symboltable.h"
#include "creatquat.h"

using namespace std;

vector<string> VNList;   //非终结符列表
vector<string> VTList;   //终结符列表

//LL(1)分析表，格式为{"E", {{"i", 1}, {"(", 1}}}
//"E"为行表头，"i"和"("为列表头，1为该表项内容即对应的产生式序号
map<string, map<string, int>> LL1Table;

vector<vector<string>> grammarRight;  //grammar按序号存储所有产生式右部

bool SyntaxAnalysis::analyse(string program, MainWindow* w)
{
    VNList.clear(); VTList.clear();
    LL1Table.clear();
    grammarRight.clear();
    list<string> stack;   //分析栈
    stack.push_back("#");
    stack.push_back("<函数>");  //开始符号入栈
    string topSymbol;   //栈顶符号
    ReToken nowWord;   //现在分析的单词
    ReToken lastWord;
    LL1TableMachine(w);

    SymbolTable symTable;
    creatquat quat;
    quat.clear();
    WordAnalysis wordAna;
    wordAna.clear();
    wordAna.scan(program, nowWord, w);
    while (1)
    {
        topSymbol = stack.back();
        stack.pop_back();
        //如果栈顶符号是终结符
        if (isVT(topSymbol))
        {
            if (topSymbol == nowWord.s
                || (topSymbol == "<标识符>" && nowWord.c == 'I')
                || (topSymbol == "<无符号整数>" && nowWord.c == 'C'))
            {
                lastWord = nowWord;
                wordAna.scan(program, nowWord, w);
            }
            else
            {
                w->outInfor("语法错误！请检查！");
                w->outInfor("错误信息：" + QString::fromStdString(nowWord.s));
                return false;
                //cout << "语法错误！请检查！" << endl;
                //cout << "错误信息：" << nowWord.s << endl;
                //exit(0);
            }
        }
        //如果栈顶符号是非终结符
        else if (isVN(topSymbol))
        {
            int graNum;
            if (nowWord.c == 'I')graNum = LL1Table[topSymbol]["<标识符>"];
            else if(nowWord.c=='C') graNum = LL1Table[topSymbol]["<无符号整数>"];
            else graNum = LL1Table[topSymbol][nowWord.s];
            if (graNum == -1)
            {
                w->outInfor("语法错误！请检查！");
                w->outInfor("错误信息：" + QString::fromStdString(nowWord.s));
                return false;
                //cout << "语法错误！请检查！" << endl;
                //cout << "错误信息：" << nowWord.s << endl;
                //exit(0);
            }
            else
            {
                //产生式右部逆序压栈
                for (auto i = grammarRight[graNum].rbegin(); i != grammarRight[graNum].rend(); i++)
                    if (*i != "<空>") stack.push_back(*i);
            }
        }
        //如果栈顶符号是符号表填写动作符号
        else if (isSymbolTableAction(topSymbol))
        {
            symTable.callAction(topSymbol, nowWord);
        }
        else if (isQuatAction(topSymbol))
        {
            quat.action(topSymbol, lastWord.s);
        }
        //如果栈顶符号是"#"
        else if (topSymbol == "#")
        {
            if (nowWord.s == "#")
            {
                w->outInfor("语法分析已完成. . .");
                //cout << "语法分析已完成！" << endl;
                break;
            }
            else
            {
                w->outInfor("语法错误！请检查！");
                w->outInfor("错误信息：已读取所有源码，语法分析器未成功结束！");
                return false;
                //cout << "语法错误！请检查！" << endl;
                //cout << "错误信息：已读取所有源码，语法分析器未成功结束！" << endl;
                //exit(1);
            }
        }
        else
        {
            w->outInfor("语法错误！请检查！");
            w->outInfor("错误信息：未知错误！");
            return false;
            //cout << "语法错误！请检查！" << endl;
            //cout << "错误信息：未知错误！" << endl;
            //exit(1);
        }
    }

    //输出符号表
    symTable.printTable(w);
    quat.PRINTQT(w);

    return true;
}

bool SyntaxAnalysis::isVN(string str)
{
    if (find(VNList.begin(), VNList.end(), str) == VNList.end())
        return false;
    else
        return true;
}

bool SyntaxAnalysis::isVT(string str)
{
    if (find(VTList.begin(), VTList.end(), str) == VTList.end())
        return false;
    else
        return true;
}

bool SyntaxAnalysis::isQuatAction(string str)
{
    if (str[0] == '$' && str != "$")return true;
    else return false;
}

bool SyntaxAnalysis::isSymbolTableAction(string str)
{
    if (str[0] == '\"' && str != "\"")return true;
    else return false;
}

//用于分析文法，生成非终结符，终结符，右部集合
//求取First集，Follow集，Select集，判定文法是否为LL(1)文法
//若是，生成LL(1)分析表
//对所分析文法的格式要求：
//1.所有非终结符用<>括起来   2. 任意符号之间用空格分开
//3.每行第一个符号为产生式的非终结符，后面是右部，不需写->，右部之间用'|'分隔
//4.每行以'#'结尾
bool SyntaxAnalysis::LL1TableMachine(MainWindow* w)
{
    vector<vector<string>> grammar;   //文法右部，按顺序存储所有右部，不包含动作符号
    vector<vector<int>> rightList;   //右部列表，左部VNList[i]的所有右部序号存于rightList[i]
    vector<int> leftList;   //左部列表，右部grammar[i]的左部序号存于leftList[i]
    string temp;
    vector<string> tempRight, tempRightAll;
    fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\grammar.txt", fstream::in);

    while (File >> temp)
    {
        VNList.push_back(temp);
        rightList.push_back({});
        tempRight.clear();
        tempRightAll.clear();
        while (1)
        {
            File >> temp;
            if (temp == "|" || temp == "#")
            {
                grammar.push_back(tempRight);
                grammarRight.push_back(tempRightAll);
                (*(rightList.end() - 1)).push_back(grammar.size() - 1);
                leftList.push_back(VNList.size() - 1);
                if (temp == "|")
                {
                    tempRight.clear();
                    tempRightAll.clear();
                    continue;
                }
                else break;
            }
            else
            {
                if ((temp[0] != '<' && temp[0] != '\"' && temp[0] != '$') || temp == "<" || temp == "\"")
                {
                    if (find(VTList.begin(), VTList.end(), temp) == VTList.end())
                    VTList.push_back(temp);
                }
                if ((temp[0] != '\"' && temp[0] != '$') || temp == "\"") tempRight.push_back(temp);
                tempRightAll.push_back(temp);
            }
        }
    }
    VTList.push_back("<标识符>"); VTList.push_back("<无符号整数>");
    File.close();

    fstream File1("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\\\TextFile\\Symbol.txt", fstream::out);
    //输出非终结符列表
    File1 << "非终结符：" << endl;
    for (auto i = VNList.begin(); i != VNList.end(); i++)
        File1 << *i << ' ';
    File1 << '\n';
    //输出终结符列表
    File1 << endl << "终结符：" << endl;
    for (auto i = VTList.begin(); i != VTList.end(); i++)
        File1 << *i << ' ';
    File1 << '\n';
    //输出所有右部
    File1 << endl << "右部：" << endl;
    int n = 1;
    for (auto i = grammar.begin(); i != grammar.end(); i++)
    {
        File1 << n++ << ": ";
        for (auto j = i->begin(); j != i->end(); j++)
            File1 << *j << ' ';
        File1 << "\n";
    }
    File1.close();
    w->outInfor("终结符、非终结符已提取. . .");
    //cout << "终结符、非终结符已提取。" << endl;

    //求First集
    map<string, vector<string>> FIRST_VNVT;
    vector<vector<string>> FIRST(grammar.size());
    //终结符First集为本身
    for (auto i = VTList.begin(); i != VTList.end(); i++)
        FIRST_VNVT[*i] = { *i };
    //非终结符First集为<空>
    for (auto i = VNList.begin(); i != VNList.end(); i++)
        FIRST_VNVT[*i] = { };
    bool changFlag = true;
    while (changFlag == true) //当First集还在改变
    {
        changFlag = false;
        int m = 0;
        //对每一个产生式A->b
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            //b=b1b2b3..., bi是VN | VT, tempFIRST_VNVT <- First(b1) - {空}
            vector<string> tempFIRST_VNVT = FIRST_VNVT[*(j->begin())];
            auto nullItera = find(tempFIRST_VNVT.begin(), tempFIRST_VNVT.end(), "<空>");
            if (nullItera != tempFIRST_VNVT.end())tempFIRST_VNVT.erase(nullItera);
            int i = 1;
            //当First(b0...bi-1)包含空，即b0到bi-1都可为空，tempFIRST_VNVT += First(bi)
            while (find(FIRST_VNVT[(*j)[i-1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                   != FIRST_VNVT[(*j)[i - 1]].end()
                   && i <= (int)j->size() - 1)
            {
                for (auto k = FIRST_VNVT[(*j)[i]].begin(); k != FIRST_VNVT[(*j)[i]].end(); k++)
                    if (*k != "<空>") tempFIRST_VNVT.push_back(*k);
                i++;
            }
            //如果右部可推出空，tempFIRST_VNVT += <空>
            if (i == (int)j->size() - 1
                && find(FIRST_VNVT[(*j)[i - 1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                != FIRST_VNVT[(*j)[i - 1]].end())
                tempFIRST_VNVT.push_back("<空>");
            //如果右部=<空>，tempFIRST_VNVT <- <空>
            if (j->size() == 1 && *(j->begin()) == "<空>")
                tempFIRST_VNVT.push_back("<空>");
            //更新左部的First集
            for (auto k = tempFIRST_VNVT.begin(); k != tempFIRST_VNVT.end(); k++)
            {
                string tempVN = VNList[leftList[m]];
                if (find(FIRST_VNVT[tempVN].begin(), FIRST_VNVT[tempVN].end(), *k) == FIRST_VNVT[tempVN].end())
                {
                    FIRST_VNVT[tempVN].push_back(*k);
                    changFlag = true;
                }
            }
            //更新右部的First集
            for (auto k = tempFIRST_VNVT.begin(); k != tempFIRST_VNVT.end(); k++)
            {
                if (find(FIRST[m].begin(), FIRST[m].end(), *k) == FIRST[m].end())
                {
                    FIRST[m].push_back(*k);
                    changFlag = true;
                }
            }
        }
    }

    //输出First集
    fstream File2("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\First.txt", fstream::out);
    File2 << "First集：" << endl;
    int m = 1;
    for (auto i = FIRST.begin(); i != FIRST.end(); i++)
    {
        File2 << m++ <<": ";
        for (auto j = i->begin(); j != i->end(); j++)
            File2 << *j << " ";
        File2 << "\n";
    }
    File2.close();
    w->outInfor("First集已求取. . .");
    //cout << "First集已求取。" << endl;

    //求Follow集
    map<string, vector<string>> FOLLOW;
    //所有非终结符Follow集置<空>
    for (auto i = VNList.begin(); i != VNList.end(); i++)
        FOLLOW[*i] = { };
    //开始符号Follow集为"#"
    FOLLOW[*(VNList.begin())].push_back("#");
    //当Follow集还在改变
    changFlag = true;
    while (changFlag == true)
    {
        changFlag = false;
        int m = 0;
        //对每一个产生式A->b1b2...bi
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            //Trailer <- Follow(A)
            vector<string> Trailer = FOLLOW[VNList[leftList[m]]];
            //从后往前遍历产生式右部
            for (auto i = j->rbegin(); i != j->rend(); i++)
            {
                //如果bi是非终结符
                if (find(VNList.begin(), VNList.end(), *i) != VNList.end())
                {
                    //Follow(bi) += Trailer
                    for (auto k = Trailer.begin(); k != Trailer.end(); k++)
                    {
                        if (find(FOLLOW[*i].begin(), FOLLOW[*i].end(), *k) == FOLLOW[*i].end())
                        {
                            FOLLOW[*i].push_back(*k);
                            changFlag = true;
                        }
                    }
                    //如果First(bi)包含空，Trailer += First(bi)-"<空>"
                    if (find(FIRST_VNVT[*i].begin(), FIRST_VNVT[*i].end(), "<空>") != FIRST_VNVT[*i].end())
                    {
                        for (auto k = FIRST_VNVT[*i].begin(); k != FIRST_VNVT[*i].end(); k++)
                        {
                            if (*k != "<空>")
                            {
                                if (find(Trailer.begin(), Trailer.end(), *k) == Trailer.end())
                                {
                                    Trailer.push_back(*k);
                                }
                            }
                        }
                    }
                    //否则Trailer = First(bi)
                    else
                    {
                        Trailer = FIRST_VNVT[*i];
                    }
                }
                //如果bi是终结符，Trailer = First(bi)
                else
                {
                    Trailer = FIRST_VNVT[*i];
                }
            }
        }
    }

    //输出Follow集
    fstream File3("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\Follow.txt", fstream::out);
    File3 << "Follow集：" << endl;
    for (auto i = VNList.begin(); i != VNList.end(); i++)
    {
        File3 << *i << ": ";
        for (auto j = FOLLOW[*i].begin(); j != FOLLOW[*i].end(); j++)
            File3 << *j << ' ';
        File3 << '\n';
    }
    File3.close();
    w->outInfor("Follow集已求取. . .");
    //cout << "Follow集已求取。" << endl;

    //求Select集
    vector<vector<string>> SELECT(grammar.size());
    //对每个A->b
    for (int i = 0; i < (int)grammar.size(); i++)
    {
        //如果First(b)不包含空，即右部不能推出空，select集等于右部first集
        if (find(FIRST[i].begin(), FIRST[i].end(), "<空>") == FIRST[i].end())
            SELECT[i] = FIRST[i];
        //否则，select集等于右部first集并上左部follow集
        else
        {
            SELECT[i] = FOLLOW[VNList[leftList[i]]];
            for (auto j = FIRST[i].begin(); j != FIRST[i].end(); j++)
            {
                if (*j != "<空>")
                    if (find(SELECT[i].begin(), SELECT[i].end(), *j) == SELECT[i].end())
                        SELECT[i].push_back(*j);
            }
        }
    }

    //打印select集
    fstream File4("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\Select.txt", fstream::out);
    File4 << "Select集：" << endl;
    int v = 1;
    for (auto i = SELECT.begin(); i != SELECT.end(); i++)
    {
        File4 << v++ << ": ";
        for (auto j = i->begin(); j != i->end(); j++)
            File4 << *j << " ";
        File4 << "\n";
    }
    File4.close();
    w->outInfor("Select集已求取. . .");
    //cout << "Select集已求取。" << endl;

    //求是不是LL1文法
    for (int i = 0; i < (int)VNList.size(); i++)
    {
        vector<string> tempSelect;
        tempSelect.clear();
        for (auto j = rightList[i].begin(); j != rightList[i].end(); j++)
        {
            for (auto k = SELECT[*j].begin(); k != SELECT[*j].end(); k++)
            {
                if (find(tempSelect.begin(), tempSelect.end(), *k) == tempSelect.end())
                    tempSelect.push_back(*k);
                else
                {
                    w->outInfor("文法非LL(1)文法！请检查！\n提示信息：左部"+QString::fromStdString(VNList[i]));
                    return false;
                    //cout << "文法非LL(1)文法！请检查！\n" << "提示信息：左部" << VNList[i] << endl;
                    //exit(0);
                }
            }
        }
    }
    w->outInfor("文法满足LL(1)文法要求. . .");
    //cout << "文法满足LL(1)文法要求。" << endl;

    //求LL1分析表
    for (int i = 0; i < (int)VNList.size(); i++)
    {
        for (auto j = VTList.begin(); j != VTList.end(); j++)
            LL1Table[VNList[i]][*j] = -1;
        LL1Table[VNList[i]]["#"] = -1;
        for (auto x = rightList[i].begin(); x != rightList[i].end(); x++)
        {
            for (auto k = SELECT[*x].begin(); k != SELECT[*x].end(); k++)
            {
                if (find(VTList.begin(), VTList.end(), *k) != VTList.end()
                    || *k == "#")
                    LL1Table[VNList[i]][*k] = *x;
            }
        }
    }

    //打印LL(1)分析表
    fstream File5("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\LL1Table.txt", fstream::out);
    File5 << "LL(1)分析表" << '\t';
    for (auto i = VTList.begin(); i != VTList.end(); i++)
        File5 << *i << '\t' << '\t';
    File5 << "#" << '\t';
    File5 << '\n';
    for (auto i = VNList.begin(); i != VNList.end(); i++)
    {
        File5 << *i;
        for (int k = i->size(); k < 15; k++) File5 << ' ';
        File5 << '\t';
        for (auto j = VTList.begin(); j != VTList.end(); j++)
            File5 << LL1Table[*i][*j] << '\t' << '\t';
        File5 << LL1Table[*i]["#"] << '\t';
        File5 << '\n';
    }
    File5.close();
    w->outInfor("LL(1)分析表已生成. . .");
    //cout << "LL(1)分析表已生成。" << endl;

    return true;
}
