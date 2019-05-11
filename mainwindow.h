#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <map>
#include "qtcpcommunication.h"
#include "global.h"
#include <QProgressBar>
#include <cmath>






namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void connectTo800(QString name, quint16 port);
    void disconnectedFromDevice();
    void writeToDeviceSignal(QString scpiCommand);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void processData();
    void writeErrorToFile();

private slots:
    void on_btn_start_clicked();
    void onConnectToDevice(bool status);
    void onDisconnectFromDevice(bool status);
    void on_btn_stop_clicked();

private:
    Ui::MainWindow *ui;
    Global * myGlobal;
    QThread * thread1;
    qTcpCommunication * dev800;
    QProgressBar *progressBar;




};

#endif // MAINWINDOW_H
