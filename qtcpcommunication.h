#ifndef QTCPCOMMUNICATION_H
#define QTCPCOMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
#include "global.h"
#include <QAbstractSocket>


class qTcpCommunication : public QObject
{
    Q_OBJECT
public:
    explicit qTcpCommunication(QObject *parent = nullptr);

signals:
    void connectToqTcpDeviceSignals(bool status);//发送连接成功与否信号,true表示连接成功，false表示连接失败
    void disconnectFromqTcpDeviceSignals(bool status);//设备断开信号，暂定true表示成功，false表示失败,暂定一直为true


public slots:
    void connectToqTcpDevice(QString deviceName, quint16 port);//连接到设备
    void connectToqTcpDeviceStatus();//是否成功连接到信号
    void disconnectFromqTcpDeviceStatus();//从Tcp设备断开
    void disconnectedFromTcpDeviceManual();//手动断开
    void writeToqTcpDevice(QString scpiCommand);
    void readFromqTcpDevice();
    void deviceConnectError(QAbstractSocket::SocketError socketError);



private:
    QTcpSocket * qTcpDevice;
    Global * myGlobal;

};

#endif // QTCPCOMMUNICATION_H
