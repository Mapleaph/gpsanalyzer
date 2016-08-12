#ifndef GPSDATARETREIVER_H
#define GPSDATARETREIVER_H

#include <QObject>
#include <QSerialPort>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

class GPSDataRetreiver : public QObject
{
    Q_OBJECT
public:
    explicit GPSDataRetreiver(QObject *parent = 0);
    qint8 checksum(QString str);
    qint8 getDecPart(double num);
    double getFracPart(double num);
    void intervalGen(int interval);

signals:
    void sigOpened();
    void sigOpenFailed();
    void sigExitThread();
    void sigHaveResult(QStringList resultList);

public slots:
    void retreive();
    void doOpen(QString portName, qint32 baudrate);
    void doClose();

private:
    QSerialPort *port;

};

#endif // GPSDATARETREIVER_H
