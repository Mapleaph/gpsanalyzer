#ifndef NMEA_H
#define NMEA_H

#include <QTime>
#include <QDate>

struct nmea_rmc {
    QString name;
    QTime hhmmss;
    QString status;

    double latitude;
    QString ns_indicator;
    double longitude;
    QString ew_indicator;
    double spd;
    double cog;
    QDate date;
    //double mv; // not being output by receiver according to the manual
    //QString mv_indicator; // not being output by receiver according to the manual
    QString mode;
    qint16 chksum;
};

struct nmea_gsa {
    QString name;
    QString smode;
    qint8 fix_status;
    qint8 sv[12];
    double pdop;
    double hdop;
    double vdop;
    qint16 chksum;
};

struct nmea_gsv_sv_status {
    qint8 id;
    double elv;
    double az;
    double cno;
};

struct nmea_gsv {
    QString name;
    qint8 msg_cnt;
    qint8 msg_no;
    qint8 sv_cnt;
    struct nmea_gsv_sv_status blk[4];
    qint16 chksum;
};

struct nmea_gga {
    QString name;
    QTime hhmmss;
    double latitude;
    QString ns_indicator;
    double longitude;
    QString ew_indicator;
    qint8 fix_status;
    qint8 sv_cnt;
    double hdop;
    double msl;
    QString msl_unit;
    double altref;
    QString altref_unit;
    double diff_age;
    qint8 diff_station_id;
    qint16 chksum;
};

#endif // NMEA_H

