#include "datamodule.h"

void DataSource::ReadBaseInfo(QString inpath, const int value){
    this->inputfilepath = inpath;
    this->outputfilepath = inpath;
    this->buildvalue = value;
}

void DataSource::SetImgInfo(int width, int height, int bandnum, int buildpixel, int depth, int byte, const char *ref, double x, double y, double pw, double ph){
    this->Width = width;
    this->Height = height;
    this->BandCount = bandnum;
    this->BuildPixel = buildpixel;
    this->TotalPixel = width*height;
    this->Depth = depth;
    this->Byte = byte;
    this->Posi_UL_x = x;
    this->Posi_UL_y = y;
    this->Pixel_Width = pw;
    this->Pixel_Height = ph;
    this->ProjRef = ref;
}

void DataSource::PrintInfo(){
    double level = BuildPixel* fabs(Pixel_Width*Pixel_Height) / TotalPixel;
    cout << "-------------------------------------------" << endl;
//    cout << "图像路径：" << inputfilepath << endl;
    cout << "图像宽度：" << Width << endl;
    cout << "图像长度：" << Height << endl;
    cout << "  位深度：" << Depth << endl;
    cout << "图像字节：" << Byte << endl;
    cout << "波段总数：" << BandCount << endl;
    cout << "建筑像素：" << BuildPixel << endl;
    cout << "  总像素：" << TotalPixel << endl;
    cout << "建筑占比：" << setprecision(1) << fixed << level << "%" << endl;
    cout << "左上坐标：" << setprecision(3) << fixed << "(" << Posi_UL_x << "," << Posi_UL_y << ")" << endl;
    cout << "横分辨率：" << Pixel_Width << endl;
    cout << "纵分辨率：" << Pixel_Height << endl;
    cout << "-------------------------------------------" << endl;
}

void DataSource::ReadTif(){
    //	vector<vector<unsigned long int>>img;//存储图像像素值

    GDALAllRegister();//注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//以防中文名不能正常读取
    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(inputfilepath.toLocal8Bit().data(), GA_ReadOnly);//打开tif
    if (m_pDataset == NULL)
    {
        cout << "指定的文件不能打开！" << endl;
    }
    int BandNum = m_pDataset->GetRasterCount();//波段数
    int dataWidth = m_pDataset->GetRasterXSize();//宽
    int dataHeight = m_pDataset->GetRasterYSize();//高

    const char *projRef = m_pDataset->GetProjectionRef();//projRef将会存储test.tif中的坐标信息
    double adfGeoTransform[6];//dfGeoTransform存储一些基准点的坐标和每一个pixel表示的长度等
    m_pDataset->GetGeoTransform(adfGeoTransform);

    int byte = GDALGetDataTypeSize(m_pDataset->GetRasterBand(1)->GetRasterDataType()) / 8;//字节数
    int depth = GDALGetDataTypeSize(m_pDataset->GetRasterBand(1)->GetRasterDataType());//位深度

    cout << "开始读取图像..." << endl;
    int totalpixel = 0;//建筑总像素数

    if (BandNum == 1){//单波段
        GDALRasterBand *pBand = m_pDataset->GetRasterBand(1);//获取第1波段的波段指针，参数就是表示第几波段的意思
        m_data = new short int[dataHeight*dataWidth];//临时存储图像像素值的指针
        CPLErr err = pBand->RasterIO(GF_Read, 0, 0, dataWidth, dataHeight, m_data, dataWidth, dataHeight, GDT_UInt16, 0, 0);//读取图像
        for (int i = 0; i < dataHeight; i++){//存入img和isChecked，更新totalpixel
            for (int j = 0; j < dataWidth; j++){
                if (m_data[i*dataWidth + j] == buildvalue){
                    m_data[i*dataWidth + j] = 1;
                    totalpixel++;
                }
                else{
                    m_data[i*dataWidth + j] = 0;
                }
            }
        }
    }
    cout << "读取完毕" << endl;
    SetImgInfo(dataWidth, dataHeight, BandNum, totalpixel, depth, byte, projRef, adfGeoTransform[0], adfGeoTransform[3], adfGeoTransform[1], adfGeoTransform[5]);//存入类中
    GDALClose(m_pDataset);//关闭gdaldataset
}

void DataSource::WriteTif(short int*data,int width,int height,int buf,int num){
    GDALAllRegister();//注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//以防中文名不能正常读取
    const char *pszFormat = "GTiff";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if (poDriver == NULL)
        exit(1);
    QString outputpath = outputfilepath.split(".tif").at(0) + "_output_" + QString::number(buf)+"_"+QString::number(num) + ".tif";
    GDALDataset *poDstDS;
    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(inputfilepath.toLocal8Bit().data(), GA_ReadOnly);//打开tif
    poDstDS = poDriver->CreateCopy(outputpath.toLocal8Bit().data(), m_pDataset, FALSE, NULL, NULL, NULL);
    //GDALDataset *poDstDS1;
    //poDstDS = poDriver->Create(output, Width, Height, 1, GDT_UInt32, nullptr);
    GDALRasterBand *poBand = poDstDS->GetRasterBand(1);
    CPLErr err = poBand->RasterIO(GF_Write, 0, 0, width, height, data, width, height, GDT_UInt16, 0, 0);
    //poDstDS1->RasterIO(GF_Write, 0, 0, Width, Height, data, Width, Height, GDT_UInt32, 1, 0, 0, 0, 0);
    GDALClose(poDstDS);//关闭gdaldataset
    GDALClose(m_pDataset);
    cout << "write in tif" << endl;
}

void DataSource::WriteShp_line(list<list<Point>>&Result,int dis){
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(inputfilepath.toLocal8Bit().data(), GA_ReadOnly);
    const char *projRef = m_pDataset->GetProjectionRef();

    /*图像坐标系转换为WGS84经纬度坐标*/
    OGRSpatialReference fRef;//fRef:原始坐标参数，和.tif一致；tRef:转换后的坐标。
    char *tmp = NULL;
    //	获得projRef的一份拷贝
    //	由于projRef是const char*,下面的一个函数不接受，所以需要转换成非const
    tmp = (char *)malloc(strlen(projRef) + 1);
    strcpy_s(tmp, strlen(projRef) + 1, projRef);

    fRef.importFromWkt(&tmp);//设置原始的坐标参数，和test.tif一致
    GDALClose(m_pDataset);

    CPLSetConfigOption("SHAPE_ENCODING", "");//使属性表字段支持中文
    QString outputpath = outputfilepath.split(".tif").at(0) + "_boundary_" + QString::number(dis) + ".shp";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");//调用对Shape文件读写的Driver
    GDALDataset *ds1 = poDriver->Create(outputpath.toLocal8Bit().data(), 0, 0, 0, GDT_Unknown, NULL);
    if (poDriver == NULL)
    {
        cout << "驱动不可用！";
        return;
    }

    OGRLayer *poLayer = ds1->CreateLayer(outputpath.toLocal8Bit().data(), &fRef, wkbLineString, NULL);//创建图层Layer

    if (poLayer == NULL)
    {
        cout << "Layer Creation Failed";
        return;
    }

    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;
    for (itor1 = Result.begin(); itor1 != Result.end(); itor1++){
        OGRFeature *poFeature;//创建一个feature
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());//GetLayerDefn()获取当前图层的属性表结构
        OGRLineString or;//创建线要素
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            double x1 = (*itor2).x;
            double y1 = (*itor2).y;
            x1 = Posi_UL_x + x1*Pixel_Width + 0.5*Pixel_Width;//求凸包中点的横坐标
            y1 = Posi_UL_y + y1*Pixel_Height + 0.5*Pixel_Height;//求凸包中点的纵坐标
            or.addPoint(x1, y1);//给线要素添加点要素
        }
        or.closeRings();
        poFeature->SetGeometry(&or);//将线要素加入图层中
        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
        {
            cout << "Failed to create feature in shapefile.\n" << endl;
            exit(1);
        }
        OGRFeature::DestroyFeature(poFeature);//关闭feature
    }

    GDALClose(ds1);
    cout << "输出边界完毕." << endl;
}

void DataSource::WriteShp_polygon(list<list<Point>>&Result,int dis){
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(inputfilepath.toLocal8Bit().data(), GA_ReadOnly);
    const char *projRef = m_pDataset->GetProjectionRef();

    /*图像坐标系转换为WGS84经纬度坐标*/
    OGRSpatialReference fRef;//fRef:原始坐标参数，和.tif一致；tRef:转换后的坐标。
    char *tmp = NULL;
    //	获得projRef的一份拷贝
    //	由于projRef是const char*,下面的一个函数不接受，所以需要转换成非const
    tmp = (char *)malloc(strlen(projRef) + 1);
    strcpy_s(tmp, strlen(projRef) + 1, projRef);

    fRef.importFromWkt(&tmp);//设置原始的坐标参数，和test.tif一致
    GDALClose(m_pDataset);

    CPLSetConfigOption("SHAPE_ENCODING", "");//使属性表字段支持中文
    QString outputpath = outputfilepath.split(".tif").at(0) + "_polygon_" + QString::number(dis) + ".shp";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");//调用对Shape文件读写的Driver
    GDALDataset *ds1 = poDriver->Create(outputpath.toLocal8Bit().data(), 0, 0, 0, GDT_Unknown, NULL);
    if (poDriver == NULL)
    {
        cout << "驱动不可用！";
        return;
    }

    OGRLayer *poLayer = ds1->CreateLayer(outputpath.toLocal8Bit().data(), &fRef, wkbPolygon, NULL);//创建图层Layer

    if (poLayer == NULL)
    {
        cout << "Layer Creation Failed";
        return;
    }

    list<list<Point>>::iterator itor1;
    list<Point>::iterator itor2;
    for (itor1 = Result.begin(); itor1 != Result.end(); itor1++){
        OGRFeature *poFeature;//创建一个feature
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());//GetLayerDefn()获取当前图层的属性表结构
        OGRPolygon poly;
        OGRLinearRing or;//创建线要素
        for (itor2 = (*itor1).begin(); itor2 != (*itor1).end(); itor2++){
            double x1 = (*itor2).x;
            double y1 = (*itor2).y;
            x1 = Posi_UL_x + x1*Pixel_Width + 0.5*Pixel_Width;//求凸包中点的横坐标
            y1 = Posi_UL_y + y1*Pixel_Height + 0.5*Pixel_Height;//求凸包中点的纵坐标
            or.addPoint(x1, y1);//给线要素添加点要素
        }
        or.closeRings();
        poly.addRing(&or);
        poFeature->SetGeometry(&poly);//将线要素加入图层中
        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
        {
            cout << "Failed to create feature in shapefile.\n" << endl;
            exit(1);
        }
        OGRFeature::DestroyFeature(poFeature);//关闭feature
    }

    GDALClose(ds1);
    cout << "输出边界完毕." << endl;
}

