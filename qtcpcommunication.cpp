#include "qtcpcommunication.h"
const unsigned writeBuffer = 200;//每次默認讀取的大小
const unsigned readBuffer = 512 * 1024;//1 * 1024 * 1024;//M

qTcpCommunication::qTcpCommunication(QObject *parent) : QObject(parent)
{
    qTcpDevice = new QTcpSocket(this);
    myGlobal = Global::getInstance();

    connect(qTcpDevice, &QTcpSocket::connected, this, &qTcpCommunication::connectToqTcpDeviceStatus);
    connect(qTcpDevice, &QTcpSocket::disconnected, this, &qTcpCommunication::disconnectFromqTcpDeviceStatus);
   // connect(qTcpDevice, &QTcpSocket::error, this, &qTcpCommunication::deviceConnectError);
    connect(qTcpDevice, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deviceConnectError(QAbstractSocket::SocketError)));
    connect(qTcpDevice, &QTcpSocket::readyRead, this, &qTcpCommunication::readFromqTcpDevice);

}

void qTcpCommunication::deviceConnectError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
       {
           case QAbstractSocket::RemoteHostClosedError:
                break;
           case QAbstractSocket::HostNotFoundError:
                emit disconnectFromqTcpDeviceSignals(false);
                break;
           case QAbstractSocket::ConnectionRefusedError:
                emit disconnectFromqTcpDeviceSignals(false);
                break;
           default:
                emit disconnectFromqTcpDeviceSignals(false);
        }


       emit disconnectFromqTcpDeviceSignals(false);

}

void qTcpCommunication::connectToqTcpDevice(QString deviceName, quint16 port)
{
    qTcpDevice->connectToHost(deviceName, port);
}

void qTcpCommunication::disconnectFromqTcpDeviceStatus()
{ 
    qTcpDevice->abort();
    emit disconnectFromqTcpDeviceSignals(false);
}

void qTcpCommunication::writeToqTcpDevice(QString scpiCommand)
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
    qDebug()<<"read";
    forever{
        if(qTcpDevice->bytesAvailable() != 0){
            //如果缓存中有可用数据就一直读取
            char r[readBuffer] = {""};
            qTcpDevice->read(r, readBuffer);
            myGlobal->qstrData += QString(r);
        }else {
            //如果缓存中没有可读取数据则退出，等待下一次数据过来时读取
            break;
        }
    }
}

void qTcpCommunication::connectToqTcpDeviceStatus()
{
    emit connectToqTcpDeviceSignals(true);
}

void qTcpCommunication::disconnectedFromTcpDeviceManual()
{
    qTcpDevice->abort();
    emit disconnectFromqTcpDeviceSignals(true);
}


