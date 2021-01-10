#include "cubic_hermite_interp.h"

void Hermite::setInfo(QString path, double gp){
    this->inputPath = path;
    this->gap = gp;
}

void Hermite::Cubic_Hermite(){
    QFile file(inputPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = file.readAll();
    QStringList lines = QString(t).split('\n');
    lines.pop_back();
    file.close();
    int len = lines.length();
    QVector<QVector<double>> RawPoints;
    for(int i=0;i<len;i++){
        QVector<double> cur;
        cur.push_back(log10(lines[i].split(',')[0].toDouble()));//radius
        cur.push_back(log10(lines[i].split(',')[1].toDouble()));//count
        RawPoints.push_back(cur);
    }
    //给采样点添加斜率
    for(int i=1;i<len-1;i++){//第二到倒数第二个点
        double x1 = RawPoints[i-1][0];
        double y1 = RawPoints[i-1][1];
        double x2 = RawPoints[i+1][0];
        double y2 = RawPoints[i+1][1];
        double x0 = RawPoints[i][0];
        double y0 = RawPoints[i][1];
        double k = 0.0;
        double d10 = sqrt(pow((x1-x0),2)+pow((y1-y0),2));
        double d02 = sqrt(pow((x2-x0),2)+pow((y2-y0),2));
        double k1 = (y1-y0)/(x1-x0);
        double k2 = (y2-y0)/(x2-x0);
        double sum = d10 + d02;
        k = k1*(sum-d10)/sum + k2*(sum-d02)/sum;
        RawPoints[i].push_back(k);
        if(i == 1){//第一个点
            RawPoints[0].push_back(k1);
        }
        if(i == len-2){//最后一个点
            RawPoints[len-1].push_back(k2);
        }
    }
    vector<double> InterPoints;
    vector<double> res;
    //生成插值点，保留首尾点
    double dis = 10;
    InterPoints.push_back(RawPoints[0][0]);
    while(dis > this->gap){
        double last = InterPoints[InterPoints.size()-1];
        InterPoints.push_back(last + this->gap);
        dis = RawPoints[len-1][0] - InterPoints[InterPoints.size()-1];
    }
    InterPoints.push_back(RawPoints[RawPoints.size()-1][0]);
    qDebug()<<InterPoints.size();
    //迭代找出区间
    for(int i = 0;i<InterPoints.size();i++){
        int left = 0;
        int right = RawPoints.size()-1;
        double value = InterPoints[i];
        while(right>= 0 && RawPoints[right][0] >= value){
            right--;
        }
        while(left <= RawPoints.size()-1 && RawPoints[left][0] <= value){
            left++;
        }
        right++;
        left--;
        qDebug()<<right<<','<<left;

        if(RawPoints[right][0] == value){
            res.push_back(RawPoints[right][1]);
        }else if(RawPoints[left][0] == value){
            res.push_back(RawPoints[left][1]);
        }else{
            //Hermite插值算法
            double x0 = RawPoints[left][0];
            double y0 = RawPoints[left][1];
            double y0_ = RawPoints[left][2];
            double x1 = RawPoints[right][0];
            double y1 = RawPoints[right][1];
            double y1_ = RawPoints[right][2];
            double x00 = InterPoints[i];
            double a0 = (x1 - 3*x0 + 2*x00)*pow(x1 - x00,2)/pow(x1 - x0,3);
            double a1 = (3*x1 - x0 - 2*x00)*pow(x00 - x0,2)/pow(x1 - x0,3);
            double b0 = (x00 - x0)*pow(x00 - x1,2)/pow(x1 - x0,2);
            double b1 = pow(x00 - x0,2)*(x00 - x1)/pow(x1 - x0,2);
            double y00 = y0*a0 + y1*a1 + y0_*b0 + y1_*b1;
            res.push_back(y00);
        }
    }
    QString outPath = this->inputPath;
    outPath.replace(outPath.indexOf("_count"),6,"_count_interp");
    QFile outputFile(outPath);
    if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtOutput(&outputFile);
        for(int i=0;i<InterPoints.size();i++){
            txtOutput << QString::number(pow(10,InterPoints[i]))+','+QString::number(pow(10,res[i]))<<endl;
        }
    }
    outputFile.flush();
    outputFile.close();
}
