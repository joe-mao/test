#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProgressBar>
#include <QDateTime>
#include <QFile>
#include <QTextStream>




//定義延時函數
void mySleep(int msec)
{
    QDateTime reachTime = QDateTime::currentDateTime().addMSecs(msec);
    while( QDateTime::currentDateTime() < reachTime){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(441, 284);

    myGlobal = Global::getInstance();


    progressBar = new QProgressBar(this);
    progressBar->setTextVisible( false );
    this->ui->statusBar->addWidget(progressBar, 1);




    thread1 = new QThread(this);
    dev800 = new qTcpCommunication();
    dev800->moveToThread(thread1);
    thread1->start();



    connect(this, &MainWindow::connectTo800, dev800, &qTcpCommunication::connectToqTcpDevice);
   connect(dev800, &qTcpCommunication::connectToqTcpDeviceSignals, this, &MainWindow::onConnectToDevice);
   connect(this, &MainWindow::disconnectedFromDevice, dev800, &qTcpCommunication::disconnectedFromTcpDeviceManual);
   connect(this, &MainWindow::writeToDeviceSignal, dev800, &qTcpCommunication::writeToqTcpDevice);
   connect(dev800, &qTcpCommunication::disconnectFromqTcpDeviceSignals, this, &MainWindow::onDisconnectFromDevice);
    emit connectTo800("192.168.1.2", 5025);

}

MainWindow::~MainWindow()
{
    thread1->quit();
    thread1->wait();
    delete ui;
}

void MainWindow::on_btn_start_clicked()
{
    this->ui->btn_start->setEnabled(false);
    forever{
        if(myGlobal->flag){
            //qDebug()<<myGlobal->flag;
            //flag为true则一直执行
            // emit writeToDeviceSignal("SYST:PREset");

            // emit writeToDeviceSignal("*WAI");

            if(myGlobal->S2112){
                //默认s21
                emit writeToDeviceSignal("CALCulate1:MEASure1:PARameter 'S21'");

                emit writeToDeviceSignal("*WAI");

                emit writeToDeviceSignal("SENS:SWE:MODE Single");

                emit writeToDeviceSignal("*WAI");

                //DISP:WIND:Y:AUTO
                emit writeToDeviceSignal("DISP:WIND:Y:AUTO");

                emit writeToDeviceSignal("CALCulate1:MEASure1:DATA:FDATA?");

                mySleep(4 * 1000);

                processData();
                writeErrorToFile();
                //每循环处理完一次数据，则将总次数加1，并更新显示
                ++myGlobal->totalCount;
                this->ui->lb_totalCount->setText(QString::number(myGlobal->totalCount));
                this->ui->lb_errorCount->setText(QString::number(myGlobal->errorCount));

                //清空数据，处理下一次的数据
                myGlobal->qstrData.clear();//清空接受的原始数据（包含的全部都是db值）
                myGlobal->data.clear();//清空data数据map类型（频点和db值对应）
                myGlobal->errorData.clear();//清空查超出范围的数据
                myGlobal->errFlag = false;

                myGlobal->S2112 = false;//下一次跑s12

                mySleep(1 * 1000);
            }else
            {
                //否则s12
                //默认s21
                emit writeToDeviceSignal("CALCulate1:MEASure1:PARameter 'S12'");

                emit writeToDeviceSignal("*WAI");

                emit writeToDeviceSignal("SENS:SWE:MODE Single");

                emit writeToDeviceSignal("*WAI");

                //DISP:WIND:Y:AUTO
                emit writeToDeviceSignal("DISP:WIND:Y:AUTO");

                emit writeToDeviceSignal("CALCulate1:MEASure1:DATA:FDATA?");

                mySleep(4 * 1000);

                processData();
                writeErrorToFile();
                //每循环处理完一次数据，则将总次数加1，并更新显示
                ++myGlobal->totalCount;
                this->ui->lb_totalCount->setText(QString::number(myGlobal->totalCount));
                this->ui->lb_errorCount->setText(QString::number(myGlobal->errorCount));

                //清空数据，处理下一次的数据
                myGlobal->qstrData.clear();//清空接受的原始数据（包含的全部都是db值）
                myGlobal->data.clear();//清空data数据map类型（频点和db值对应）
                myGlobal->errorData.clear();//清空查超出范围的数据
                myGlobal->errFlag = false;
                myGlobal->S2112 = true;

                mySleep(1 * 1000);

            }
        }else{
            break;
        }
    }
}

void MainWindow::onConnectToDevice(bool status)
{

    if(status == true){
        progressBar->setRange(0,0);

        return;
    }else {
        progressBar->setRange(0,1);
        return;
    }
}

void MainWindow::on_btn_stop_clicked()
{
    progressBar->setRange(0,1);
    emit disconnectedFromDevice();
    myGlobal->flag = false;
    thread1->quit();
    thread1->wait();
}

void MainWindow::onDisconnectFromDevice(bool status)
{
    qDebug()<<"error Occur in Network";
    progressBar->setRange(0,1);
    myGlobal->flag = false;
    thread1->quit();
    thread1->wait();
}

void MainWindow::writeErrorToFile()
{
    //将errorData中的数据写入到文件中
    QFile data("output.txt");
     if (data.open(QFile::Append | QFile::Text)) {
         QTextStream out(&data);
         QDateTime current_date_time = QDateTime::currentDateTime();
         QString current_date = current_date_time.toString("yyyy-MM-dd");
         QString current_time = current_date_time.toString("hh:mm:ss.zzz ");
         if(myGlobal->S2112){
             out<<"S21   ";
         }else{
             out<<"S12   ";
         }
         out <<current_date<<"      "<<current_time<<"      ";
         for(std::map<uint64_t, double>::iterator it = myGlobal->errorData.begin(); it != myGlobal->errorData.end(); it++){
             out<<"("<<QString::number(it->first)<<","<<QString::number(it->second)<<")"<<" ";
         }
         out<<"\n";
    }
     data.close();
}

void MainWindow::processData()
{

    //对输出的数据进行分析
    //先转化成Map再对Map进行分析
    uint64_t per = ( myGlobal->stopFrequency - myGlobal->startFrequency ) / ( myGlobal->point - 1 );

    QStringList qStringList = myGlobal->qstrData.split(',');
    unsigned long count = 0;
    //qDebug()<<qStringList.count();;
    //qDebug()<<myGlobal->qstrData.left(myGlobal->qstrData.length() - 1);//取代哦最后一个\n,带\n竟然也可以直接转。。。
    //将数据转换为map形式
    foreach(QString data1, qStringList){
        myGlobal->data[100000 + per * count] = data1.toDouble();
        ++count;
    }
   // qDebug()<<"-----------------------------------";

    for(std::map<uint64_t, double>::iterator it = myGlobal->data.begin(); it != myGlobal->data.end(); it++){
        //qDebug()<<it->first<<"***"<<it->second;
//        if(100000 <= it->first && it->first <= 300000){
//            if(fabs(it->second) >= 0.03){
//                //范围之外
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }else if(300000 < it->first && it->first <= 10000000){
//            if(fabs(it->second) >= 0.03){
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }else if(10000000 < it->first && it->first <= 3000000000){
//            if(fabs(it->second) >= 0.03){
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }else if(3000000000 < it->first && it->first <= 6000000000){
//            if(fabs(it->second) >= 0.03){
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }else if(6000000000 < it->first && it->first <= 8500000000){
//            if(fabs(it->second) >= 0.03){
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }else if(8500000000 < it->first && it->first <= 9000000000){
//            if(fabs(it->second) >= 0.03){
//                myGlobal->errorData[it->first] = it->second;
//                myGlobal->errFlag = true;
//            }

//        }
        if(100000 <= it->first && it->first <= 300000){
            if(fabs(it->second) > 0.04){
                //范围之外
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }else if(300000 < it->first && it->first <= 10000000){
            if(fabs(it->second) > 0.035){
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }else if(10000000 < it->first && it->first <= 3000000000){
            if(fabs(it->second) > 0.03){
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }else if(3000000000 < it->first && it->first <= 6000000000){
            if(fabs(it->second) > 0.07){
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }else if(6000000000 < it->first && it->first <= 8500000000){
            if(fabs(it->second) > 0.1){
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }else if(8500000000 < it->first && it->first <= 9000000000){
            if(fabs(it->second) > 0.15){
                myGlobal->errorData[it->first] = it->second;
                myGlobal->errFlag = true;
            }

        }
    }

    if(myGlobal->errFlag){
        //有一个频点fail则表示fail,此时表示error发生myGlobal->errFlag表示true
        ++myGlobal->errorCount;
    }
}
