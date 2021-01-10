#ifndef DATAMODULE_H
#define DATAMODULE_H
#include "all.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "Shlwapi.h"
#include "cpl_string.h"
#include <atlstr.h>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <functional>
#include <iomanip>
#include "point.h"

class DataSource{
private:
    QString inputfilepath;//输入文件路径，Qt中改为Qstring
    QString outputfilepath;//输出文件路径，Qt中改为Qstring
    int buildvalue;//分类后的TIF图像中，建筑用地的值
    int Width;//图像宽（像素）
    int Height;//图像高（像素）
    int BandCount;//波段数
    int BuildPixel;//建筑所占的总像素数
    int TotalPixel;//图像总像素数
    int Byte;//字节数
    int Depth;//位深度
    const char*ProjRef;//图像坐标信息
    double Posi_UL_x;//图像左上角点的横向坐标
    double Posi_UL_y;//图像左上角点的纵向坐标
    double Pixel_Width;//像素横向分辨率
    double Pixel_Height;//像素纵向分辨率
public:
    short int* m_data;

    ~DataSource(){
        qDebug()<<"~DataSource";
        delete[]m_data;
        m_data = NULL;
        inputfilepath = nullptr;
        outputfilepath = nullptr;
        ProjRef = nullptr;
    }

    /*
     * 设置图像基本信息
     * width:图宽
     * height:图高
     * buildpixel:建筑总像素数
     * depth:位深度
     * byte:字节数
    ·* ref:头信息
     * x:左上角横坐标
     * y:左上角纵坐标
     * pw:像素横向分辨率
     * ph:像素纵向分辨率*/
    void SetImgInfo(int width, int height, int bandnum, int buildpixel, int depth, int byte, const char *ref, double x, double y, double pw, double ph);

    /*输出图像信息*/
    void PrintInfo();

    /* 读取输入路径，输出路径，建筑分类后的值 */
    void ReadBaseInfo(QString inpath, const int value);

    /* 读取TIF图像 */
    void ReadTif();

    /* 输出城市斑块tif */
    void WriteTif(short int*data, int width, int height,int buf,int num);

    /* 输出城市边界线shp */
    void WriteShp_line(list<list<Point>>&Result,int dis);

    /* 输出城市边界面shp */
    void WriteShp_polygon(list<list<Point>>&Result,int dis);

    /* 外部获取图像宽 */
    int GetWidth(){ return this->Width; }

    /* 外部获取图像高 */
    int GetHeight(){ return this->Height; }

    /* 外部获取图像分辨率 */
    int GetPixelLen(){ return this->Pixel_Width; }

};

#endif // DATAMODULE_H

