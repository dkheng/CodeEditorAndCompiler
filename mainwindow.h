#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "highlighter.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void inputData(QString data);
    void outInfor(QString qstr);
    void outQuat(QString qstr);
    void outCode(QString qstr);
private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    void setUpHighlighter();

    //记录文件信息
    QString fileName;
    QString filePath;
    bool fileSaved;
    bool isRunning;
    void initFileData();
    bool firstLoad;

public slots:
    void changeSaveState();
    //工具栏菜单栏响应函数
    void newFile();
    void saveFile();
    void openFile();
    void undo();
    void redo();
    void run();
    void about();
protected:
    void resizeEvent(QResizeEvent* event)override;
    void closeEvent(QCloseEvent* event)override;
};

#endif // MAINWINDOW_H
