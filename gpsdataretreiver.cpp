#include "gpsdataretreiver.h"
#include "nmea.h"

bool retreiveFlag = true;

GPSDataRetreiver::GPSDataRetreiver(QObject *parent) : QObject(parent)
{

}

void GPSDataRetreiver::retreive()
{
    qDebug() << "retreive";


    QRegExp rx("[^$].*");

    QStringList resultList;

    while (retreiveFlag) {

        if (port->canReadLine()) {

            QByteArray line = port->readLine().trimmed();
            QString str(line);
            bool match = rx.exactMatch(str);
            qDebug() << match;

            if (str != "" && !match) {
                QStringList arr = str.split("$")[1].split("*");

                qint16 chksum = arr[1].toInt(0, 16);
                QStringList subarr = arr[0].split(",");

                for (qint16 i=0; i<subarr.size(); i++) {
                    qDebug()<< subarr[i];
                }

                qDebug() << chksum;

                if (chksum != checksum(arr[0])) {
                    continue;
                }

                qDebug() << "-----------------------------";

                if (subarr[0] == "GPGGA") {
                    struct nmea_gga cmd;

                    cmd.name = subarr[0];
                    cmd.hhmmss = (subarr[1] != "") ? (QTime::fromString(subarr[1], "hhmmss.z")) : QTime(0,0,0,0);
                    cmd.latitude = subarr[2].toDouble() / 100;
                    cmd.ns_indicator = subarr[3];
                    cmd.longitude = subarr[4].toDouble() / 100;
                    cmd.ew_indicator = subarr[5];
                    cmd.fix_status = subarr[6].toInt();
                    cmd.sv_cnt = subarr[7].toInt();
                    cmd.hdop = subarr[8].toDouble();
                    cmd.msl = subarr[9].toDouble();
                    cmd.msl_unit = subarr[10];
                    cmd.altref = subarr[11].toDouble();
                    cmd.altref_unit = subarr[12];
                    cmd.diff_age = subarr[13].toDouble();
                    cmd.diff_station_id = subarr[14].toInt();
                    cmd.chksum = chksum;

                    qDebug() << "1.name:" << cmd.name;
                    qDebug() << "2.hhmmss:" << cmd.hhmmss.toString("hhmmss.zzz");
                    qDebug() << "3.latitude:" << QString("%1").arg(cmd.latitude, 0, 'g', 9);
                    qDebug() << "4.N/S:" << cmd.ns_indicator;
                    qDebug() << "5.longitude:" << QString("%1").arg(cmd.longitude, 0, 'g', 10);
                    qDebug() << "6.E/W:" << cmd.ew_indicator;
                    qDebug() << "7.NoSV:" << cmd.sv_cnt;
                    qDebug() << "8.msl" << cmd.msl;
                    qDebug() << "9.umsl" << cmd.msl_unit;
                    qDebug() << "10.altref:" << cmd.altref;
                    qDebug() << "11.usep:" << cmd.altref_unit;
                    qDebug() << "12.diffage:" << cmd.diff_age;
                    qDebug() << "13.diffstation:" << cmd.diff_station_id;
                    qDebug() << "14.CS:" << cmd.chksum;
                    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++";


                    resultList << cmd.name;
                    resultList << QString::number(cmd.msl, 'f', 2) + " " + cmd.msl_unit;
                    resultList << QString::number(cmd.altref, 'f', 2) + " " + cmd.altref_unit;


                }

                if (subarr[0] == "GPRMC") {

                    struct nmea_rmc cmd;
                    cmd.name = subarr[0];
                    cmd.hhmmss = (subarr[1] != "") ? (QTime::fromString(subarr[1], "hhmmss.z")) : QTime(0,0,0,0);
                    cmd.status = subarr[2];
                    cmd.latitude = subarr[3].toDouble() / 100;
                    cmd.ns_indicator = subarr[4];
                    cmd.longitude = subarr[5].toDouble() / 100;
                    cmd.ew_indicator = subarr[6];
                    cmd.spd = subarr[7].toDouble();
                    cmd.cog = subarr[8].toDouble();
                    cmd.date = QDate::fromString(subarr[9], "ddMMyy");
                    cmd.mode = subarr[12];
                    cmd.chksum = chksum;

                    qDebug() << "1.name:" << cmd.name;
                    qDebug() << "2.hhmmss:" << cmd.hhmmss.toString("hhmmss.zzz");
                    qDebug() << "3.status:" << cmd.status;
                    qDebug() << "4.latitude:" << QString("%1").arg(cmd.latitude, 0, 'g', 9);
                    qDebug() << "5.N/S:" << cmd.ns_indicator;
                    qDebug() << "6.longitude:" << QString("%1").arg(cmd.longitude, 0, 'g', 10);
                    qDebug() << "7.E/W:" << cmd.ew_indicator;
                    qDebug() << "8.SPD:" << cmd.spd;
                    qDebug() << "9.COG" << cmd.cog;
                    qDebug() << "10.date:" << cmd.date.toString("yy-MM-dd");
                    qDebug() << "11.mv:" << "NULL";
                    qDebug() << "12.mvE:" << "NULL";
                    qDebug() << "13.mode:" << cmd.mode;
                    qDebug() << "14.CS:" << cmd.chksum;
                    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++";


                    qint8 dec_part_lat = getDecPart(cmd.latitude);
                    double frac_part_lat = getFracPart(cmd.latitude);

                    qint8 dec_part_long = getDecPart(cmd.longitude);
                    double frac_part_long = getFracPart(cmd.longitude);

                    resultList << cmd.name;

                    if (subarr[9] == "" || subarr[2] == "") {
                        resultList << "0000-00-00 00:00:00.000";
                    } else {
                        resultList << cmd.date.toString("yy-MM-dd") + " " + cmd.hhmmss.toString("hh:mm:ss.zzz");
                    }

                    resultList << QString::number(dec_part_long) + "°" + QString::number(frac_part_long*100) + "'" + " " + cmd.ew_indicator;
                    resultList << QString::number(dec_part_lat) + "°" + QString::number(frac_part_lat*100) + "'" + " " + cmd.ns_indicator;
                    resultList << QString::number(cmd.spd) + " " + "kn";
                    resultList << QString::number(cmd.spd * 1.852, 'f', 3) + " " + "km/s";
                    resultList << QString::number(cmd.cog) + "°";

                }

                if (subarr[0] == "GPGSA") {

                    struct nmea_gsa cmd;
                    cmd.name = subarr[0];
                    cmd.smode = subarr[1];
                    cmd.fix_status = subarr[2].toInt();
                    cmd.sv[0] = subarr[3].toInt();
                    cmd.sv[1] = subarr[4].toInt();
                    cmd.sv[2] = subarr[5].toInt();
                    cmd.sv[3] = subarr[6].toInt();
                    cmd.sv[4] = subarr[7].toInt();
                    cmd.sv[5] = subarr[8].toInt();
                    cmd.sv[6] = subarr[9].toInt();
                    cmd.sv[7] = subarr[10].toInt();
                    cmd.sv[8] = subarr[11].toInt();
                    cmd.sv[9] = subarr[12].toInt();
                    cmd.sv[10] = subarr[13].toInt();
                    cmd.sv[11] = subarr[14].toInt();
                    cmd.pdop = subarr[15].toDouble();
                    cmd.hdop = subarr[16].toDouble();
                    cmd.vdop = subarr[17].toDouble();
                    cmd.chksum = chksum;

                    qDebug() << cmd.name;
                    qDebug() << cmd.smode;
                    qDebug() << cmd.fix_status;

                    for (qint8 i=0; i<12; i++) {
                        qDebug() << cmd.sv[i];
                    }
                    qDebug() << cmd.pdop;
                    qDebug() << cmd.hdop;
                    qDebug() << cmd.vdop;
                    qDebug() << chksum;

                    resultList << cmd.name;

                    for (int i=0; i<12; i++) {
                        if (cmd.sv[i] != 0) {
                            resultList << QString::number(cmd.sv[i]);
                        } else {
                            resultList << "NULL";
                        }
                    }

                    resultList << QString::number(cmd.pdop);
                    resultList << QString::number(cmd.hdop);
                    resultList << QString::number(cmd.vdop);

                }

                emit sigHaveResult(resultList);
                resultList.clear();
            }
        }
        intervalGen(100);
    }
}

void GPSDataRetreiver::doOpen(QString portName, qint32 baudrate)
{
    qDebug() << "into doOpen";

    qDebug() << portName;

    port = new QSerialPort(portName);

    port->setBaudRate(baudrate);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (port->open(QIODevice::ReadWrite)) {

        emit sigOpened();

        retreive();


    } else {

        emit sigOpenFailed();

    }

    qDebug() << "out of doOpen";

}

void GPSDataRetreiver::doClose()
{
    if (port && port->isOpen()) {

        port->close();

    }

    if (port) {

        // add this line to stop the infinite loop
        retreiveFlag = false;
        emit sigExitThread();

    }
}

qint8 GPSDataRetreiver::checksum(QString str)
{
    QChar* qchr = str.data();

    int cs = 0;

    while (!qchr->isNull()) {

        QString tmpstr(*qchr);

        int dec = tmpstr.toLatin1().toHex().toInt(0, 16);

        cs = cs ^ dec;

        ++qchr;
    }

    //qDebug() << cs;

    return cs;
}

void GPSDataRetreiver::intervalGen(int interval)
{
    QEventLoop loop;
    QTimer::singleShot(interval, &loop, SLOT(quit()));
    loop.exec();
}

qint8 GPSDataRetreiver::getDecPart(double num)
{
    qint8 dec_part = (int)num;

    if (dec_part > num) {
        dec_part -= 1;
    }

    return dec_part;
}

double GPSDataRetreiver::getFracPart(double num)
{
    qint8 dec_part = getDecPart(num);

    return num - dec_part;
}
