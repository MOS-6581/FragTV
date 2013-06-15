#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <QObject>
#include <QtCore>

class Playback : public QObject
{

    Q_OBJECT


public:

    Playback(QObject* parent = 0);
    ~Playback();


private:


    QProcess* wolfcam;
    QStringList wolfCommandLine;

    QFileInfo wolfExeFile;
    QString exePath;


    int messageCount;


    QFile* demoFile;
    bool isDemoPlaybackReady;
    int currentDemoChunksRecieved;
    quint64 lastDemoFileUpdate;


    QTimer* dataMatrixScannerTimer;
    QTimer* wolfLoadedFileScannerTimer;
    bool isSyncedTwice;

    qint64 demoFileStartTime;
    int demoServerZeroTime;


public slots:

    void setPaths();
    void getWolfExePath();

    void prepWolfcam();
    void resetWolfcam();
    void writeMessageFile(QStringList commands, bool execute = true);
    void wolfcamLoadScanner();

    void demoNew(QString fileName, QByteArray contents, qint64 timeStamp);
    void demoAppend(QString fileName, QByteArray contents);
    void demoFinish();

    void setDemo(QString fileName, qint64 fileTimeStamp);
    void stopDemo();
    void startWolfcam();
    void testWolfcam();

    void setDecodeTimeout(int value) { dataMatrixScannerTimer->setInterval(value); };

    void getVideoOffset();
    void setVideoOffset(int offset);

    void writeChat(QStringList chatLines);

    void stateHandler(QProcess::ProcessState state);


signals:

    void ipcGetVideoOffset(QStringList message);


};

#endif // PLAYBACK_H