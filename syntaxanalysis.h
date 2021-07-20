#ifndef SYNTAXANALYSIS_H
#define SYNTAXANALYSIS_H

#include <string>
#include "mainwindow.h"
#include "lexicalanalysis.h"

using namespace std;

class SyntaxAnalysis
{
public:
    bool analyse(string program, MainWindow* w);
private:
    bool LL1TableMachine(MainWindow* w);
    bool isVN(string str);
    bool isVT(string str);
    bool isSymbolTableAction(string str);
    bool isQuatAction(string str);
};

#endif // SYNTAXANALYSIS_H
