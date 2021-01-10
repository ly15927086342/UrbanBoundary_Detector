#ifndef AGGLOMERATIVECLUSTERING
#define AGGLOMERATIVECLUSTERING

#include "all.h"

class AgglomerativeClustering{
private:
    QString rankPath = "";
    int city_Count;
public:
    ~AgglomerativeClustering(){

    }

    void setInfo(QString path);
    int calClusters();
};

#endif // AGGLOMERATIVECLUSTERING

