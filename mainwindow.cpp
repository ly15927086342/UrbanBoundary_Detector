#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fractalmodule.h"
#include "curvature.h"
#include "bufferdilationmodule.h"
#include "findcontours.h"
#include "cubic_hermite_interp.h"
#include "agglomerativeclustering.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置软件logo
    setWindowIcon(QIcon(":/new/prefix1/logo.png"));

    //固定软件窗口，不可缩放
    this->setFixedSize( this->width (),this->height ());

    //默认四邻域
    ui->fourtype->setChecked(true);

    //初始化当前标签页
    pageIndex = 0;
    ui->tabWidget->setCurrentIndex(pageIndex);

    //初始化BIC表格
    this->BICmodel = new QStandardItemModel;
    ui->tableView->setModel(BICmodel);
    init_BIC_model();

    //初始化状态栏和进度条
    statuslabel = new QLabel();
    progress = new QProgressBar();
    QLabel *statuslabeltitle = new QLabel();
    ui->statusBar->addWidget(statuslabeltitle,0);
    ui->statusBar->addWidget(statuslabel,1);
    ui->statusBar->addWidget(progress,2);
    statuslabeltitle->setText(QString::fromLocal8Bit("状态："));
    statuslabel->setText(QString::fromLocal8Bit("无任务"));
    statuslabel->setMinimumWidth(100);
    progress->setFixedHeight(20);
    progress->setVisible(false);
    progress->setValue(0);
    progress->setMaximumWidth(300);
    progress->setRange(0,100);

    //初始化图表
    init_DilationCurve();
    init_BicCurve();
    init_RankSizeCurve();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********初始化图表***********/
//初始化膨胀曲线图
void MainWindow::init_DilationCurve(){
    ui->dilationcurve->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ui->dilationcurve, "Dilation Curve", QFont("times new roman", 12, QFont::Bold));
    ui->dilationcurve->plotLayout()->addElement(0, 0, title);

    ui->dilationcurve->xAxis->setLabel("Width of the Dilation Buffer(m)");
    ui->dilationcurve->yAxis->setLabel("Number of Built-up Clusters");
}

//初始化BIC图
void MainWindow::init_BicCurve(){
    ui->biccurve->plotLayout()->insertRow(0);
    QCPTextElement *title2 = new QCPTextElement(ui->biccurve, "The BIC for Polynomial Estimations of Dilation Curve", QFont("times new roman", 12, QFont::Bold));
    ui->biccurve->plotLayout()->addElement(0, 0, title2);

    ui->biccurve->xAxis->setLabel("Polynomial Degree of the Estimation Curve");
    ui->biccurve->yAxis->setLabel("Value of the BIC");

    ui->biccurve->xAxis->setRange(0,14);
}

//初始化等级规模分布图
void MainWindow::init_RankSizeCurve(){
    ui->rankcurve->plotLayout()->insertRow(0);
    QCPTextElement *title3 = new QCPTextElement(ui->rankcurve, "Rank-Size Distribution of Built-up Clusters", QFont("times new roman", 12, QFont::Bold));
    ui->rankcurve->plotLayout()->addElement(0, 0, title3);

    ui->rankcurve->xAxis->setLabel("Rank of Built-up Clusters");
    ui->rankcurve->yAxis->setLabel("Area of Built-up Clusters");
}

//初始化BIC表格
void MainWindow::init_BIC_model(){
    this->BICmodel->clear();//清空表格内容
    this->BICmodel->setHorizontalHeaderItem(0, new QStandardItem(QString::fromLocal8Bit("次数")));
    this->BICmodel->setHorizontalHeaderItem(1, new QStandardItem(QString::fromLocal8Bit("BIC")));
    this->BICmodel->setHorizontalHeaderItem(2, new QStandardItem(QString::fromLocal8Bit("R2")));
    this->BICmodel->setHorizontalHeaderItem(3, new QStandardItem(QString::fromLocal8Bit("选中位置")));
    this->BICmodel->setHorizontalHeaderItem(4, new QStandardItem(QString::fromLocal8Bit("临界半径")));
    this->BICmodel->setHorizontalHeaderItem(5, new QStandardItem(QString::fromLocal8Bit("函数表达式")));
    this->BICmodel->setHorizontalHeaderItem(6, new QStandardItem(QString::fromLocal8Bit("最大曲度及位置")));

    //setColumnWidth设置每一列的宽度
    this->ui->tableView->setColumnWidth(0, 70); //设置列的宽度
    this->ui->tableView->setColumnWidth(1, 70); //设置列的宽度
    this->ui->tableView->setColumnWidth(2, 70); //设置列的宽度
    this->ui->tableView->setColumnWidth(3, 70); //设置列的宽度
    this->ui->tableView->setColumnWidth(4, 70); //设置列的宽度
    this->ui->tableView->setColumnWidth(5, 500); //设置列的宽度
    this->ui->tableView->setColumnWidth(6, 200); //设置列的宽度

    BICmodel->setRowCount(13);
}
/***********END***********/

/************绘制图表**********/
//绘制膨胀曲线图
void MainWindow::drawDilationCurve(QVector<double> r,QVector<double>n){
    ui->dilationcurve->clearGraphs();
    ui->dilationcurve->clearItems();
    ui->dilationcurve->addGraph();
    QVector<QCPGraphData> *mData;
    mData = ui->dilationcurve->graph(0)->data()->coreData();
    mData->clear();//数据清空
    ui->dilationcurve->graph(0)->setData(r, n);
    ui->dilationcurve->graph(0)->rescaleAxes();

    ui->dilationcurve->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->dilationcurve->xAxis->setScaleType(QCPAxis::stLogarithmic);

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    //    ui->dilationcurve->axisRect()->setupFullAxesBox();
    //    ui->dilationcurve->legend->setVisible(true);//图例
    ui->dilationcurve->yAxis->setTicker(logTicker);
    ui->dilationcurve->xAxis->setTicker(logTicker);

    //    ui->dilationcurve->graph(0)->setName(ui->yearinput->text());
    ui->dilationcurve->graph(0)->setLineStyle((QCPGraph::LineStyle)(QCPGraph::lsNone));
    ui->dilationcurve->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

    ui->dilationcurve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->dilationcurve->yAxis->scaleRange(1.15, ui->dilationcurve->yAxis->range().center());
    ui->dilationcurve->xAxis->scaleRange(1.15, ui->dilationcurve->xAxis->range().center());
    ui->dilationcurve->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->dilationcurve->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

    ui->dilationcurve->replot();
}

//绘制BIC曲线
void MainWindow::drawBicCurve(QVector<double> x,QVector<double> y,int pos){
    ui->biccurve->clearGraphs();
    ui->biccurve->clearItems();
    ui->biccurve->addGraph();
    QVector<QCPGraphData> *mData;
    mData = ui->biccurve->graph(0)->data()->coreData();
    mData->clear();//数据清空
    ui->biccurve->graph(0)->setData(x, y);
    ui->biccurve->graph(0)->rescaleAxes();
    ui->biccurve->yAxis->scaleRange(1.15, ui->biccurve->yAxis->range().center());
    ui->biccurve->xAxis->scaleRange(1.15, ui->biccurve->xAxis->range().center());

    ui->biccurve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    qDebug()<<QString::fromLocal8Bit("bic curve out");

    // add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):
    QCPItemTracer *phaseTracer = new QCPItemTracer(ui->biccurve);
    //    itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
    phaseTracer->setGraph(ui->biccurve->graph(0));
    phaseTracer->setGraphKey(pos);
    phaseTracer->setInterpolating(true);
    phaseTracer->setStyle(QCPItemTracer::tsCircle);
    phaseTracer->setPen(QPen(Qt::red));
    phaseTracer->setBrush(Qt::red);
    phaseTracer->setSize(7);
    ui->biccurve->replot();
}

//绘制等级规模分布图
void MainWindow::drawRankSizeCurve(QVector<double> x,QVector<double> y){
    ui->rankcurve->clearGraphs();
    ui->rankcurve->clearItems();
    ui->rankcurve->addGraph();
    QVector<QCPGraphData> *mData;
    mData = ui->rankcurve->graph(0)->data()->coreData();
    mData->clear();//数据清空
    ui->rankcurve->graph(0)->setData(x, y);
    ui->rankcurve->graph(0)->rescaleAxes();

    ui->rankcurve->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->rankcurve->xAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->rankcurve->yAxis->scaleRange(1.15, ui->rankcurve->yAxis->range().center());
    ui->rankcurve->xAxis->scaleRange(1.15, ui->rankcurve->xAxis->range().center());
    ui->rankcurve->yAxis->setTicker(logTicker);
    ui->rankcurve->xAxis->setTicker(logTicker);
    ui->rankcurve->graph(0)->setLineStyle((QCPGraph::LineStyle)(QCPGraph::lsNone));
    ui->rankcurve->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    ui->rankcurve->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->rankcurve->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

    ui->rankcurve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->rankcurve->replot();
}
/**********END*************/

/*********自定义槽函数*********/
//设置状态
void MainWindow::slot_setStatus(QString text){
    setStatus(text);
}

//设置进度
void MainWindow::slot_setProgress(int value){
    setProgress(value);
}

//绘制等级规模分布图
void MainWindow::slot_drawrs(QString rspath){
    QFile file(rspath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = file.readAll();
    QStringList lines = QString(t).split(',');
    lines.pop_back();
    file.close();
    int len = lines.length();
    QVector<double> rank(len),size(len);
    for(int i=0;i<len;i++){
        rank[i] = i+1;
        size[i] = lines[i].toInt();
    }
    drawRankSizeCurve(rank,size);
    // 调用HAC.py计算分类结果

//   AgglomerativeClustering HAC;
//   HAC.setInfo(rspath);
//   int count = HAC.calClusters();
//   cout<<"count:"<<count<<endl;
//   HAC.~AgglomerativeClustering();
}

//隐藏进度条，翻页，设置按钮可点击
void MainWindow::hide_progress(){//minkowski complete...
    progress->setVisible(false);
    ui->tabWidget->setCurrentIndex(++pageIndex);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
}

//隐藏进度条
void MainWindow::urban_finish(){//minkowski complete...
    progress->setVisible(false);
}

//下载膨胀曲线图
void MainWindow::on_dl_dilation_clicked()
{
    QFile file("dilationcurve.png");
    if(file.exists()){
        file.remove();
    }
    progress->setVisible(true);
    setProgress(0);
    ui->dilationcurve->savePng("dilationcurve.png",600,400,1.0,-1,96);
    setProgress(100);
    setStatus(QString::fromLocal8Bit("已下载图片(dilationcurve.png)!"));
}

//下载等级规模分布图
void MainWindow::on_dl_rank_clicked()
{
    QFile file("ranksizedistribution.png");
    if(file.exists()){
        file.remove();
    }
    progress->setVisible(true);
    setProgress(0);
    ui->rankcurve->savePng("ranksizedistribution.png",600,400,1.0,-1,96);
    setProgress(100);
    setStatus(QString::fromLocal8Bit("已下载图片(ranksizedistribution.png)!"));
}

//下载bic图
void MainWindow::on_dl_bic_clicked()
{
    QFile file("bic.png");
    if(file.exists()){
        file.remove();
    }
    progress->setVisible(true);
    setProgress(0);
    ui->biccurve->savePng("bic.png",600,400,1.0,-1,96);
    setProgress(100);
    setStatus(QString::fromLocal8Bit("已下载图片(bic.png)!"));
}

//标签页改变，给全局变量pageindex赋值
void MainWindow::on_tabWidget_currentChanged(int index)
{
    pageIndex = index;
}
/**********END**********/

/***********功能函数*************/
//组合位置和曲度信息为一个字符串
QString MakeCurOutput(vector<double> pos,vector<double> cur){
    QString res = "";
    for(int i=0;i<pos.size();i++){
        res += "(" + QString::fromLocal8Bit("位置:")+ QString::number(pos[i]) + ";" + QString::fromLocal8Bit("曲度:")+ QString::number(cur[i]) + ")";
    }
    return res;
}

//获取多项式最大曲度的位置对应的索引
int FindMaxPos(vector<double> &A,vector<double> &pos,double Min,double Max){
    int max = 0;
    double AMax = 0;
    if(int(A.size()) == 0){
        return -1;
    }
    for(int i=0;i<A.size();i++){
        if(pos[i] >= Min && pos[i] <= Max){
            if(fabs(A[i]) > AMax){
                max = i;
                AMax = fabs(A[i]);
            }
        }
    }
    if(AMax > 0){
        return max;
    }else{
        return -1;
    }
}
/*************END***************/

/*******上一步*********/
void MainWindow::on_pushButton_4_clicked()
{
    ui->tabWidget->setCurrentIndex(--pageIndex);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->tabWidget->setCurrentIndex(--pageIndex);
}

void MainWindow::on_pushButton_14_clicked()
{
    ui->tabWidget->setCurrentIndex(--pageIndex);
}
/********END********/

/*********下一步**********/
void MainWindow::on_pushButton_3_clicked()
{
    ui->tabWidget->setCurrentIndex(++pageIndex);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->tabWidget->setCurrentIndex(++pageIndex);
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->tabWidget->setCurrentIndex(++pageIndex);
}

void MainWindow::on_pushButton_19_clicked()
{
    ui->tabWidget->setCurrentIndex(--pageIndex);
}

void MainWindow::on_pushButton_18_clicked()
{
    ui->tabWidget->setCurrentIndex(++pageIndex);
}
/*******END******/

/******选择文件函数*****/
//选择建设用地栅格图文件
void MainWindow::on_pushButton_clicked()
{
    QString TifPath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择建设用地栅格图"),"", tr("(*.tif);;all(*.*)"));
    qDebug()<<TifPath;
    ui->input_tifpath->setText(TifPath);
}

//选择_count.txt文件生成_count_interp.txt
void MainWindow::on_pushButton_16_clicked()
{
    QString countPath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择统计文件"),"", tr("(*_count.txt);;all(*.*)"));
    if(countPath == ""){
        setStatus(QString::fromLocal8Bit("未选择count文件！"));
        return;
    }

    qDebug()<<countPath;
    ui->urbanpath_input->setText(countPath);
}

//选择count.txt文件生成bic.txt
void MainWindow::on_pushButton_5_clicked()
{
    QString countPath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择插值后的统计文件"),"", tr("count(*.txt);;all(*.*)"));
    if(countPath == ""){
        setStatus(QString::fromLocal8Bit("未选择count文件！"));
        return;
    }

    qDebug()<<countPath;
    ui->count_Path->setText(countPath);

    Fractal F_module;
    F_module.Readpath(countPath);
    F_module.FractalProcess();
    setProgress(100);
    setStatus(QString::fromLocal8Bit("多项式拟合完成，输出path_bic.txt"));
}

//选择城市斑块栅格图路径
void MainWindow::on_pushButton_15_clicked()//选择城市斑块栅格图像路径
{
    QString urbanpath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择城市斑块栅格图"),"", tr("bic(*.tif);;all(*.*)"));
    if(urbanpath == ""){
        setStatus(QString::fromLocal8Bit("未选择栅格图！"));
        return;
    }
    ui->urbanpath->setText(urbanpath);
}

//选择ranksize.txt文件
void MainWindow::on_pushButton_10_clicked()
{
    QString rsPath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择rank_size统计文件"),"", tr("rank_size(*.txt);;all(*.*)"));
    if(rsPath == ""){
        setStatus(QString::fromLocal8Bit("未选择rank_size文件！"));
        return;
    }

    ui->rspath->setText(rsPath);
}

//选择bic.txt文件
void MainWindow::on_pushButton_12_clicked()
{
    QString bicPath = QFileDialog::getOpenFileName(NULL,QString::fromLocal8Bit("选择bic统计文件"),"", tr("bic(*.txt);;all(*.*)"));
    if(bicPath == ""){
        setStatus(QString::fromLocal8Bit("未选择bic文件！"));
        return;
    }

    ui->bicpath->setText(bicPath);
}
/*********END*********/

/*********核心算法调用函数*********/
//threshold calculate and rank size step
void MainWindow::on_pushButton_9_clicked()
{
    init_BIC_model();
    QString bicPath = ui->bicpath->text();
    QString pppth = bicPath.split("_bic.txt").at(0);

    QFile filer(pppth + "_r.txt");
    filer.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = filer.readAll();
    QStringList lines = QString(t).split(',');
    lines.pop_back();
    filer.close();

    QFile filen(pppth + "_n.txt");
    filen.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray tn = filen.readAll();
    QStringList linesn = QString(tn).split(',');
    linesn.pop_back();
    filen.close();

    int len = lines.length();
    QVector<double> r(len),n(len);
    for(int i=0;i<len;i++){
        r[i] = lines[i].toDouble();
        n[i] = linesn[i].toDouble();
    }
    drawDilationCurve(r,n);

    double threshold = 0.0;
    ifstream inf;
    string line;
    inf.open(bicPath.toLocal8Bit().data(), ios::in);
    int module = 0;//0:bic值和R2，1：多项式，2：阈值，3：曲度和位置
    int pos = 0;
    QString minL,maxL = "";
    double min,max  = 0.0;
    vector<Curvature> Curves;
    vector<int> rank;
    QVector<double> bicx(13), bicy(13);
    while (getline(inf, line)){
        switch (module) {
        case 0:
            if(line!=""){
                QString lin = QString(QString::fromLocal8Bit(line.c_str()));
                QStringList linlist = lin.split(" ");
                int ran = linlist.at(1).split(QString::fromLocal8Bit("次多项式")).at(0).toInt();
                rank.push_back(ran);
                this->BICmodel->setItem(pos,0,new QStandardItem(linlist.at(1).split(QString::fromLocal8Bit("次多项式")).at(0)));
                this->BICmodel->setItem(pos,1,new QStandardItem(linlist.at(2).split(":").at(1)));
                this->BICmodel->setItem(pos,2,new QStandardItem(linlist.at(3).split(":").at(1)));
                bicx[pos] = pos+1;
                bicy[ran-1] = linlist.at(2).split(":").at(1).toDouble();
                pos++;
            }else{
                module++;
                pos = 0;
            }
            break;
        case 1:
            if(line != ""){
                if(pos != 0){
                    for(int i=0;i<rank.size();i++){
                        if(rank[i] == pos){
                            QString lin = QString(QString::fromLocal8Bit(line.c_str()));
                            this->BICmodel->setItem(i,5,new QStandardItem(lin));
                        }
                    }
                }
                pos++;
            }else{
                module++;
                pos = 0;
            }
            break;
        case 2:
            if(line != ""){
                QString lin = QString(QString::fromLocal8Bit(line.c_str()));
                if(pos == 0){
                    minL = lin.split(":").at(1);
                }else{
                    maxL = lin.split(":").at(1);
                }
                bool ok;
                min = minL.toDouble(&ok);
                max = maxL.toDouble(&ok);
                ui->range->setText(minL + "~" + maxL);
                pos++;
            }else{
                module++;
                pos = 0;
            }
            break;
        case 3:
            bool ok;
            if(pos <= 12){//1-13次
                Curvature curve(pos+1);
                while(getline(inf,line)){
                    if(line == ""){
                        pos++;
                        break;
                    }else{
                        QString lin = QString(QString::fromLocal8Bit(line.c_str()));
                        double po = lin.split(";").at(0).split(":").at(1).toDouble(&ok);
                        double maxcur = lin.split(";").at(1).split(":").at(1).toDouble(&ok);
                        curve.Pos.push_back(po);
                        curve.Maxcur.push_back(maxcur);
                    }
                }
                Curves.push_back(curve);
            }
            break;
        default:
            break;
        }
    }
    inf.close();

    bool isFindit = false;
    for(int i=0;i<int(rank.size());i++){
        if(!isFindit){
            int maxPos = 0;
            maxPos = FindMaxPos(Curves[rank[i]-1].Maxcur,Curves[rank[i]-1].Pos,min,max);//找到最大曲率的索引值
            if(maxPos != -1){//最大曲率在阈值内
                threshold = pow(10,Curves[rank[i]-1].Pos[maxPos]);
                Threshold = threshold;
                cout<<"threshold:"<<threshold<<endl;
                isFindit = true;
                this->BICmodel->setItem(i,3,new QStandardItem(QString::number(Curves[rank[i]-1].Pos[maxPos])));
                this->BICmodel->setItem(i,4,new QStandardItem(QString::number(threshold)));
            }
        }
        this->BICmodel->setItem(i,6,new QStandardItem(MakeCurOutput(Curves[rank[i]-1].Pos,Curves[rank[i]-1].Maxcur)));
    }
    drawBicCurve(bicx,bicy,rank[0]);

    //是否计算等级规模分布
    if(ui->iscalrs->isChecked()){
        qDebug()<<"checked";
        QString tifpath = ui->input_tifpath->text();
        //        QString countpath = ui->count_Path->text();
        int buildpixel = ui->buildpixel->text().toInt();
        int type = 4;
        if(ui->fourtype->isChecked()){
            type = 4;
        }else if(ui->eighttype->isChecked()){
            type = 8;
        }
        if(tifpath == ""){
            setStatus(QString::fromLocal8Bit("未选择tif文件！请回到步骤一选择输入tif路径和建筑物像素"));
            return;
        }else{
            QThread * t1 = new QThread(0);
            BufferDilation * BD = new BufferDilation;//声明膨胀函数

            progress->setVisible(true);

            setStatus(QString::fromLocal8Bit("计算等级规模分布..."));
            setProgress(10);

            BD->moveToThread(t1);

            BD->setMinParm(tifpath,buildpixel,type);

            BD->SetThreshold(threshold);//设置参数
            qDebug()<<"setThreshold";

            QObject::connect(t1,&QThread::started,BD,&BufferDilation::runRank);
            QObject::connect(BD,&BufferDilation::workFinished,t1,&QThread::quit);
            QObject::connect(BD,&BufferDilation::workFinished,this,&MainWindow::hide_progress);
            QObject::connect(t1,&QThread::finished,t1,&QObject::deleteLater);
            QObject::connect(t1,&QThread::finished,BD,&BufferDilation::deleteBD);
            QObject::connect(BD,&BufferDilation::setStatus,this,&MainWindow::slot_setStatus);
            QObject::connect(BD,&BufferDilation::setProgress,this,&MainWindow::slot_setProgress);
            QObject::connect(BD,&BufferDilation::drawrs,this,&MainWindow::slot_drawrs);

            t1->start();
        }
    }else{//已存在rank_size文件
        QString rspath = ui->rspath->text();
        slot_drawrs(rspath);
        hide_progress();
    }
}

//output urban clusters tif step
void MainWindow::on_pushButton_11_clicked()
{
    int outputnum = ui->numinput->text().toInt();
    QString tifpath = ui->input_tifpath->text();
    int buildpixel = ui->buildpixel->text().toInt();
    int type = 4;
    if(ui->fourtype->isChecked()){
        type = 4;
    }else if(ui->eighttype->isChecked()){
        type = 8;
    }

    if(tifpath == ""){
        setStatus(QString::fromLocal8Bit("未选择tif文件！请回到步骤一选择输入tif路径和建筑物像素"));
        return;
    }else{
        QThread * t1 = new QThread(0);
        BufferDilation * BD = new BufferDilation;//声明膨胀函数

        progress->setVisible(true);

        setStatus(QString::fromLocal8Bit("输出城市斑块..."));
        setProgress(10);

        BD->moveToThread(t1);

        BD->setMinParm(tifpath,buildpixel,type);
        BD->SetThreshold(Threshold);//设置参数
        qDebug()<<"setThreshold";
        BD->SetOutputParm(outputnum);

        QObject::connect(BD,&BufferDilation::setStatus,this,&MainWindow::slot_setStatus);
        QObject::connect(BD,&BufferDilation::setProgress,this,&MainWindow::slot_setProgress);

        QObject::connect(t1,&QThread::started,BD,&BufferDilation::runOutput);
        QObject::connect(BD,&BufferDilation::workFinished,t1,&QThread::quit);
        QObject::connect(BD,&BufferDilation::workFinished,this,&MainWindow::hide_progress);
        QObject::connect(t1,&QThread::finished,t1,&QObject::deleteLater);
        QObject::connect(t1,&QThread::finished,BD,&BufferDilation::deleteBD);

        t1->start();
    }
}

// output urban boundary shp step
void MainWindow::on_pushButton_13_clicked()
{
    int distance = ui->disinput->text().toInt();
    int urbanpixel = ui->urbanpixelinput->text().toInt();
    QString tifpath = ui->urbanpath->text();
    FindContours * FC = new FindContours;

    progress->setVisible(true);
    setProgress(10);

    FC->SetParam(tifpath,distance,urbanpixel);
    QThread * t1 = new QThread;
    FC->moveToThread(t1);

    QObject::connect(FC,&FindContours::setStatus,this,&MainWindow::slot_setStatus);
    QObject::connect(FC,&FindContours::setProgress,this,&MainWindow::slot_setProgress);

    QObject::connect(t1,&QThread::started,FC,&FindContours::runFC);
    QObject::connect(FC,&FindContours::workFinished,t1,&QThread::quit);
    QObject::connect(FC,&FindContours::workFinished,this,&MainWindow::urban_finish);
    QObject::connect(t1,&QThread::finished,t1,&QObject::deleteLater);
    QObject::connect(t1,&QThread::finished,FC,&FindContours::deleteFC);

    t1->start();
}

// Minkowski's dilation step
void MainWindow::on_pushButton_6_clicked()//Minkowski运算
{
    QString tifpath = ui->input_tifpath->text();
    if(tifpath == ""){
        setStatus(QString::fromLocal8Bit("未选择tif文件！"));
        return;
    }
    int buildpixel = ui->buildpixel->text().toInt();
    int type = 4;
    if(ui->fourtype->isChecked()){
        type = 4;
    }else if(ui->eighttype->isChecked()){
        type = 8;
    }

    progress->setVisible(true);
    setProgress(0);

    BufferDilation * BD = new BufferDilation;//声明膨胀函数

    setStatus(QString::fromLocal8Bit("Minkowski膨胀算法开始..."));
    setProgress(5);

    QThread * t1 = new QThread;
    BD->moveToThread(t1);

    qDebug()<<type;

    BD->setMinParm(tifpath,buildpixel,type);

    QObject::connect(BD,&BufferDilation::setStatus,this,&MainWindow::slot_setStatus);
    QObject::connect(BD,&BufferDilation::setProgress,this,&MainWindow::slot_setProgress);

    QObject::connect(t1,&QThread::started,BD,&BufferDilation::runMin);
    QObject::connect(BD,&BufferDilation::workFinished,t1,&QThread::quit);
    QObject::connect(BD,&BufferDilation::workFinished,this,&MainWindow::hide_progress);
    QObject::connect(t1,&QThread::finished,t1,&QObject::deleteLater);
    QObject::connect(t1,&QThread::finished,BD,&BufferDilation::deleteBD);


    t1->start();

    ui->pushButton_6->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
}

//Cubic Hermite Interpolation
void MainWindow::on_pushButton_17_clicked()
{
    setProgress(0);
    setStatus(QString::fromLocal8Bit("开始插值..."));
    Hermite h_module;
    h_module.setInfo(ui->urbanpath_input->text(),ui->interp_gap->text().toDouble());
    h_module.Cubic_Hermite();
    setStatus(QString::fromLocal8Bit("插值完毕，输出文件_count_interp.txt"));
    setProgress(100);
    hide_progress();
}
/************END***********/
