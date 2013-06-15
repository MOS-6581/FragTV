#include "Playback.h"
#include "clientWindow.h"
#include "WidgetFlash.h"

#include <QtCore>
#include <QFileDialog>


Playback::Playback(QObject* parent) : QObject(parent), demoFile(NULL), demoFileStartTime(0), demoServerZeroTime(0)
{
    wolfcam = new QProcess();


    connect(CLIENTUI->wolfExePathField                 , SIGNAL(textChanged(QString)) , this , SLOT(setPaths())            );
    connect(CLIENTUI->wolfExePathBrowseButton          , SIGNAL(clicked())            , this , SLOT(getWolfExePath())      );
    connect(CLIENTUI->wolfRestartButton                , SIGNAL(clicked())            , this , SLOT(startWolfcam())        );
    connect(CLIENTUI->wolfCommandLineTestExecuteButton , SIGNAL(clicked())            , this , SLOT(testWolfcam())         );
    connect(CLIENTUI->dataMatrixTimeoutSpin            , SIGNAL(valueChanged(int))    , this , SLOT(setDecodeTimeout(int)) );


    connect( wolfcam , SIGNAL(stateChanged(QProcess::ProcessState)), 
             this    , SLOT(stateHandler(QProcess::ProcessState)));


    setPaths();


    // Set interval to 2x the timeout, we need to decode 2 screenshots, + 500ms margin
    int decodeTimeoutInterval = (CLIENTUI->dataMatrixTimeoutSpin->value() * 2) + 500;

    dataMatrixScannerTimer = new QTimer();
    dataMatrixScannerTimer->setInterval(decodeTimeoutInterval);

    connect( dataMatrixScannerTimer , SIGNAL(timeout()), 
             this                   , SLOT(getVideoOffset()));


    connect(CLIENTUI->syncResyncButton  , SIGNAL(clicked()) , dataMatrixScannerTimer , SLOT(start())          );
    connect(CLIENTUI->syncResyncButton  , SIGNAL(clicked()) , this                   , SLOT(getVideoOffset()) );
    connect(CLIENTUI->syncResyncButton2 , SIGNAL(clicked()) , dataMatrixScannerTimer , SLOT(start())          );
    connect(CLIENTUI->syncResyncButton2 , SIGNAL(clicked()) , this                   , SLOT(getVideoOffset()) );


    wolfLoadedFileScannerTimer = new QTimer();
    wolfLoadedFileScannerTimer->setInterval(500);

    connect( wolfLoadedFileScannerTimer, SIGNAL(timeout()), 
             this, SLOT(wolfcamLoadScanner()));
}
Playback::~Playback()
{
}


void Playback::setPaths()
{
    wolfExeFile = QFileInfo(CLIENTUI->wolfExePathField->text());

    if(!wolfExeFile.exists() || !wolfExeFile.isExecutable())
    {
        return;
    }


    exePath  = wolfExeFile.absoluteDir().absolutePath();


    QDir dir;
    dir.mkpath(exePath + "/wolfcam-ql/demos/FragTV/");
    dir.mkpath(exePath + "/wolfcam-ql/FragTVMessages/");
    dir.mkpath(exePath + "/FragTV/");


    prepWolfcam();
}
void Playback::getWolfExePath()
{
    QString path = QFileDialog::getOpenFileName(CLIENTUI->centralWidget, "Path to WolfcamQL executable file", CLIENTUI->wolfExePathField->text(), QString::null);

    if(!path.isNull())
    {
        CLIENTUI->wolfExePathField->setText(path);
    }
}

void Playback::setDemo(QString fileName, qint64 fileTimeStamp)
{
    resetWolfcam();


    demoFileStartTime   = fileTimeStamp;
    isDemoPlaybackReady = true;

    CLIENTUI->wolfRestartButton->setEnabled(true);


    wolfCommandLine.clear();
    wolfCommandLine << "+demo"   << "FragTV/" + fileName                                     ;

    wolfCommandLine << "+set"    << "fs_homepath"  << exePath + "/FragTV/"                   ;
    wolfCommandLine << "+set"    << "r_noborder"   << "0"                                  ;
    wolfCommandLine << "+set"    << "r_fullScreen" << "0"                                  ;
    wolfCommandLine << "+set"    << "in_nograb"    << "1"                                  ;

    wolfCommandLine << "+set"    << "FragTVMessage"  << "exec FragTVMessages/FragTVMessage0.cfg" ;
    wolfCommandLine << "+fxload" << "FragTVLoop.fx"                                          ;
    wolfCommandLine << "+runfx"  << "FragTVLoop"                                             ;


    if(CLIENTUI->wolfAutoRun->isChecked())
    {
        startWolfcam();
    }
}
void Playback::stopDemo()
{
    isDemoPlaybackReady = false;

    CLIENTUI->wolfStatusField->setText("Demo done, finishing playback");
    WidgetFlash widgetFlash;
    widgetFlash.green(CLIENTUI->wolfStatusField);

    wolfLoadedFileScannerTimer->stop();
    dataMatrixScannerTimer->stop();
}
void Playback::startWolfcam()
{
    if(!wolfExeFile.exists() || !wolfExeFile.isExecutable())
    {
        CLIENTUI->wolfStatusField->setText("ERROR: Cant find executable");
        WidgetFlash widgetFlash;
        widgetFlash.red(CLIENTUI->wolfStatusField);

        qDebug() << "WolfcamQL execute failed, can't find executable file";

        return;
    }


    resetWolfcam();


    qDebug() << "Starting WolfcamQL!";

    wolfcam->start(wolfExeFile.absoluteFilePath(), wolfCommandLine);

    CLIENTUI->wolfStatusField->setText("WolfcamQL loading..");
    WidgetFlash widgetFlash;
    widgetFlash.green(CLIENTUI->wolfStatusField);


    wolfLoadedFileScannerTimer->start();

    qDebug() << "WolfcamQL loading, scanning for ready signal";
}
void Playback::testWolfcam()
{
    if(!wolfExeFile.exists() || !wolfExeFile.isExecutable())
    {
        CLIENTUI->wolfStatusField->setText("ERROR: Cant find executable");
        WidgetFlash widgetFlash;
        widgetFlash.red(CLIENTUI->wolfStatusField);

        qDebug() << "Test execute failed, can't find executable file";

        return;
    }


    resetWolfcam();


    QStringList wolfTestCommandLine;

    wolfTestCommandLine << "+devmap" << "bloodrun"                           ;
    wolfTestCommandLine << "+set"    << "fs_homepath"  << exePath + "/FragTV/" ;
    wolfTestCommandLine << "+set"    << "r_noborder"   << "0"                ;
    wolfTestCommandLine << "+set"    << "r_fullScreen" << "0"                ;


    wolfcam->start(wolfExeFile.absoluteFilePath(), wolfTestCommandLine);


    CLIENTUI->wolfStatusField->setText("WolfcamQL test execute..");
    WidgetFlash widgetFlash;
    widgetFlash.yellow(CLIENTUI->wolfStatusField);
}

void Playback::prepWolfcam()
{
    // Init file for WolfcamQL: 
    // "all set up has been completed and all the game info and commands are available"
    QFile initFile(exePath + "/wolfcam-ql/cgamepostinit.cfg");

    if(!initFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not write init file";
        return; 
    }

    QString initContents =
        "\n" "AT NOW WOLFCAM DEMO READY"
        "\n" "SAVEAT FragTVWolfcamReady.txt";

    initFile.write(initContents.toLatin1());
    initFile.close();


    // Ghetto event loop for FragTV commands in WolfcamQL.. 
    QFile loopFxFile(exePath + "/wolfcam-ql/FragTVLoop.fx");

    if(!loopFxFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not write: " << loopFxFile.fileName() << loopFxFile.errorString();
        return; 
    }

    QString loopFx =
        "FragTVLoop {"
        "\n" "  command clearat" // tasks are not removed after they trigger
        "\n"
        "\n" "  t0 cgtime + 100"
        "\n" "  t1 cgtime + 150"
        "\n"
        "\n" "  command at %t0 vstr FragTVMessage" // execute new message files
        "\n" "  command at %t1 runfx FragTVLoop" // loop
        "\n" "}"
        "\n" "FragTVSync {"
        "\n" "  command clearat" // pause loop
        "\n" "  t0 1" // else condition is broken, use this as workaround
        "\n"
        "\n" "  if (cgtime+100 < FragTV_SyncTarget) {"
        "\n" "     t0 0"
        "\n" "     command at %FragTV_SyncTarget timescale 1" // Sync stop
        "\n" "     command at %FragTV_SyncTarget+500 echopopup SYNC FINISHED"
        "\n" "     command at %FragTV_SyncTarget+500 echo SYNC FINISHED"
        "\n"
        "\n" "     command at %FragTV_SyncTarget+200 runfx FragTVLoop" // restart loop
        "\n"
        "\n" "     command at now timescale %FragTV_SyncTimeScale" // Sync fast-forward
        "\n" "  }"
        "\n" "  if (t0) {"
        "\n" "     command at now echo ALREADY IN SYNC"
        "\n" "     command at now echopopup ALREADY IN SYNC"
        "\n" "     command at now runfx FragTVLoop"
        "\n" "  }"
        "\n" "}";

    loopFxFile.write(loopFx.toLatin1());
    loopFxFile.close();
}
void Playback::resetWolfcam()
{
    if(wolfcam->state() != QProcess::NotRunning)
    {
        qDebug() << "WolfcamQL already playing, terminating!";

        wolfcam->kill();
        wolfcam->waitForFinished();
    }


    QFile wolfReady(exePath + "/FragTV/wolfcam-ql/FragTVWolfcamReady.txt");

    if(wolfReady.exists())
    {
        wolfReady.remove();
        if(wolfReady.error())
        {
            qDebug() << "Failed to remove previous ready signal, WolfcamQL still running? error: " << wolfReady.errorString();
        }
        else
        {
            qDebug() << "Deleted previous signal file: " << wolfReady.fileName();
        }
    }


    QDirIterator messageIter(exePath + "/wolfcam-ql/FragTVMessages/");
    QDir dir;

    while(messageIter.hasNext())
    {
        messageIter.next();
        dir.remove(messageIter.filePath());
    }


    demoServerZeroTime = 0;
    messageCount = 0;

    isSyncedTwice = false;
}
void Playback::writeMessageFile(QStringList commands, bool execute)
{
    QString messageFileName = exePath + "/wolfcam-ql/FragTVMessages/FragTVMessage" + QString::number(messageCount) + ".cfg";
    QFile messageFile(messageFileName);

    if(!messageFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not write message file";
        return; 
    }


    QString nextMessageFileName = "FragTVMessages" + QString(QDir::separator()) + "FragTVMessage" + QString::number(messageCount+1) + ".cfg";

    QString messageContents = "\n" "set FragTVMessage \"exec " + nextMessageFileName + "\"";


    foreach(QString command, commands)
    {
        messageContents.append("\n" + command);
    }


    if(execute)
    {
        messageContents.append("\n" "vstr FragTVMessage");
    }


    messageFile.write(messageContents.toLatin1());
    messageFile.close();

    messageCount++;
}
void Playback::wolfcamLoadScanner()
{
    if(wolfcam->state() == QProcess::NotRunning)
    {
        qDebug() << "WolfcamQL error.. aborting ready scan";

        wolfLoadedFileScannerTimer->stop();

        return;
    }


    QFile wolfReadyFile(exePath + "/FragTV/wolfcam-ql/FragTVWolfcamReady.txt");

    if(wolfReadyFile.exists())
    {
        if(wolfReadyFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "WolfcamQL loaded, ready file found!";

            CLIENTUI->wolfStatusField->setText("WolfcamQL loaded! Scanning video for barcode..");
            WidgetFlash widgetFlash;
            widgetFlash.green(CLIENTUI->wolfStatusField);



            QString readyContents = wolfReadyFile.readAll();

            wolfReadyFile.remove();

            wolfLoadedFileScannerTimer->stop();


            QString demoStartTimeString = readyContents.split(" ").value(1);
            demoServerZeroTime = demoStartTimeString.split(".").value(0).toInt();


            dataMatrixScannerTimer->start();


            qDebug() << "Scanning video stream for DataMatrix signature";
        }
        else
        {
            qDebug() << "WolfcamQL loaded, but error opening ready file: " << wolfReadyFile.fileName() << " " << wolfReadyFile.errorString();

            CLIENTUI->wolfStatusField->setText("Unknown error, could not read ready file..");
            WidgetFlash widgetFlash;
            widgetFlash.red(CLIENTUI->wolfStatusField);
        }
    }
}

void Playback::demoNew(QString fileName, QByteArray contents, qint64 timeStamp)
{
    QFileInfo fileNameInfo(fileName);
    fileName = fileNameInfo.completeBaseName(); 
    fileName.append(".dm_73");


    lastDemoFileUpdate        = QDateTime::currentMSecsSinceEpoch();
    demoFile                  = new QFile(exePath + "/wolfcam-ql/demos/FragTV/" + fileName);
    currentDemoChunksRecieved = 1;


    if(!demoFile->open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open output file, aborting!";

        qDebug() << demoFile->fileName() << " " << demoFile->errorString();

        demoFile = NULL;

        return; 
    }


    qDebug() << "Writing: " << demoFile->fileName();

    demoFile->write(contents);
    demoFile->close();
    demoFile->open(QIODevice::Append); 


    setDemo(fileName, timeStamp);
}
void Playback::demoAppend(QString fileName, QByteArray contents)
{
    Q_UNUSED(fileName);

    if(!demoFile)
    {
        return;
    }


    int delay          = QDateTime::currentMSecsSinceEpoch() - lastDemoFileUpdate; 
    lastDemoFileUpdate = QDateTime::currentMSecsSinceEpoch();


    demoFile->write(contents);
    demoFile->flush();


    currentDemoChunksRecieved++;

    QByteArray chunkOffset = QByteArray::number(delay);
    QByteArray chunkSize   = QByteArray::number(contents.size());
    QByteArray chunksSent  = QByteArray::number(currentDemoChunksRecieved);

    qDebug() << "New demo chunk: " << chunkOffset << "ms offset (" << chunkSize << " bytes) chunks received: " << chunksSent;
}
void Playback::demoFinish()
{
    qDebug() << "Demo finished";

    stopDemo();
}

void Playback::getVideoOffset()
{
    if(CLIENTUI->syncManualOverrideCheck->isChecked())
    {
        setVideoOffset(0);
    }
    else
    {
        QStringList message("getVideoOffset");

        emit this->ipcGetVideoOffset(message);
    }
}
void Playback::setVideoOffset(int offset)
{
    // fast-forwarding takes time, after the first sync is finished there might still be a delay
    if(isSyncedTwice)
    {
        dataMatrixScannerTimer->stop();
    }
    else
    {
        isSyncedTwice = true;
    }


    if(CLIENTUI->syncManualOverrideCheck->isChecked())
    {
        offset = CLIENTUI->syncManualOverrideSpin->value();
    }
    else
    {
        CLIENTUI->syncAutoDetectedDelaySpin->setValue(offset);

        qDebug() << "DataMatrix found, offset: " << offset << "ms";
    }


    if(wolfcam->state() == QProcess::NotRunning)
    {
        qDebug() << "Sync aborted, WolfcamQL is not running";
        return;
    }


    // Convert the demo file's starting cgtime to a unix timestamp
    qint64 demoServerBootTime = demoFileStartTime - demoServerZeroTime;

    // Calculate the current unix time of the video stream
    qint64 streamCurrentTime = QDateTime::currentMSecsSinceEpoch() - offset;

    // Calculate the cgtime for the video position
    qint64 seekPos = streamCurrentTime - demoServerBootTime; 


    QStringList commands;

    QString safeTimescale = CLIENTUI->safeTimeScaleCheck->isChecked() ? "1" : "0";
    commands << "set com_timescaleSafe " + safeTimescale;

    commands << "set FragTV_SyncTimeScale " + QString::number(CLIENTUI->timeScaleSpin->value());
    commands << "set FragTV_SyncTarget " + QString::number(seekPos);
    commands << "runfx FragTVSync";

    writeMessageFile(commands, false);


    CLIENTUI->wolfStatusField->setText("WolfcamQL should be loaded and synced..");
    WidgetFlash widgetFlash;
    widgetFlash.green(CLIENTUI->wolfStatusField);

    qDebug() << "Writing SYNC message file, seekPos: " << seekPos << " offset: " << offset << "\n\n";
}

void Playback::writeChat(QStringList chatLines)
{
    bool chatEnabled  = CLIENTUI->chatEnableChatCheck->isChecked();
    bool chatBeeps    = CLIENTUI->chatBeepsCheck->isChecked();
    QString chatColor = CLIENTUI->chatColorSpin->text();
    QString beepSound = "sound/player/talk.ogg";


    if(wolfcam->state() != QProcess::Running || !chatEnabled)
    {
        return;
    }


    QStringList commands;

    foreach(QString line, chatLines)
    {
        commands << "addchatline ^" + chatColor + line; // fake chatline
        commands << "echo ^" + chatColor + line; // for console history

        if(chatBeeps)
        {
            commands << "play " + beepSound; // fake chatbeep
            commands << "wait 15"; // 150ms delay between each message
        }
    }

    writeMessageFile(commands, true);
}

void Playback::stateHandler(QProcess::ProcessState state)
{
    QProcess::ProcessError err = wolfcam->error();

    if(err != QProcess::UnknownError)
    {
        qDebug() << "Error starting WolfcamQL!";

        if(err == QProcess::FailedToStart) 
        {
            qDebug() << "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        }
        else if(err == QProcess::Crashed)
        {
            qDebug() << "The process crashed some time after starting successfully.";
        }
        else if(err == QProcess::Timedout)
        {
            qDebug() << "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again";
        }
        else if(err == QProcess::WriteError)
        {
            qDebug() << "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        }
        else if(err == QProcess::ReadError)
        {
            qDebug() << "An error occurred when attempting to read from the process. For example, the process may not be running.";
        }

        qDebug() << wolfcam->readAllStandardOutput();
        qDebug() << wolfcam->readAllStandardError();
    }

    if(state == QProcess::NotRunning && isDemoPlaybackReady)
    {
        CLIENTUI->wolfRestartButton->setEnabled(true);
        CLIENTUI->wolfStatusField->setText("WolfcamQL terminated before the demo finished?");
        WidgetFlash widgetFlash;
        widgetFlash.yellow(CLIENTUI->wolfStatusField);

        int index = CLIENTUI->tabWidget->indexOf(CLIENTUI->wolfcamTab);
        CLIENTUI->tabWidget->setCurrentIndex(index);

        qDebug() << "WolfcamQL terminated before the demo finished?";
    }

    if(state == QProcess::NotRunning && !isDemoPlaybackReady)
    {
        CLIENTUI->wolfRestartButton->setEnabled(true);
        CLIENTUI->wolfStatusField->setText("Playback finished, waiting for new demo..");
        WidgetFlash widgetFlash;
        widgetFlash.green(CLIENTUI->wolfStatusField);

        qDebug() << "Playback finished, waiting for new demo..";
    }
}

