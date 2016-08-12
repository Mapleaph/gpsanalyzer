#include <QtWidgets>
#include "dialog.h"

volatile bool openFlag = false;

Dialog::Dialog()
{

    port = NULL;

    QGridLayout *mainLayout = new QGridLayout;

    createGPSDataBox();
    createSerialPortBox();
    createSatelliteBox();

    connect(openClsBtn, SIGNAL(clicked()), this, SLOT(on_openClsBtn_clicked()));

    mainLayout->addWidget(serialPortBox, 0, 0, 1, 1);
    mainLayout->addWidget(satelliteBox, 1, 0, 1, 1);
    mainLayout->addWidget(gpsDataBox, 0, 1, 2, 1);

    QRect screenRec = QApplication::desktop()->screenGeometry();

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), screenRec));

    setLayout(mainLayout);

    setWindowTitle(tr("GPS Data Analyzer"));

    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setSizeGripEnabled(false);

}

void Dialog::createGPSDataBox()
{
    gpsDataBox = new QGroupBox(tr("Real-time GPS Data"));

    QFormLayout *layout = new QFormLayout;

    utc_le = new QLineEdit();
    longitude_le = new QLineEdit;
    latitude_le = new QLineEdit;
    msl_le = new QLineEdit;
    geoidsep_le = new QLineEdit;
    spd_le = new QLineEdit;
    spd_conv_le = new QLineEdit;
    cog_le = new QLineEdit;
    pdop_le = new QLineEdit;
    hdop_le = new QLineEdit;
    vdop_le = new QLineEdit;

    utc_le->setReadOnly(true);
    longitude_le->setReadOnly(true);
    latitude_le->setReadOnly(true);
    msl_le->setReadOnly(true);
    geoidsep_le->setReadOnly(true);
    spd_le->setReadOnly(true);
    spd_conv_le->setReadOnly(true);
    cog_le->setReadOnly(true);
    pdop_le->setReadOnly(true);
    hdop_le->setReadOnly(true);
    vdop_le->setReadOnly(true);


    layout->addRow(tr("UTC"), utc_le);
    layout->addRow(tr("Longitude"), longitude_le);
    layout->addRow(tr("Latitude"), latitude_le);
    layout->addRow(tr("MSL_Altitude"), msl_le);
    layout->addRow(tr("Geoid Sep"), geoidsep_le);
    layout->addRow(tr("SPD"), spd_le);
    layout->addRow(tr("SPD"), spd_conv_le);
    layout->addRow(tr("COG"), cog_le);
    layout->addRow(tr("PDOP"), pdop_le);
    layout->addRow(tr("HDOP"), hdop_le);
    layout->addRow(tr("VDOP"), vdop_le);

    layout->setLabelAlignment(Qt::AlignRight);

    gpsDataBox->setLayout(layout);

}

void Dialog::createSerialPortBox()
{

    serialPortBox = new QGroupBox(tr("Serial Port Settings"));

    QGridLayout *layout = new QGridLayout;

    serialPortSel_lb = new QLabel(tr("Port"));
    serialPortSel = new QComboBox;

    baudrateSel_lb = new QLabel(tr("Baudrate"));
    baudrateSel = new QComboBox;

    openClsBtn = new QPushButton(tr("Open"));

    layout->addWidget(serialPortSel_lb, 0, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(serialPortSel, 0, 1);
    layout->addWidget(baudrateSel_lb, 1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(baudrateSel, 1, 1);
    layout->addWidget(openClsBtn, 2, 1, 1, 1);

    serialPortBox->setLayout(layout);

    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();

    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {

        serialPortSel->addItem(serialPortInfo.portName());

    }

    QList<qint32> baudrateList = QSerialPortInfo::standardBaudRates();

    foreach (const qint32 baudrate, baudrateList) {

        baudrateSel->addItem(QString::number(baudrate));
    }

}

void Dialog::on_openClsBtn_clicked()
{

    if (!openFlag) {

        QString portName = serialPortSel->currentText();
        qint32 baudrate = baudrateSel->currentText().toInt();

        gpsdr = new GPSDataRetreiver;
        gpsdr_thread = new QThread;

        gpsdr->moveToThread(gpsdr_thread);

        connect(this, SIGNAL(sigOpen(QString, qint32)), gpsdr, SLOT(doOpen(QString, qint32)));
        connect(gpsdr, SIGNAL(sigOpened()), this, SLOT(changeToOpen()));
        connect(gpsdr, SIGNAL(sigOpenFailed()), this, SLOT(openFailedNotify()));
        connect(this, SIGNAL(sigClose()), gpsdr, SLOT(doClose()));
        connect(gpsdr, SIGNAL(sigExitThread()), this, SLOT(exitThread()));
        connect(gpsdr, SIGNAL(sigHaveResult(QStringList)), this, SLOT(updateUI(QStringList)));

        gpsdr_thread->start();

        emit sigOpen(portName, baudrate);

    } else {

        emit sigClose();
        openFlag = false;

        openClsBtn->setText(tr("Open"));
        serialPortSel->setDisabled(false);
        baudrateSel->setDisabled(false);


    }

}

void Dialog::openFailedNotify()
{
    QMessageBox::about(NULL, tr("Warning"), tr("Open Com Port Failed!"));
    openFlag = false;

}

void Dialog::changeToOpen()
{
    openFlag = true;

    openClsBtn->setText(tr("Close"));
    serialPortSel->setDisabled(true);
    baudrateSel->setDisabled(true);

}

void Dialog::exitThread()
{
    gpsdr_thread->exit();
    gpsdr_thread->wait();
}

void Dialog::createSatelliteBox()
{
    qint32 i;

    QPixmap red(":/images/red");

    for (i=0; i<SATELLITE_NUM; i++) {

        satellite_lb[i] = new QLabel;
        satellite_lb[i]->setPixmap(red);
        satellite[i] = new QLineEdit;
        satellite[i]->setReadOnly(true);

    }

    QGridLayout *layout = new QGridLayout;

    for (i=0; i<SATELLITE_NUM; i+=SATELLITE_NUM/2) {

        layout->addWidget(satellite_lb[i], i % 6, i/2);
        layout->addWidget(satellite_lb[i+1], i % 6 + 1, i/2);
        layout->addWidget(satellite_lb[i+2], i % 6 + 2, i/2);
        layout->addWidget(satellite_lb[i+3], i % 6 + 3, i/2);
        layout->addWidget(satellite_lb[i+4], i % 6 + 4, i/2);
        layout->addWidget(satellite_lb[i+5], i % 6 + 5, i/2);

        layout->addWidget(satellite[i], i % 6, i / 2 + 1);
        layout->addWidget(satellite[i+1], i % 6 + 1, i / 2 + 1);
        layout->addWidget(satellite[i+2], i % 6 + 2, i / 2 + 1);
        layout->addWidget(satellite[i+3], i % 6 + 3, i / 2 + 1);
        layout->addWidget(satellite[i+4], i % 6 + 4, i / 2 + 1);
        layout->addWidget(satellite[i+5], i % 6 + 5, i / 2 + 1);

    }

    satelliteBox = new QGroupBox(tr("Satellite Detection"));

    satelliteBox->setLayout(layout);

}

void Dialog::updateUI(QStringList resultList)
{
    qDebug() << "into updateUI";

    QPixmap red(":/images/red");
    QPixmap green(":/images/green");

    if (resultList.at(0) == "GPGGA") {

        msl_le->setText(resultList.at(1));
        geoidsep_le->setText(resultList.at(2));

    } else if (resultList.at(0) == "GPGSA") {

        for (int i=0; i<SATELLITE_NUM; i++) {

            if (resultList.at(i+1) != "NULL") {

                satellite_lb[i]->setPixmap(green);
                satellite[i]->setText(resultList.at(i+1));

            } else {

                satellite_lb[i]->setPixmap(red);

            }
        }

        pdop_le->setText(resultList.at(resultList.size()-3));
        hdop_le->setText(resultList.at(resultList.size()-2));
        vdop_le->setText(resultList.at(resultList.size()-1));

    } else if (resultList.at(0) == "GPRMC") {

        utc_le->setText(resultList.at(1));
        latitude_le->setText(resultList.at(2));
        longitude_le->setText(resultList.at(3));
        spd_le->setText(resultList.at(4));
        spd_conv_le->setText(resultList.at(5));
        cog_le->setText(resultList.at(6));
    }

}
