#ifndef VISACOMMUNICATION_H
#define VISACOMMUNICATION_H

#include <QObject>
#include <visa.h>

class visaCommunication : public QObject
{
    Q_OBJECT
public:
    explicit visaCommunication(QObject *parent = nullptr);
    void setVisaDeviceAttribute(ViAttr attrName, ViAttrState attrValue);//设置VISA设备属性

signals:
    void connectToVISADeviceSignals(bool status);//连接到VISA设备的信号，true表示连接成功，false表示连接失败
    void disconnectFromVISADeviceSignals(bool status);//从VISA设备断开连接的信号，true表示连接成功，false表示连接失败
    void setVisaAttributeSignals(bool status);//设置visa设备属性是否成功的信号，true表示设置成功，false表示设置失败

public slots:
    void connectToVISADevice(QString & deviceName);//连接到VISA设备，参数表示需要连接的设备的名字，需按照VISA格式配置
    void disconnectFromVISADevice();//从链接的VISA设备断开

    void writeToViasDevice(QString & scpiCommand);//向VISA设备发送信息
    void readFromViasDevice(QString & readBuffer);//从VISA设备读取信息


private:
    ViStatus status;
    ViSession defaultRM;
    ViSession visaDevice;
};

#endif // VISACOMMUNICATION_H
