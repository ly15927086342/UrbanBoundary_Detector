#ifndef FRACTALMODULE_H
#define FRACTALMODULE_H
#include "all.h"
#include <Eigen3/Eigen/Dense>
#include "cpl_string.h"
#include <atlstr.h>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <functional>
#include <iomanip>

using namespace Eigen;
#define PI 3.1415926535897932384626
#define e 2.718281828

class Fractal{
private:
    vector<vector<double>>Minkowski_data;//存储统计得到半径和数量的双对数散点
    QString inputDilationPath = "";

public:
    ~Fractal(){
        vector<vector<double>> a;
        a.swap(Minkowski_data);
        Minkowski_data.clear();
        cout<<"~fractal"<<endl;
    }
    void Readpath(QString path);
    /*
     * 分形算法实现
     * area:读取的面积数组
     * V:缓冲区半径的增长间隔，单位为米
     * areafield:建筑的面积字段*/
    void FractalProcess();

    void SplitString(const string& s, vector<double>& v, const string& c);
};
#endif // FRACTALMODULE_H

