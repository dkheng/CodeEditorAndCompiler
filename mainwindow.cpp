#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "syntaxanalysis.h"
#include "optimization.h"
#include "targetcode.h"
#include<QDebug>
#include<QFileDialog>
#include <QFile>
#include <QTextStream>
#include <string>

using namespace std;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    firstLoad=true;
    ui->setupUi(this);
    setUpHighlighter();

    //设置状态栏初始信息
    ui->statusBar->showMessage("Ready");

    //设置工具栏
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setFixedHeight(40);
    ui->mainToolBar->setContentsMargins(10,0,0,0);
    ui->mainToolBar->setStyleSheet("QToolButton:hover {background-color:cyan}"
                                   "QToolBar {border:none}");

    //设置窗口颜色
    QPalette windowPalette;
    windowPalette.setColor(QPalette::Window,QColor(19,44,51));
    windowPalette.setColor(QPalette::WindowText,QColor(255,240,245));
    this->setPalette(windowPalette);

    //设置四元式目标代码输出字体
    QFont font("Consolas",10);
    font.setFixedPitch(true);
    ui->QuadrupleText->setFont(font);
    ui->ObjectCodeText->setFont(font);

    //初始化当前文件信息
    initFileData();

    //Action连接
    connect(ui->actionNewFile,SIGNAL(triggered(bool)),this,SLOT(newFile()));
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openFile()));
    connect(ui->actionSave_File,SIGNAL(triggered(bool)),this,SLOT(saveFile()));
    connect(ui->actionUndo,SIGNAL(triggered(bool)),this,SLOT(undo()));
    connect(ui->actionRedo,SIGNAL(triggered(bool)),this,SLOT(redo()));
    connect(ui->editor,SIGNAL(textChanged()),this,SLOT(changeSaveState()));
    connect(ui->actionRun,SIGNAL(triggered(bool)),this,SLOT(run()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(about()));

    fileSaved=true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUpHighlighter()
{
    QFont font("Consolas",13);  //创建字体
    font.setFixedPitch(true);

    ui->editor->setFont(font);  //设置代码编辑器字体为Font

    QFontMetrics metrics(font);  //设置tab缩进长度
    ui->editor->setTabStopDistance(metrics.width(' ') * 4);

    highlighter=new Highlighter(ui->editor->document());
}

void MainWindow::initFileData()
{
    fileName = tr("Untitled.txt");
    filePath = tr("~/Desktop/Untitled.txt");
    fileSaved = true;
    isRunning = false;
}

void MainWindow::undo()
{
    ui->editor->undo();
}

void MainWindow::redo()
{
    ui->editor->redo();
}

void MainWindow::saveFile()
{
    QString savePath=QFileDialog::getSaveFileName(this,"选择保存路径与文件名",fileName,"*.txt");
    if(!savePath.isEmpty())
    {
        QFile out(savePath);
        out.open(QIODevice::WriteOnly|QIODevice::Text);
        QTextStream str(&out);
        str << ui->editor->toPlainText();
        out.close();
        fileSaved=true;
        QRegularExpression re("(?<=\\/)\\w+\\.txt");
        fileName=re.match(savePath).captured();
        filePath=savePath;
        this->setWindowTitle(tr("HJ Editor - ")+fileName);
    }
}
void MainWindow::newFile()
{
    MainWindow *newWindow=new MainWindow();
    QRect newPos=this->geometry();
    newWindow->setGeometry(newPos.x()+10,newPos.y()+10,newPos.width(),newPos.height());
    newWindow->show();
}

void MainWindow::openFile()
{
    if(!fileSaved)
    {
        if(QMessageBox::question(this, "文件未保存", "当前文件未保存，是否保存？", QMessageBox::Yes, QMessageBox::No)
           == QMessageBox::Yes)
        saveFile();
    }
    QString openPath = QFileDialog::getOpenFileName(this, "选择要打开的文件", filePath, "*.txt");
    if(!openPath.isEmpty())
    {
        QFile in(openPath);
        in.open(QIODevice::ReadOnly|QIODevice::Text);
        QTextStream str(&in);
        ui->editor->setPlainText(str.readAll());
        QRegularExpression re("(?<=\\/)\\w+\\.txt");
        fileName = re.match(openPath).captured();
        this->setWindowTitle("HJ Editor - " + fileName);
        filePath = openPath;
        fileSaved = true;
    }
}

void MainWindow::changeSaveState()
{
    if(firstLoad&&fileSaved)
    {
        this->setWindowTitle(tr("HJ Editor - ")+fileName);
        firstLoad=false;
        return;
    }
    fileSaved=false;
    this->setWindowTitle(tr("HJ Editor - ")+fileName+tr("*"));
}

void MainWindow::run()
{
    if(isRunning)
    {
        QMessageBox::critical(this, "正在编译", "编译器正在运行，请稍作等待！",QMessageBox::Yes);
        return;
    }
    isRunning = true;
    ui->statusBar->showMessage("正在编译...");
    ui->outputText->clear();
    ui->QuadrupleText->clear();
    ui->ObjectCodeText->clear();

    //编译
    QString Qprogram = ui->editor->toPlainText();
    string program = Qprogram.toStdString() + "#";
    SyntaxAnalysis s;
    if(s.analyse(program, this)==true)
    {
        Opt o;
        o.run(this);
        TargetCode tarCode;
        tarCode.clear();
        tarCode.scan(this);
    }
    isRunning=false;
    ui->statusBar->showMessage("Ready");
}

void MainWindow::outInfor(QString qstr)
{
    if(ui->outputText->toPlainText()=="")
        ui->outputText->setPlainText(ui->outputText->toPlainText() + qstr);
    else
        ui->outputText->setPlainText(ui->outputText->toPlainText() + "\n" + qstr);
}

void MainWindow::outQuat(QString qstr)
{
    ui->QuadrupleText->setPlainText(ui->QuadrupleText->toPlainText() + qstr);
}

void MainWindow::outCode(QString qstr)
{
    if(ui->ObjectCodeText->toPlainText()=="")
        ui->ObjectCodeText->setPlainText(ui->ObjectCodeText->toPlainText() + qstr);
    else
        ui->ObjectCodeText->setPlainText(ui->ObjectCodeText->toPlainText() + "\n" + qstr);
}

//当窗口大小变化时，修改部件大小使之始终与窗口吻合
//该函数窗口大小发生变化时自动调用
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    ui->editor->setGeometry(10, 0, (width() - 40) / 7 * 5,
                            (height() - ui->statusBar->height() - ui->mainToolBar->height() - 41) / 5 * 4 );
    ui->outputText->setGeometry(10, ui->editor->height() + 10, (width() - 40) / 7 * 5,
                                (height() - ui->statusBar->height() - ui->mainToolBar->height() - 41) / 5 * 1);
    ui->QuadrupleText->setGeometry(20 + ui->editor->width(), 0, (width() - 40) / 7 * 1,
                                   height() - ui->statusBar->height() - ui->mainToolBar->height() - 31);
    ui->ObjectCodeText->setGeometry(30 + ui->editor->width() + ui->QuadrupleText->width(), 0, (width() - 40) / 7 * 1,
                                    height() - ui->statusBar->height() - ui->mainToolBar->height() - 31);
}

//关闭程序时检测文件是否已保存，若未保存给出提示
//该函数程序关闭时自动调用
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if(!fileSaved){
        if(QMessageBox::question(this,"文件未保存", "当前文件未保存，是否保存？\n直接退出会导致文件改动丢失！",QMessageBox::Yes,QMessageBox::No)
           == QMessageBox::Yes)
            saveFile();
        fileSaved=true;
    }
}

//“关于”信息
void MainWindow::about()
{
    QMessageBox::information(this,"关于"," Code Editor v1.0 ",QMessageBox::Ok);
}
