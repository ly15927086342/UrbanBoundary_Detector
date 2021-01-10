#include "fractalmodule.h"

void Fractal::Readpath(QString path){
    qDebug()<<path;
    QStringList a = path.split("_count_interp.txt");
    inputDilationPath = a.at(0);
    qDebug()<<inputDilationPath;

    ifstream inf;
    string line;
    ofstream of1;
    ofstream of2;
    of1.open((inputDilationPath + "_r.txt").toLocal8Bit().data(), ios::out);
    of2.open((inputDilationPath + "_n.txt").toLocal8Bit().data(), ios::out);
    inf.open((inputDilationPath + "_count_interp.txt").toLocal8Bit().data(), ios::in);
    Minkowski_data.resize(2);


    while (getline(inf, line)){
        vector<double>v(0);
        SplitString(line, v, ",");
        of1 << v[0] << ",";
        of2 << v[1] << ",";
        v[0] = log10(v[0]);
        v[1] = log10(v[1]);
        Minkowski_data[0].push_back(v[0]);
        Minkowski_data[1].push_back(v[1]);
    }
    of1.close();
    of2.close();
    inf.close();
}

void Fractal::SplitString(const string& s, vector<double>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        v.push_back(atof(s.substr(pos1, pos2 - pos1).c_str()));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(atof(s.substr(pos1).c_str()));
}

void Fractal::FractalProcess(){
    vector<vector<double>>A(14, vector<double>(14));//存储多项式的系数
    vector<double>BIC(13);//存储多项式的BIC
    vector<double>R2(13);
    vector<int>BIC_rank(13);//存储BIC从小到大的排序

    double xmin = (Minkowski_data[0][Minkowski_data[0].size() - 1] - Minkowski_data[0][0])*0.2 + Minkowski_data[0][0];
    double xmax = Minkowski_data[0][Minkowski_data[0].size() - 1] - (Minkowski_data[0][Minkowski_data[0].size() - 1] - Minkowski_data[0][0])*0.1;

    double total = 0.0;
    double meany = 0.0;
    vector<vector<double>>nihe(2);//用于拟合的散点
    for (int i = 0; i < Minkowski_data[0].size(); i++){
        nihe[0].push_back(Minkowski_data[0][i]);
        nihe[1].push_back(Minkowski_data[1][i]);
        meany += Minkowski_data[1][i];
    }
    meany = meany / Minkowski_data[0].size();
    cout << "拟合数据量 " << nihe[0].size() << endl;



    //求最小二乘拟合多项式系数，最高13次多项式，公式为B*a0=C，a0=inv(B)*C
    for (int n = 1; n <= 13; n++){
        MatrixXd a0 = MatrixXd::Zero(n + 1, 1);//多项式系数
        MatrixXd B = MatrixXd::Zero(n + 1, n + 1);//算式左边系数矩阵，对称正定矩阵
//        MatrixXd BT = MatrixXd::Zero(n + 1, n + 1);//B的转置矩阵
        MatrixXd C = MatrixXd::Zero(n + 1, 1);//算式右边矩阵
//        MatrixXd B_NI = MatrixXd::Zero(n + 1, n + 1);//系数矩阵逆矩阵

        for (int i = 0; i <= n; i++){
            for (int j = 0; j <= n; j++){
                double sum = 0;
                for (int k = 0; k < nihe[0].size(); k++){
                    sum += pow(nihe[0][k], j + i);
                }
                //	B.write(i, j, sum);
                B(i, j) = sum;
            }
            double sy = 0;
            for (int k = 0; k < nihe[0].size(); k++){
                sy += pow(nihe[0][k], i)*nihe[1][k];
            }
            //	C.write(i, 0, sy);
            C(i, 0) = sy;
        }
        a0 = B.fullPivHouseholderQr().solve(C);//B*a0=C类型矩阵计算a0

        vector<double>V0(nihe[0].size());//拟合的残差
        //	double ave = 0;
        for (int i = 0; i <= n; i++){
            A[n][i] = a0(i, 0);//多项式系数存入A
        }

        //求BIC
        for (int i = 0; i < nihe[0].size(); i++){
            double Y_nihe = 0;
            for (int j = 0; j <= n; j++){
                Y_nihe += a0(j, 0)*pow(nihe[0][i], j);
            }
            V0[i] = Y_nihe - nihe[1][i];
            total += pow(nihe[1][i] - meany, 2);//SST总离差平方和
            //ave += V0[i];
        }
        //ave = ave / nihe[0].size();
        double thegma_2 = 0;//残差方差平方和 SSE
        for (int i = 0; i < nihe[0].size(); i++){
            thegma_2 += pow(V0[i], 2);

        }
        R2[n - 1] = (total - thegma_2) / total;//R2 = SSR/SST = (SST-SSE)/SST
        thegma_2 = thegma_2 / nihe[0].size();
        BIC[n - 1] =/* nihe[0].size()*log(2 * PI) +*/ nihe[0].size()*log(thegma_2) /*+ nihe[0].size()*/ + (n + 1)*log(nihe[0].size());//BIC公式，用于衡量函数拟合的准确性和简洁性，参考http://www.a-site.cn/article/834868.html
    }

    ofstream of3;
    of3.open((inputDilationPath + "_bic.txt").toLocal8Bit().data(), ios::out);

    cout << "多项式计算完毕" << endl;

    double BIC_mina = 1000;
    double BIC_mark = 0;
    int BIC_rak = 0;
    double BIC_r2 = 0;

    for (int i = 0; i < 13; i++){
        BIC_rank[i] = i + 1;
    }
    for (int i = 0; i < 13; i++)//选择1-13次多项式中BIC值最小的多项式作为拟合函数
    {
        for (int j = i + 1; j < 13; j++){
            if (BIC[i] > BIC[j])
            {
                BIC_mina = BIC[j];
                BIC[j] = BIC[i];
                BIC[i] = BIC_mina;
                BIC_rak = BIC_rank[j];
                BIC_rank[j] = BIC_rank[i];
                BIC_rank[i] = BIC_rak;
                BIC_r2 = R2[j];
                R2[j] = R2[i];
                R2[i] = BIC_r2;
            }
        }
    }

    cout << "BIC计算结果：" << endl;
    for (int i = 0; i < 13; i++){
        cout << "排名:" << i + 1 << " " << BIC_rank[i] << "次多项式 " << "BIC值:" << BIC[i] << " R2值:" << R2[i] << endl;
        of3 << "排名:" << i + 1 << " " << BIC_rank[i] << "次多项式 " << "BIC值:" << BIC[i] << " R2值:" << R2[i] << endl;
    }

    of3 << endl;

    BIC_mark = BIC_rank[0];

    //BIC_mark = 5;

    cout << "BIC最小的次数" << BIC_mark << endl;

    for (int i = 0; i <= BIC_mark; i++){
        cout << "xishu:" << i << ":" << setprecision(8) << A[BIC_mark][i] << endl;
    }

    for (int i = 0; i <= 13; i++){
        of3 << "y=";
        for (int j = 0; j < i; j++){
            of3 << "(" << A[i][j] << ")" << "*x.^" << j << "+";
        }
        of3 << "(" << A[i][i] << ")" << "*x.^" << i << endl;
    }

    of3 << endl;
    of3 << "取值范围最小值:" << xmin << endl;
    of3 << "取值范围最大值:" << xmax << endl;
    of3 << endl;


    for (int bic = 1; bic <= 13; bic++){
        //curvature of dilation curve曲度计算
        double presize = 0.001;
        int cur_count = (int)((nihe[0][nihe[0].size() - 1] - nihe[0][0]) / presize);//曲线上每隔0.01计算一次曲度
        cout << "曲度上的点数 " << cur_count << endl;
        vector<vector<double>>curvature(cur_count, vector<double>(2));//存曲度
        for (int k = 0; k < cur_count; k++){
            curvature[k][0] = k*presize + nihe[0][0];
            double y_2 = 0;
            double y_1 = 0;
            for (int j = 0; j <= bic; j++)
            {
                if (j > 1)
                {
                    y_2 += j * (j - 1) * A[bic][j] * pow(curvature[k][0], j - 2);
                }
                if (j > 0)
                {
                    y_1 += j * A[bic][j] * pow(curvature[k][0], j - 1);
                }
            }
            curvature[k][1] = y_2 / pow((1 + y_1 * y_1), 1.5);
        }
        cout << "曲度计算完毕" << endl;

        of3 << bic << "次多项式\n";
        for (int i = 1; i < cur_count - 1; i++)//找出曲度最大的点
        {
            if ((curvature[i][1] - curvature[i - 1][1]) * (curvature[i][1] - curvature[i + 1][1]) > 0)
            {
                of3 << "最大曲度位置:" << curvature[i][0] << ";最大曲度值:" << curvature[i][1] << endl;
                cout << curvature[i][0] << ":" << curvature[i][1] << endl;
            }
        }
        of3 << endl;
    }
    of3.close();
}
