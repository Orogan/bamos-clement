#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QTimer>
#include <cmath>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void readData();


    void on_DecoButton_clicked();

    void on_connectButton_clicked();

    unsigned char decal_octet(unsigned char mesure_Octets);

    unsigned char add_octet(unsigned char octet1, unsigned char octet2, int valeur_max);

    void startFrame();

    void on_pushButton_stop_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray temp;
    QTimer *timer;
    unsigned char octet1_modif;
    float valeur2;
    bool test_connect = false;


};

#endif // MAINWINDOW_H
