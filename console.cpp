#include "console.h"

Console::Console(QWidget* parent) : QTextBrowser(parent)
{
    //设置字体
    QFont font("Microsoft YaHei",9);
    font.setFixedPitch(true);
    this->setFont(font);

    //设置颜色
    QPalette p = this->palette();
    p.setColor(QPalette::Base, QColor(16,31,48));
    p.setColor(QPalette::Text, QColor(255,240,245));
    this->setPalette(p);
}
