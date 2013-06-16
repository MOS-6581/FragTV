#ifndef DEMOSCANNER_H
#define DEMOSCANNER_H


#include <QtCore>
#include <QObject>


struct demoChunk
{
    QByteArray message;
    int messageType; 
    qint64 readTime;
};


class DemoScanner : public QObject
{

    Q_OBJECT


public:

    DemoScanner();
    ~DemoScanner();


    QString demoFolderPath;

    bool debugging;


private:

    QStringList oldDemoFolderContents;
    QFileSystemWatcher* demoFolderWatcher; 

    QTimer* demoFeedTimer;
    qint64 lastFileUpdateTime;
    int currentDemoReadPosition;
    int chunksRead;

    bool isStreaming;
    QString currentDemoFileName;
    QFile* currentDemoFile;

    bool delayData;
    int delayDuration;
    QList<demoChunk> delayedChunks;


public slots:

    void setDemoFolderPath(QString path) { demoFolderPath = path; };
    void setDebugging(int state) { debugging = state ? true : false; };

    void demoScannerStart();
    void demoScannerStop();
    void checkForNewDemo(QString path);

    void readNewDemo(QString fileName);
    void feedDemo();
    void demoFinished();

    void sendDelayedData();


signals:

    void newDemo(QByteArray message);
    void appendDemo(QByteArray message);
    void finishDemo(QByteArray message);

    void setStatusMessage(QString statusMessage);
    void setStatus(bool scanning);

};

#endif