#include "findcontours.h"

/* a到bc的距离 */
double Distance(Point a, Point b, Point c){
    int x0 = a.x; int y0 = a.y;
    int x1 = b.x; int y1 = b.y;
    int x2 = c.x; int y2 = c.y;
    return fabs(((x0 - x1)*(y1 - y2) + (y0 - y1)*(x2 - x1)) / sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)));
}

void FindContours::findcontours(){
    for (short int i = 0; i < Height; i++){
        for (short int j = 0; j < Width; j++){
            if (isChecked[i*Width + j] == 0){
                continue;
            }

            list<Point>outline;//轮廓线
            //边界点
            //轮廓追踪
            int dir = 5;
            Point pbegin;
            Point cpoint;//当前点

            //找左上角点

            cpoint.x = j;
            cpoint.y = i;
            pbegin.x = j;
            pbegin.y = i;
            outline.push_back(cpoint);
            //isChecked[i*Width + j] = 1;

            for (int n = 0; n < 4; n++){
                Point nextp = Boutline(cpoint, dir);
                if (nextp.y < 0 || nextp.y >= Height || nextp.x < 0 || nextp.x >= Width){
                    dir = (dir + 1) % 8;
                    continue;
                }
                if (isChecked[nextp.y*Width + nextp.x] == 1){
                    outline.push_back(nextp);
                    //isChecked[nextp.y*Width + nextp.x] = 1;
                    cpoint = nextp;
                    break;
                }
                dir = (dir + 1) % 8;
            }

            while (cpoint.x != pbegin.x || cpoint.y != pbegin.y){
                //更新dir初始位置
                if (dir % 2 == 0){//偶
                    dir = (dir + 7) % 8;
                }
                else{//奇
                    dir = (dir + 6) % 8;
                }
                for (;; dir = (dir + 1) % 8){
                    Point nextp = Boutline(cpoint, dir);
                    if (nextp.y < 0 || nextp.y >= Height || nextp.x < 0 || nextp.x >= Width){
                        continue;
                    }
                    if (isChecked[nextp.y*Width + nextp.x] == 1){
                        outline.push_back(nextp);
                        //isChecked[nextp.y*Width + nextp.x] = 1;
                        cpoint = nextp;
                        break;
                    }
                }
            }

            Black(pbegin.x, pbegin.y);
            Points.push_back(outline);
            outline.clear();//轮廓线
        }
    }

    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;

    if (Threshold <= 1){
        /*for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
            for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
                DP_Points.push_back(*itor2);
            }
        }*/
        DP_Points.swap(Points);
    }
    else{
        int num = 0;
        for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
            DP.resize((*itor1).size());
            location.resize((*itor1).size(), 0);
            vector<Point>tmp;
            vector<int>tmp1;
            num = 0;
            for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
                DP[num] = (*itor2);//存入vector
                num++;
            }

            //轮廓分两段
            Douglas_Peucker(0, (int)((*itor1).size() / 2));
            Douglas_Peucker((int)((*itor1).size() / 2), (*itor1).size() - 1);
            location[0] = location[(int)((*itor1).size() / 2)] = location[(*itor1).size() - 1] = 1;

            list<Point>OK;
            for (int i = 0; i < (*itor1).size(); i++){
                if (location[i] == 0)
                    continue;
                OK.push_back(DP[i]);
            }
            DP_Points.push_back(OK);
            OK.clear();

            tmp.swap(DP);
            tmp1.swap(location);
        }
    }


}

Point FindContours::Boutline(Point cp, int dir){//返回值为类，可以调用类的复制构造函数，或者函数的参数有类，也可以调用
    short int x = 0, y = 0;
    switch (dir)
    {
    case 0:	x = cp.x + 1; y = cp.y; break;
    case 1:	x = cp.x + 1; y = cp.y - 1; break;
    case 2:	x = cp.x; y = cp.y - 1; break;
    case 3:	x = cp.x - 1; y = cp.y - 1; break;
    case 4:	x = cp.x - 1; y = cp.y; break;
    case 5:	x = cp.x - 1; y = cp.y + 1; break;
    case 6:	x = cp.x; y = cp.y + 1; break;
    case 7:	x = cp.x + 1; y = cp.y + 1; break;
    default:break;
    }
    Point nextp(x, y);
    return nextp;
}

void FindContours::Black(short int j, short int i){
    stack<Point> pointstack;//存储待生长的种子

    int dx[8] = { -1, 0, 1, 0, -1, -1, 1, 1 };
    int dy[8] = { 0, -1, 0, 1, -1, 1, -1, 1 };

    short int current_x, current_y;//当前正在处理的中心像素点
    short int lin_x, lin_y;//邻域待判断像素点

    Point p(j, i);
    pointstack.push(p);

    while (pointstack.size() > 0){
        current_x = pointstack.top().x;
        current_y = pointstack.top().y;
        pointstack.pop();
        isChecked[current_y*Width + current_x] = 0;

        for (int k = 0; k < 8; k++){
            lin_x = current_x + dx[k];
            lin_y = current_y + dy[k];
            //判断是否超界
            if ((lin_x < Width && lin_x >= 0) && (lin_y < Height && lin_y >= 0)){
                if (isChecked[lin_y*Width + lin_x] == 1){//若点无归属，则压入栈
                    Point p(lin_x, lin_y);
                    pointstack.push(p);
                    isChecked[lin_y*Width + lin_x] = 0;//点设置为有归属，不再判断该点
                }
            }
        }
    }
}

void FindContours::Douglas_Peucker(int begin, int end){
    int mark = 0;
    double distance = 0.0;
    double max_dis = 0.0;
    for (int i = begin + 1; i < end; i++){
        distance = Distance(DP[i], DP[begin], DP[end]);
        if (distance>max_dis){
            max_dis = distance;
            mark = i;
        }
    }

    if (max_dis > Threshold){
        location[mark] = 1;
        Douglas_Peucker(begin, mark);
        Douglas_Peucker(mark, end);
    }
}

void FindContours::runFC(){
    DS.ReadBaseInfo(Path,Mark);
    DS.ReadTif();
    sendProgress(50);
    sendStatus(QString::fromLocal8Bit("开始计算矢量边界..."));
    ReadInfo(DS.GetWidth(),DS.GetHeight(),DS.m_data,Mark,Threshold);
    findcontours();
    sendProgress(90);
    sendStatus(QString::fromLocal8Bit("计算完毕"));
    DS.WriteShp_line(DP_Points,Threshold);
    DS.WriteShp_polygon(DP_Points,Threshold);
    sendProgress(100);
    sendStatus(QString::fromLocal8Bit("输出完毕"));
    cout<<"输出完毕"<<endl;
    emit workFinished();
}
