#ifndef BUFFERDILATIONMODULE_H
#define BUFFERDILATIONMODULE_H
#include <Windows.h>
#include <ppl.h>
#include <concurrent_vector.h>
#include <array>
#include "all.h"
#include "point.h"
#include "datamodule.h"
#include <time.h>
#include <algorithm>
#include <QString>
#include <QDebug>
#include <QObject>
//#include <QtConcurrent>
//#include <QVector>
//#include <QFuture>
using namespace Concurrency;

#define Par_num 1 //线程数

class BufferDilation : public QObject{

    Q_OBJECT

private:
    int Width;//图像宽
    int Height;//图像高
    int Pixel_len;//分辨率
    short int *isChecked;
    short int *Output;
    list<list<Point>>Points;//存放区域生长得到的区域
    list<list<Point>>Rank_size;
    QString outputPath = "";
    string tifName = "";
    int begin_num = 0;
    double threshold;
    int Outputnum = 0;
    QString TifPath = "";
    int BuildPixel = 1;
    DataSource DS;
    vector<int>dx;
    vector<int>dy;
    int typeLin = 4;

public:
//    BufferDilation(QObject* parent = 0);
    ~BufferDilation(){
        qDebug()<<"~Bufferdilation";
        Points.clear();
        Rank_size.clear();
        delete[]isChecked;
        isChecked = nullptr;
        delete[]Output;
        Output = nullptr;
        vector<int>a;
        vector<int>b;
        a.swap(dx);
        b.swap(dy);
        dx.clear();
        dy.clear();
    }

    int mapfunctor(const int &buf,short *&Output);

    void seedGrowth(short int *&output);

    void SetThreshold(double thre){
        threshold = thre;
    }

    void SetOutputParm(int num){
        Outputnum = num;
    }

    void setMinParm(QString path,int bp,int type){
        TifPath = path;
        BuildPixel = bp;
        qDebug()<<TifPath;
        qDebug()<<BuildPixel;
        qDebug()<<begin_num;
        typeLin = type;
        if(type == 4){//4邻域
            dx.push_back(-1);
            dx.push_back(0);
            dx.push_back(1);
            dx.push_back(0);
            dy.push_back(0);
            dy.push_back(-1);
            dy.push_back(0);
            dy.push_back(1);
        }else if(type == 8){//8邻域
            dx.push_back(-1);
            dx.push_back(0);
            dx.push_back(1);
            dx.push_back(0);
            dx.push_back(-1);
            dx.push_back(-1);
            dx.push_back(1);
            dx.push_back(1);
            dy.push_back(0);
            dy.push_back(-1);
            dy.push_back(0);
            dy.push_back(1);
            dy.push_back(-1);
            dy.push_back(1);
            dy.push_back(-1);
            dy.push_back(1);
        }
    }

    /*输入图像的宽、高以及图像的归属数组
     * width:宽
     * height:高
     * isChecked1:归属数组*/
    void SetInfo(int width, int height,int pixel, short int* &isChecked1, QString path);

    /*
     * 输入种子的x,y坐标，输出区域生长的链表
     * seedx:种子的横坐标
     * seedy:种子的纵坐标 */
    list<Point> areaGrow(short int seedx, short int seedy, short int *&output);
    void AreaGrow(short int seedx, short int seedy, short int *&output);

    /*
     * 整幅图像的区域生长函数
     * minpixelcount:城区的最少像素数 */
    int SeedGrowth(short int *&output);

    /*
     * 二值图像轮廓提取和轮廓追踪，用于计算建筑周长，计算斑块形状指数patch shape index */
    void FindContours(short int * &hasCheck);

    int ParFunc(int buf, short int * &output);

    Point Boutline(Point cp, int dir);

    void Black(short int j,short int i, short int * &hasCheck);

    /*输出城市边界shp*/
    void OutputBoundary(DataSource &ds);

    /*输出count.txt*/
    void Minkowski_Dilation();

    /*输出rank-size.txt*/
    void Rank_sizeCal();

public slots://槽
    //设置状态
    void sendStatus(QString text){
        emit setStatus(text);
    }
    //设置进度
    void sendProgress(int value){
        emit setProgress(value);
    }
    //触发析构的槽
    void deleteBD(){
        this->~BufferDilation();
    }
    void runMin();
    void runRank();
    void runOutput();

signals://信号
    void workFinished();
    void setStatus(QString text);
    void setProgress(int value);
    void drawrs(QString path);
};

#endif // BUFFERDILATIONMODULE_H

