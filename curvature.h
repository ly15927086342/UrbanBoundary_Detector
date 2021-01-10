#ifndef CURVATURE_H
#define CURVATURE_H
#include "all.h"

/*定义多项式曲度类，存次数，最大曲度和位置*/
class Curvature{
public:
    int polynomial;//次数
    vector<double> Maxcur;//曲度值
    vector<double> Pos;//位置
    Curvature(){
        this->polynomial = 0;
        this->Maxcur.resize(0);
        this->Pos.resize(0);
    }
    Curvature(int x1){
        this->polynomial = x1;
        this->Maxcur.resize(0);
        this->Pos.resize(0);
    }
    ~Curvature(){
        vector<double> a;
        a.swap(Maxcur);
        vector<double> b;
        b.swap(Pos);
        Maxcur.clear();
        Pos.clear();
    }
};
#endif // CURVATURE_H

