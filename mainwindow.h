#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QProgressBar>
#include <QFileDialog>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QThread>
#include <QLabel>
#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel * BICmodel;
    QLabel *statuslabel;
    QProgressBar *progress;
    int pageIndex = 0;//标签页索引
    double Threshold = 0.0;//距离临界
    //设置状态
    void setStatus(QString text){
        statuslabel->setText(text);
    }
    //设置进度
    void setProgress(int value){
        progress->setValue(value);
    }
    void drawDilationCurve(QVector<double> r,QVector<double> n);
    void drawBicCurve(QVector<double> x,QVector<double> y,int pos);//添加数据
    void drawRankSizeCurve(QVector<double> x,QVector<double> y);
    void init_BicCurve();//初始化图
    void init_RankSizeCurve();
    void init_DilationCurve();
    void init_BIC_model();


private slots:
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_8_clicked();

    //自定义槽函数
    void slot_setStatus(QString text);
    void slot_setProgress(int value);
    void slot_drawrs(QString rspath);
    void hide_progress();
    void urban_finish();
    void on_tabWidget_currentChanged(int index);
    void on_dl_dilation_clicked();
    void on_dl_rank_clicked();
    void on_dl_bic_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
};

#endif // MAINWINDOW_H
