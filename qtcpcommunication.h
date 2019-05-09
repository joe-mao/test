#ifndef QTCPCOMMUNICATION_H
#define QTCPCOMMUNICATION_H

#include <QObject>
#include <QTcpSocket>


class qTcpCommunication : public QObject
{
    Q_OBJECT
public:
    explicit qTcpCommunication(QObject *parent = nullptr);

signals:
    void connectToqTcpDeviceSignals(bool status);//发送连接成功与否信号,true表示连接成功，false表示连接失败


public slots:
    void connectToqTcpDevice(QString & deviceName, quint16 port);//连接到设备
    void disconnectFromqTcpDevice();//从Tcp设备断开
    void writeToqTcpDevice(QString & scpiCommand);
    void readFromqTcpDevice();



private:
    QTcpSocket * qTcpDevice;
};

#endif // QTCPCOMMUNICATION_H
