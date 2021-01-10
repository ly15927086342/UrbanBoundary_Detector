#include "bufferdilationmodule.h"

bool comp(const list<Point> &t1, const list<Point> &t2){
    return t1.size() > t2.size();
}

void BufferDilation::SetInfo(int width, int height,int pixel, short int* &isChecked1, QString path){
    Width = width;
    Height = height;
    Pixel_len = pixel;
    isChecked = new short int[width*height];
    for (int i = 0; i < width*height; i++){
        isChecked[i] = isChecked1[i];
    }
    outputPath = path;
}

list<Point> BufferDilation::areaGrow(short int seedx, short int seedy, short int *&output){
    stack<Point> pointstack;//存储待生长的种子
    list<Point> pointlist;//存储区域生长范围内的像素点

    short int current_x, current_y;//当前正在处理的中心像素点
    short int lin_x, lin_y;//邻域待判断像素点

    Point p(seedx, seedy);
    pointstack.push(p);
    pointlist.push_back(p);

    while (pointstack.size() > 0){
        current_x = pointstack.top().x;
        current_y = pointstack.top().y;
        pointstack.pop();
        output[current_y*Width + current_x] = 0;

        for (int k = 0; k < typeLin; k++){
            lin_x = current_x + dx[k];
            lin_y = current_y + dy[k];
            //判断是否超界
            if ((lin_x < Width && lin_x >= 0) && (lin_y < Height && lin_y >= 0)){
                if (output[lin_y*Width + lin_x] == 1){//若点无归属，则压入栈
                    Point p(lin_x, lin_y);
                    pointstack.push(p);
                    pointlist.push_back(p);//压入链表

                    output[lin_y*Width + lin_x] = 0;//点设置为有归属，不再判断该点
                }
            }
        }
    }
    return pointlist;
}


void BufferDilation::seedGrowth(short int *&output){
    int count = 0;
    //按顺序找种子点
    for (int i = 0; i < Height; i++){
        for (int j = 0; j < Width; j++){
            if (output[i*Width + j] != 1){
                continue;
            }
            Rank_size.push_back(areaGrow(j, i, output));
            count++;
        }
    }
}

void BufferDilation::AreaGrow(short int seedx, short int seedy, short int *&output){
    stack<Point> pointstack;//存储待生长的种子

    short int current_x, current_y;//当前正在处理的中心像素点
    short int lin_x, lin_y;//邻域待判断像素点

    Point p(seedx, seedy);
    pointstack.push(p);
    //pointlist.push_back(p);

    while (pointstack.size() > 0){
        current_x = pointstack.top().x;
        current_y = pointstack.top().y;
        pointstack.pop();
        output[current_y*Width + current_x] = 0;

        for (int k = 0; k < typeLin; k++){
            lin_x = current_x + dx[k];
            lin_y = current_y + dy[k];
            //判断是否超界
            if ((lin_x < Width && lin_x >= 0) && (lin_y < Height && lin_y >= 0)){
                if (output[lin_y*Width + lin_x] == 1){//若点无归属，则压入栈
                    Point p(lin_x, lin_y);
                    pointstack.push(p);
                    //pointlist.push_back(p);//压入链表

                    output[lin_y*Width + lin_x] = 0;//点设置为有归属，不再判断该点
                }
            }
        }
    }
}


int BufferDilation::SeedGrowth(short int * &output){
    int count = 0;
    //按顺序找种子点
    for (int i = 0; i < Height; i++){
        for (int j = 0; j < Width; j++){
            if (output[i*Width + j] != 1){
                continue;
            }
            AreaGrow(j, i, output);
            count++;
        }
    }
    return count;
}

void BufferDilation::Black(short int j, short int i, short int * &hasCheck){
    stack<Point> pointstack;//存储待生长的种子

    short int current_x, current_y;//当前正在处理的中心像素点
    short int lin_x, lin_y;//邻域待判断像素点

    Point p(j, i);
    pointstack.push(p);

    while (pointstack.size() > 0){
        current_x = pointstack.top().x;
        current_y = pointstack.top().y;
        pointstack.pop();
        hasCheck[current_y*Width + current_x] = 0;

        for (int k = 0; k < typeLin; k++){
            lin_x = current_x + dx[k];
            lin_y = current_y + dy[k];
            //判断是否超界
            if ((lin_x < Width && lin_x >= 0) && (lin_y < Height && lin_y >= 0)){
                if (hasCheck[lin_y*Width + lin_x] == 1){//若点无归属，则压入栈
                    Point p(lin_x, lin_y);
                    pointstack.push(p);
                    hasCheck[lin_y*Width + lin_x] = 0;//点设置为有归属，不再判断该点
                }
            }
        }
    }
}




int BufferDilation::ParFunc(int buf, short int * &output){
    short int *utput = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        utput[i] = output[i];
    }
    short int buffer_x = 0, buffer_y = 0;
    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;
    //生成缓冲区图像
    for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            short int x = (*itor2).x;
            short int y = (*itor2).y;

            for (short int j = -buf; j <= buf; j++){
                buffer_x = x + j;
                if (buffer_x < 0 || buffer_x >= Width){
                    continue;
                }
                int max_y = (int)(sqrt(buf*buf - j*j));
                for (short int q = -max_y; q <= max_y; q++){
                    buffer_y = y + q;
                    if (buffer_y < 0 || buffer_y >= Height){
                        continue;
                    }
                    else{
                        utput[buffer_y*Width + buffer_x] = 1;
                    }
                }
            }
        }
    }

    int num = SeedGrowth(utput);
    delete[]utput;
    utput = nullptr;
    return num;
}

void BufferDilation::Minkowski_Dilation(){
    double start_t, end_t, cost;
    int prog = 6;

    Output = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        Output[i] = isChecked[i];
    }
    cout << "开始边界" << endl;
    sendStatus(QString::fromLocal8Bit("生成边界开始..."));
    FindContours(this->isChecked);//生成斑块边界
    sendStatus(QString::fromLocal8Bit("生成边界完毕..."));
    sendProgress(prog);

    cout << "结束" << endl;

    array<int, Par_num> buffer_cur;

    this->begin_num += this->Pixel_len;//如果是0，则从分辨率开始，如果不是0，则从下一个分辨率开始
    int time = 0;
    int count = 10;
    sendStatus(QString::fromLocal8Bit("缓冲算法开始..."));
    while (count != 1){//串行
//        concurrent_vector<int>result;
        int result = 0;
        start_t = clock();

        cout<<Par_num<<endl;

        QString tip = QString::fromLocal8Bit("线程数：") + QString::number(Par_num) + QString::fromLocal8Bit("；缓冲半径：");
        for (int i = 0; i < Par_num; i++){//并行
            buffer_cur[i] = this->begin_num / this->Pixel_len;
            tip += QString::number(this->begin_num);
            tip += QString::fromLocal8Bit("米;");
            this->begin_num += this->Pixel_len;
        }

        sendStatus(tip);

        cout << "par begin..." << endl;

//        parallel_for_each(begin(buffer_cur), end(buffer_cur), [&](int buf) {
//            //缓冲,统计,保存结果
//            int res = ParFunc(buf, Output);
//            result.push_back(res);
//            cout << ++time << endl;
//        });

        for(int i = 0;i<Par_num;i++){
            int res = ParFunc(buffer_cur[i], Output);
//            result.push_back(res);
            result = res;
            cout << ++time << endl;
        }


        end_t = clock();
        cost = (end_t - start_t) / 1000;
        cout << cost << "秒" << endl;
        sendStatus(QString::fromLocal8Bit("该线程结束，用时")+QString::number(cost)+QString::fromLocal8Bit("秒"));
        //并行结果排序
//        sort(begin(result), end(result));

//        count = result[0];//最小的值
        count = result;
        cout << count << endl;
        sendStatus(QString::fromLocal8Bit("最少斑块数：")+QString::number(count));

        for (int i = 1; i <= Par_num; i++){
            ofstream outf;
            string file = (outputPath + "_count.txt").toLocal8Bit().data();
            outf.open(file.c_str(), ios::app | ios::out);
//            outf << buffer_cur[i-1]*this->Pixel_len << "," << result[Par_num - i] << endl;
            outf << buffer_cur[i-1]*this->Pixel_len << "," << result << endl;
            outf.close();
//            if(result[Par_num - i] == 1){//输出1即停止
//                break;
//            }
            if(result == 1){
                break;
            }
        }
        sendStatus(QString::fromLocal8Bit("写入文件..."));
        prog += 1;
        sendProgress(prog);

//        result.clear();
    }
}

void BufferDilation::FindContours(short int * &hasCheck){
    for (short int i = 0; i < Height; i++){
        for (short int j = 0; j < Width; j++){
            if (hasCheck[i*Width + j] == 0){
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
            //hasCheck[i*Width + j] = 1;

            for (int n = 0; n < 4; n++){
                Point nextp = Boutline(cpoint, dir);
                if (nextp.y < 0 || nextp.y >= Height || nextp.x < 0 || nextp.x >= Width){
                    dir = (dir + 1) % 8;
                    continue;
                }
                if (hasCheck[nextp.y*Width + nextp.x] == 1){
                    outline.push_back(nextp);
                    //hasCheck[nextp.y*Width + nextp.x] = 1;
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
                    if (hasCheck[nextp.y*Width + nextp.x] == 1){
                        outline.push_back(nextp);
                        //hasCheck[nextp.y*Width + nextp.x] = 1;
                        cpoint = nextp;
                        break;
                    }
                }
            }
            Black(pbegin.x, pbegin.y, hasCheck);
            Points.push_back(outline);
            outline.clear();//轮廓线
        }
    }
}

Point BufferDilation::Boutline(Point cp, int dir){//返回值为类，可以调用类的复制构造函数，或者函数的参数有类，也可以调用
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

void BufferDilation::OutputBoundary(DataSource &ds){
    cout << "ouputboundary" << endl;
    short int buf = (int)((threshold / this->Pixel_len) + 0.5);//四舍五入
//    short int buf = (int)((threshold) / this->Pixel_len) + 1;//进一
    short int buffer_x = 0, buffer_y = 0;
    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;

    Output = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        Output[i] = isChecked[i];
    }

    Points.clear();
    FindContours(this->isChecked);

    //生成缓冲区图像
    for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            short int x = (*itor2).x;
            short int y = (*itor2).y;

            for (short int j = -buf; j <= buf; j++){
                buffer_x = x + j;
                if (buffer_x < 0 || buffer_x >= Width){
                    continue;
                }
                int max_y = (int)(sqrt(buf*buf - j*j));
                for (short int q = -max_y; q <= max_y; q++){
                    buffer_y = y + q;
                    if (buffer_y < 0 || buffer_y >= Height){
                        continue;
                    }
                    else{
                        Output[buffer_y*Width + buffer_x] = 1;
                    }
                }
            }
        }
    }

    short int *Output_Cur = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        Output_Cur[i] = Output[i];
    }

    Points.clear();
    FindContours(Output_Cur);//找到缓冲后的板块边界

    cout << "first" << endl;


    //Output颠倒像素
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        if(Output[i] != 1){
            Output[i] = 1;
        }else{
            Output[i] = 0;
        }
    }

    cout << "second" << endl;

    int buf_inner = buf - 1;

    //反向缓冲，腐蚀效果
    //腐蚀的半径要-1
    for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            short int x = (*itor2).x;
            short int y = (*itor2).y;

            for (short int j = -buf_inner; j <= buf_inner; j++){
                buffer_x = x + j;
                if (buffer_x < 0 || buffer_x >= Width){
                    continue;
                }
                int max_y = (int)(sqrt(buf_inner*buf_inner - j*j));
                for (short int q = -max_y; q <= max_y; q++){
                    buffer_y = y + q;
                    if (buffer_y < 0 || buffer_y >= Height){
                        continue;
                    }
                    else{
                        Output[buffer_y*Width + buffer_x] = 1;
                    }
                }
            }
        }
    }

    cout << "third" << endl;

    //Output颠倒像素
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        if(Output[i] != 1){
            Output[i] = 1;
        }else{
            Output[i] = 0;
        }
    }

    cout << "fourth" << endl;

    sendProgress(50);
    seedGrowth(Output);
    sendProgress(90);

    Rank_size.sort(comp);
    list<list<Point>>::iterator tior;

    cout << "fifth" << endl;

    /*获取第n大的斑块*/
    tior = Rank_size.begin();
    for (int i = 0; i < Outputnum; i++){
        //if (i == 2){//第几块斑块
        for (itor2 = (*tior).begin(); itor2 != (*tior).end(); itor2++){
            isChecked[(*itor2).y*Width + (*itor2).x] = 1;
        }
        //}
        tior++;
    }

    cout << "sixth" << endl;

    /*绘制输出tif*/
    ds.WriteTif(isChecked, Width, Height, buf, Outputnum);

    delete[]Output;
    Output = nullptr;
}

void BufferDilation::Rank_sizeCal(){
    short int buf = (int)((threshold / this->Pixel_len) + 0.5);//四舍五入
//    short int buf = (int)((threshold) / this->Pixel_len) + 1;//进一
    short int buffer_x = 0, buffer_y = 0;
    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;

    Output = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        Output[i] = isChecked[i];
    }

    sendProgress(10);

    Points.clear();
    FindContours(this->isChecked);

    //生成缓冲区图像
    for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            short int x = (*itor2).x;
            short int y = (*itor2).y;

            for (short int j = -buf; j <= buf; j++){
                buffer_x = x + j;
                if (buffer_x < 0 || buffer_x >= Width){
                    continue;
                }
                int max_y = (int)(sqrt(buf*buf - j*j));
                for (short int q = -max_y; q <= max_y; q++){
                    buffer_y = y + q;
                    if (buffer_y < 0 || buffer_y >= Height){
                        continue;
                    }
                    else{
                        Output[buffer_y*Width + buffer_x] = 1;
                    }
                }
            }
        }
    }

    short int *Output_Cur = new short int[Height*Width];
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        Output_Cur[i] = Output[i];
    }

    Points.clear();
    FindContours(Output_Cur);//找到缓冲后的板块边界

    cout << "first" << endl;


    //Output颠倒像素
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        if(Output[i] != 1){
            Output[i] = 1;
        }else{
            Output[i] = 0;
        }
    }

    cout << "second" << endl;

    int buf_inner = buf - 1;

    //反向缓冲，腐蚀效果
    //腐蚀的半径要-1
    for (itor1 = Points.begin(); itor1 != Points.end(); itor1++){
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            short int x = (*itor2).x;
            short int y = (*itor2).y;

            for (short int j = -buf_inner; j <= buf_inner; j++){
                buffer_x = x + j;
                if (buffer_x < 0 || buffer_x >= Width){
                    continue;
                }
                int max_y = (int)(sqrt(buf_inner*buf_inner - j*j));
                for (short int q = -max_y; q <= max_y; q++){
                    buffer_y = y + q;
                    if (buffer_y < 0 || buffer_y >= Height){
                        continue;
                    }
                    else{
                        Output[buffer_y*Width + buffer_x] = 1;
                    }
                }
            }
        }
    }

    cout << "third" << endl;

    //Output颠倒像素
    for (int i = 0; i < Height*Width; i++){//FindContours()会改变ischecked，用Output存ischecked
        if(Output[i] != 1){
            Output[i] = 1;
        }else{
            Output[i] = 0;
        }
    }

    cout << "fourth" << endl;

    sendProgress(50);

    seedGrowth(Output);

    sendProgress(90);

    Rank_size.sort(comp);
    list<list<Point>>::iterator tior;

    ofstream outf;
    outf.open((outputPath + "_rank_size_" + QString::number(buf) + ".txt").toLocal8Bit().data(), ios::out);
    for (tior = Rank_size.begin(); tior != Rank_size.end(); tior++){
        outf << (*tior).size() << ",";
    }
    outf.close();

    cout << "write in rank_size" << endl;
    sendStatus(QString::fromLocal8Bit("完毕."));
    sendProgress(100);
    sendStatus(QString::fromLocal8Bit("已输出等级规模分布统计文件rank_size.txt"));
    emit drawrs(outputPath + "_rank_size_" + QString::number(buf) + ".txt");   
}

void BufferDilation::runMin(){
    DS.ReadBaseInfo(TifPath, BuildPixel);//输入输出文件路径，建筑像素值
    sendStatus(QString::fromLocal8Bit("读取tif完毕..."));
    qDebug()<<TifPath;

//    QFile file(rspath);
//    file.open(QIODevice::ReadOnly | QIODevice::Text);
//    QByteArray t = file.readAll();
//    QStringList lines = QString(t).split(',');
//    lines.pop_back();
//    file.close();

    DS.ReadTif();//读取tif
    DS.PrintInfo();//输出图像信息，可省略/

    SetInfo(DS.GetWidth(), DS.GetHeight(), DS.GetPixelLen(), DS.m_data, TifPath.split(".tif").at(0));

    QString cfile = this->TifPath;
    cfile.replace(cfile.indexOf(".tif"),4,"_count.txt");

    //判断从哪里开始缓冲，如果上次没算完，可以继续算
    QFile file(cfile);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QByteArray t = file.readAll();
        QStringList lines = QString(t).split('\n');
        QString res = lines[lines.length()-2];//最后一行是空，倒数第二行是最后一组数据
        qDebug()<<lines.length();
        qDebug()<<res;
        this->begin_num = res.split(',')[0].toInt();
        qDebug()<<this->begin_num;
    }else{
        this->begin_num = 0;//-像素开始，这样会从半径0开始计算
    }
    file.close();
    qDebug()<<this->begin_num;

    this->Minkowski_Dilation();

    sendStatus(QString::fromLocal8Bit("运算结束！"));
    sendProgress(100);
    sendStatus(QString::fromLocal8Bit("统计完成，输出path_count.txt文件"));
    emit workFinished();
}

void BufferDilation::runRank(){
    DS.ReadBaseInfo(TifPath, BuildPixel);//输入输出文件路径，建筑像素值
    sendStatus(QString::fromLocal8Bit("读取tif完毕..."));

    DS.ReadTif();//读取tif
    DS.PrintInfo();//输出图像信息，可省略/

    SetInfo(DS.GetWidth(), DS.GetHeight(), DS.GetPixelLen(), DS.m_data, TifPath.split(".tif").at(0));
    Rank_sizeCal();
    emit workFinished();
}

void BufferDilation::runOutput(){
    DS.ReadBaseInfo(TifPath, BuildPixel);//输入输出文件路径，建筑像素值
    sendStatus(QString::fromLocal8Bit("读取tif完毕..."));

    DS.ReadTif();//读取tif
    DS.PrintInfo();//输出图像信息，可省略/

    SetInfo(DS.GetWidth(), DS.GetHeight(), DS.GetPixelLen(), DS.m_data, TifPath.split(".tif").at(0));
    OutputBoundary(DS);

    sendStatus(QString::fromLocal8Bit("完毕."));
    sendProgress(100);
    sendStatus(QString::fromLocal8Bit("已输出城市集群斑块栅格图像path.tif"));
    emit workFinished();
}
