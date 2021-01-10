#ifndef FINDCONTOURS_H
#define FINDCONTOURS_H
#include "all.h"
#include "point.h"
#include "datamodule.h"

class FindContours : public QObject{

    Q_OBJECT

private:
    int Width;//图像宽
    int Height;//图像高
    int Mark;//目标像素
    int Threshold;//道格拉斯扑克距离临界
    short int *isChecked;
    list<list<Point>>Points;//存放轮廓点坐标
    vector<Point>DP;//存每个轮廓
    vector<int>location;//标记轮廓离散化后的点的位置
    QString Path;
    DataSource DS;

public:
    list<list<Point>>DP_Points;//存放DP后的轮廓点坐标

    ~FindContours(){
        cout<<"~findcontours"<<endl;
        delete[]isChecked;
        isChecked = NULL;
        Points.clear();
        vector<Point>tmp2;
        vector<int>tmp3;
        tmp2.swap(DP);
        tmp3.swap(location);
        DP_Points.clear();
    }

    /*
     * width:图像宽
     * height:图像长
     * isChecked1:存储图像的指针
     * mark:目标像素值*/
    void ReadInfo(int width, int height, short int*isChecked1, int mark,int threshold){
        this->Width = width;
        this->Height = height;
        isChecked = new short int[width*height];
        for (int i = 0; i < width*height; i++){
            isChecked[i] = isChecked1[i];
        }
        this->Mark = mark;
        this->Threshold = threshold;
    }

    /* 二值图像轮廓提取和轮廓追踪 */
    void findcontours();

    /* 判断当前点的该方向是不是周长点 */
    Point Boutline(Point cp, int dir);

    /* 将找出周长的图斑变为0 */
    void Black(short int j, short int i);



    /* 曲线离散化算法 */
    void Douglas_Peucker(int begin, int end);

    /* 设置参数 */
    void SetParam(QString path,int dis,int value){
        Mark = value;
        Threshold = dis;
        Path = path;
    }

public slots://槽
    void runFC();
    //设置状态
    void sendStatus(QString text){
        emit setStatus(text);
    }
    //设置进度
    void sendProgress(int value){
        emit setProgress(value);
    }
    //触发析构的槽
    void deleteFC(){
        this->~FindContours();
    }

signals://信号
    void workFinished();
    void setStatus(QString text);
    void setProgress(int value);
};

#endif // FINDCONTOURS_H

