#include "mainwindow.h"
#include <QApplication>
//#include <QtConcurrent>
//#include <QVector>
//#include <QDebug>
//#include <QFuture>

int MapFunction(const int& num) {\
    return num+1;
}
int mappedReducedFunction(const int& num) {
    return num + 1;
}
void ReduceFunction(int& result, const int& item) {
    int t_r = result;
    result = item > result ? item : result;
    qDebug()<<t_r<<result<<item;
}

inline void EnableMemLeakCheck(){
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //设置默认字体
    QFont font;
    font.setFamily("times new roman");
    a.setFont(font);
    MainWindow w;
    w.show();

    EnableMemLeakCheck();

    return a.exec();
}
