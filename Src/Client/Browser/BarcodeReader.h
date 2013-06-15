#ifndef BARCODEREADER_H
#define BARCODEREADER_H

#include "dmtx.h"

#include <QObject>
#include <QtCore>
#include <QImage>

class BarcodeReader : public QObject
{

    Q_OBJECT


public:

    BarcodeReader(int decodeTimeoutLimit);
    ~BarcodeReader();


private:

    int decodeTimeoutLimit;
    qint64 screenShotMyLocalTime;

    DmtxImage* convertImage(QImage image);
    QString decodeImage(DmtxImage* image);


public slots:

    void setDecodeTimeout(int timeoutLimit) { decodeTimeoutLimit = timeoutLimit; };

    void readTimeStamp(qint64 screenShotTime, QImage screenShot1, QImage screenShot2);


signals:

    void ipcVideoOffset(QStringList message);
    

};

#endif // BARCODEREADER_H
