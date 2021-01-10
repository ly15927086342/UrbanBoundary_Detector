#include "agglomerativeclustering.h"

void AgglomerativeClustering::setInfo(QString path){
    this->rankPath = path;
}

int AgglomerativeClustering::calClusters(){
//    QString a = QString::fromLocal8Bit("F:\Python\Python37_86");
//    Py_SetPythonHome(a.toStdWString().c_str());
//    Py_Initialize();

//    string path = "";
//    string chdir_cmd = string("sys.path.append(\"") + path + "\")";
//    const char* cstr_cmd = chdir_cmd.c_str();
//    PyRun_SimpleString("import sys");
//    PyRun_SimpleString(cstr_cmd);

//    PyObject* moduleName = Py_BuildValue("s","HAC"); //模块名，不是文件名
//    PyObject* pModule = PyImport_Import(moduleName);
//    if (!pModule) // 加载模块失败
//    {
//        cout << "[ERROR] Python get module failed." << endl;
//        return -1;
//    }
//    cout << "[INFO] Python get module succeed." << endl;

//    // 加载函数
//    PyObject* pv = PyObject_GetAttrString(pModule, "cal_CityClusters");
//    if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
//    {
//        cout << "[ERROR] Can't find funftion (test_add)" << endl;
//        return -1;
//    }
//    cout << "[INFO] Get function (test_add) succeed." << endl;

//    // 设置参数
//    PyObject* args = PyTuple_New(1);   // 1个参数
//    PyObject* arg1 = Py_BuildValue("s", rankPath.toStdString().c_str());    // rank_size文件路径
//    PyTuple_SetItem(args, 0, arg1);

//    PyObject* pRet = PyObject_CallObject(pv, args);

//    // 获取参数
//    if (pRet)  // 验证是否调用成功
//    {
//        this->city_Count = PyLong_AsLong(pRet);
//        cout << "result:" << this->city_Count;
//    }

//    Py_Finalize();

    return this->city_Count;
}
