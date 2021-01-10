#ifndef CUBIC_HERMITE_INTERP
#define CUBIC_HERMITE_INTERP

#include "all.h"

class Hermite{
private:
    QString inputPath = "";
    double gap;
public:
    ~Hermite(){

    }

    void setInfo(QString path,double gp);

    void Cubic_Hermite();
};

#endif // CUBIC_HERMITE_INTERP
