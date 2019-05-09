#include "qtcpcommunication.h"
const unsigned writeBuffer = 200;//每次默認讀取的大小
const unsigned readBuffer = 512 * 1024;//1 * 1024 * 1024;//M

qTcpCommunication::qTcpCommunication(QObject *parent) : QObject(parent)
{
    qTcpDevice = new QTcpSocket(this);
}


void qTcpCommunication::connectToqTcpDevice(QString & deviceName, quint16 port)
{
    qTcpDevice->connectToHost(deviceName, port);
}

void qTcpCommunication::disconnectFromqTcpDevice()
{
    qTcpDevice->abort();
}

void qTcpCommunication::writeToqTcpDevice(QString & scpiCommand)
{
    std::string str = scpiCommand.toStdString();
    if(str == ""){
        return;
    }
    str += "\n";
    char w[writeBuffer] = {""};
    qstrcpy(w, str.data());
    qTcpDevice->write(w, writeBuffer);
}

void qTcpCommunication::readFromqTcpDevice()
{
    forever{
        if(qTcpDevice->bytesAvailable() != 0){
            //如果缓存中有可用数据就一直读取
            char r[readBuffer] = {""};
            qTcpDevice->read(r, readBuffer);
        }else {
            //如果缓存中没有可读取数据则退出，等待下一次数据过来时读取
            break;
        }
    }
}
