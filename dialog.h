#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QPixmap>
#include <QStringList>
#include <QDebug>

#include "gpsdataretreiver.h"

#define SATELLITE_NUM 12

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog();

signals:
    void opened();
    void sigOpen(QString portName, qint32 baudrate);
    void sigClose();

private slots:
    void on_openClsBtn_clicked();
    void changeToOpen();
    void openFailedNotify();
    void exitThread();
    void updateUI(QStringList resultList);

private:

    // gpsDataBox and its sub-elements
    QGroupBox *gpsDataBox;
    QLineEdit *utc_le;
    QLineEdit *longitude_le;
    QLineEdit *latitude_le;
    QLineEdit *msl_le;
    QLineEdit *geoidsep_le;
    QLineEdit *spd_le;
    QLineEdit *spd_conv_le;
    QLineEdit *cog_le;
    QLineEdit *pdop_le;
    QLineEdit *hdop_le;
    QLineEdit *vdop_le;

    // serialPortBox and its sub-elements
    QGroupBox *serialPortBox;

    QLabel *serialPortSel_lb;
    QComboBox *serialPortSel;

    QLabel *baudrateSel_lb;
    QComboBox *baudrateSel;

    QPushButton *openClsBtn;

    QSerialPort *port;

    // satelliteBox and its sub-elements
    QGroupBox *satelliteBox;
    QLabel *satellite_lb[SATELLITE_NUM];
    QLineEdit *satellite[SATELLITE_NUM];

    void createGPSDataBox();
    void createSerialPortBox();
    void createSatelliteBox();

    QThread *gpsdr_thread;
    QTimer *gpsdr_timer;
    GPSDataRetreiver *gpsdr;

};

#endif // DIALOG_H
