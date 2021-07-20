#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QDebug>
#include <QTextBlock>
#include "mainwindow.h"

class Console : public QTextBrowser
{
    Q_OBJECT

public:
    Console(QWidget* parent=0);
    MainWindow *parentWindow;

};

#endif
