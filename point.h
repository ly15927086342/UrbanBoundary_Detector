#ifndef POINT_H
#define POINT_H
#include "all.h"

/*定义整型点类，用于存储像素点x,y坐标*/
class Point{
public:
    short int x;
    short int y;
    Point(){
        x = 0;
        y = 0;
    }
    Point(short int x1,short int y1) :x(x1), y(y1){}
    ~Point(){}
};
#endif // POINT_H

