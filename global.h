#ifndef GLOBAL_H
#define GLOBAL_H
#include <map>
#include <QString>
#include <QDateTime>


class Global
{

public:
    static Global * getInstance();

private:
    Global();
public:
    static Global * instance;

    uint64_t totalCount = 0;
    uint64_t errorCount = 0;
    bool errFlag = false;

    std::map<uint64_t, double> data;
    std::map<uint64_t, double> errorData;

    bool flag = true;
    bool S2112 = true;//true表示s21，false表示s12


    QString qstrData = "";

    //预设
    uint64_t startFrequency = 100000;
    uint64_t stopFrequency = 9000000000;
    uint64_t point = 201;







};

#endif // GLOBAL_H
