/**
 * @file mainwindow.cpp
 * @brief Programme de recuperation des donnees du logycap
 * @author GIRARD Clement
 * @date 25 mars 2016
 * @version 2.0
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 * @return
 *
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    timer = new QTimer(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    serial->setPortName("COM1");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);

    ui->doubleSpinBox_pabs->setDecimals(1);
    ui->doubleSpinBox_pabs->setSingleStep(0.1);
    ui->doubleSpinBox_pabs->setMinimum(0);
    ui->doubleSpinBox_pabs->setMaximum(1001);

}

/**
 * @brief MainWindow::~MainWindow
 * @return
 *
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_pushButton_clicked
 * @return
 *
 */
void MainWindow::on_pushButton_clicked()
{        
    if(test_connect == false)
    {
        qDebug() << "Erreur Non Connecte !";
    }
    else
    {
        //timer pour lancer en boucle l'envoie de la trame
        connect(timer, SIGNAL(timeout()), this, SLOT(startFrame()));
        timer->start(500);
    }
}

/**
 * @brief MainWindow::startFrame
 * @return
 *
 */
void MainWindow::startFrame()
{
    //envoie de la trame pour commencer la communication avec le logycap
    QByteArray trame;
    trame[0] = 0x81;

    serial->write(trame);
}

/**
 * @brief MainWindow::readData
 * @return
 *
 */
void MainWindow::readData()
{
    QByteArray data;

    //lecture sur le port série
    data = serial->readAll();

    //reconstitution de la trame qui arrive en deux partie
    temp.append(data);

    if(temp.length() == 17)
    {
        //recuperation de la valeur entrer dans le champ Pabs
        QString recup_pabs = ui->doubleSpinBox_pabs ->text();
        float p_abs = recup_pabs.toFloat();

        //recuperation, calcul et affichage de la vitesse
        QByteArray vitesse = temp.mid(1,2);

        qDebug() << temp;

        decal_octet(vitesse[0]);
        add_octet(octet1_modif, vitesse[1], 3000);
        float vitesse1 = valeur2;
        QString valvit = QString::number(valeur2);
        ui->lineEdit_vitesse->setText(valvit);

        //recuperation, calcul et affichage du courant
        QByteArray courant = temp.mid(5,2);
        decal_octet(courant[0]);
        add_octet(octet1_modif, courant[1], 20);
        float courant1 = valeur2;
        QString valcour = QString::number(valeur2);
        ui->lineEdit_courant->setText(valcour);

        //recuperation, calcul et affichage de la tension
        QByteArray tension = temp.mid(7,2);
        decal_octet(tension[0]);
        add_octet(octet1_modif, tension[1],1000);
        float tension1 = valeur2;
        QString valtens = QString::number(valeur2);
        ui->lineEdit_tension->setText(valtens);

        //calcul et affichage de Vdt
        float vdt = vitesse1 * 60/1000;
        QString vdt1 = QString::number(vdt);
        ui->lineEdit_vdt->setText(vdt1);

        //calcul et affichage de cos phi
        float phi = p_abs / (sqrt(3) * tension1 * courant1 );
        QString phi1 = QString::number(phi);
        ui->lineEdit_cos->setText(phi1);

        //calcul et affichage de Qabs
        float q_abs = p_abs * tan(acos(phi));
        QString q_abs1 = QString::number(q_abs);
        ui->lineEdit_qabs->setText(q_abs1);

        //calcul et affichage de g
        float g = (1500 - vitesse1) / 1500;
        QString g1 = QString::number(g);
        ui->lineEdit_g->setText(g1);

        //calcul et affichage de Pjs
        float rs = 7;
        float pjs = 3 * rs * pow(courant1,2);
        QString pjs1 = QString::number(pjs);
        ui->lineEdit_pjs->setText(pjs1);

        //calcul et affichage de la resistance
        float ohm = (M_PI * vitesse1) / 30;
        QString ohm1 = QString::number(ohm);
        ui->lineEdit_ohm->setText(ohm1);

        //remise a zero de la variable qui contient la trame pour pouvoir recevoir la prochaine
        temp = 0;
    }
}

/**
 * @brief MainWindow::decal_octet
 * @param mesure_Octets
 * @return
 *
 */
unsigned char MainWindow::decal_octet(unsigned char mesure_Octets)
{
    //decalage de 4 bit sur la gauche
    octet1_modif = mesure_Octets << 4;

    return octet1_modif;
}

/**
 * @brief MainWindow::add_octet
 * @param octet1
 * @param octet2
 * @param valeur_max
 * @return
 *
 */
unsigned char MainWindow::add_octet(unsigned char octet1, unsigned char octet2, int valeur_max)
{
    float val_test;

    //addition des deux octets
    unsigned char valeur = octet1 | octet2;
    val_test = valeur;

    //conversion de la valeur
    valeur2 = (val_test * valeur_max) / 256;
    return valeur2;
}

/**
 * @brief MainWindow::on_DecoButton_clicked
 */
void MainWindow::on_DecoButton_clicked()
{
    //fermeture de la liaison serie
    serial->close();

    //arret du timer
    timer->stop();

    qDebug() << "deconnexion";

    test_connect = false;
}

/**
 * @brief MainWindow::on_connectButton_clicked
 */
void MainWindow::on_connectButton_clicked()
{
    //ouverture de la liaison serie
    serial->open(QIODevice::ReadWrite);
    qDebug() << "Connect";
    test_connect = true;
}

/**
 * @brief MainWindow::on_pushButton_stop_clicked
 */
void MainWindow::on_pushButton_stop_clicked()
{
    //arret du timer
    timer->stop();
}
