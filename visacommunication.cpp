#include "visacommunication.h"

visaCommunication::visaCommunication(QObject *parent) : QObject(parent)
{

}

void visaCommunication::connectToVISADevice(QString & deviceName)
{
    status = viOpenDefaultRM(&defaultRM);
    if(status != VI_SUCCESS){
        //连接失败
        emit connectToVISADeviceSignals(false);
        return;
    }

    status = viOpen(defaultRM, deviceName.toStdString().data(), VI_NULL, VI_NULL, &visaDevice);

    if(status != VI_SUCCESS){
        emit connectToVISADeviceSignals(false);
        return;
    }

    //连接成功
    emit connectToVISADeviceSignals(true);
}

void visaCommunication::disconnectFromVISADevice()
{
    status = viClose(visaDevice);
    status += viClose(defaultRM);
    if(status == VI_SUCCESS){
        emit disconnectFromVISADeviceSignals(true);
    }else{
        emit disconnectFromVISADeviceSignals(false);
    }
}

void visaCommunication::writeToViasDevice(QString & scpiCommand)
{
    std::string str = scpiCommand.toStdString();
    if(str == ""){
        return;
    }
    str += "\n";
    status = viPrintf(visaDevice, str.data(), 0);
}

void visaCommunication::readFromViasDevice(QString & readBuffer)
{
    //待写
}

void visaCommunication::setVisaDeviceAttribute(ViAttr attrName, ViAttrState attrValue)
{
    status = viSetAttribute(visaDevice, attrName, attrValue);
    if(status != VI_SUCCESS)
    {
        //设置失败
        emit setVisaAttributeSignals(false);
        return;
    }else{
        //设置成功
        emit setVisaAttributeSignals(true);
    }
}
