#ifndef DEDI_SERVER_UI_H
#define DEDI_SERVER_UI_H


#include "Persistence.h"

class DediServerUI 
{
public:

    explicit DediServerUI(Persistence *aPersistence);
    ~DediServerUI();
    static const DediServerUI *getInstance() { return singletonInstance; }
    
    int getLimitConnections() const { return persistence->getIntSetting("limitConnectionsSpin"); }
    int getMaximumClients() const { return persistence->getIntSetting("maxClientsSpin"); }
    QString getListenIP() const { return persistence->getStringSetting("ipField"); }
    int getListenPort() const { return persistence->getIntSetting("portField"); }
    bool getListenOnStartup() const { return persistence->getBoolSetting("listenOnStartupCheck"); }

    int getMaxThrottle() const { return persistence->getIntSetting("throttleConnectionsSpin"); }
    QString getVideoURL() const { return persistence->getStringSetting("videoBrowserUrlField"); }
    QString getChatURL() const { return persistence->getStringSetting("chatBrowserUrlField"); }
    QString getMotdHtml() const;
    int getNumThreads() const { return persistence->getIntSetting("threadsSpin"); }

    bool getDemoScannerEnabled() const { return persistence->getBoolSetting("demoStartupScanCheck"); }
    QString getDemoFolderPath() const { return persistence->getStringSetting("demoPathField"); }
    bool getDemoScannerDebug() const { return persistence->getBoolSetting("demoScannerDebugCheck"); }


private:

    Persistence* persistence;
    static DediServerUI* singletonInstance;
};


#endif // NULL_SERVER_UI_H
