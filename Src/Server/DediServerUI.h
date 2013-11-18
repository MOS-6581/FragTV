#ifndef DEDI_SERVER_UI_H
#define DEDI_SERVER_UI_H


#include "Persistence.h"

class DediServerUI 
{
public:

    explicit DediServerUI(Persistence *aPersistence);
    ~DediServerUI();
    static const DediServerUI *getInstance() { return singletonInstance; }
    
    int getLimitConnections() const { return persistence->getIntSetting("limitConnectionsSpin", 2); }
    int getMaximumClients() const { return persistence->getIntSetting("maxClientsSpin", 2000); }
    QString getListenIP() const { return persistence->getStringSetting("ipField", "127.0.0.1"); }
    int getListenPort() const { return persistence->getIntSetting("portField", 80); }
    bool getListenOnStartup() const { return persistence->getBoolSetting("listenOnStartupCheck", true); }

    int getMaxThrottle() const { return persistence->getIntSetting("throttleConnectionsSpin", 10); }
    QString getVideoURL() const { return persistence->getStringSetting("videoBrowserUrlField", ""); }
    QString getChatURL() const { return persistence->getStringSetting("chatBrowserUrlField", ""); }
    QString getMotdHtml() const;
    int getNumThreads() const { return persistence->getIntSetting("threadsSpin", 3); }

    bool getDemoScannerEnabled() const { return persistence->getBoolSetting("demoStartupScanCheck", true); }
    QString getDemoFolderPath() const { return persistence->getStringSetting("demoPathField", "/tmp"); }
    bool getDemoScannerDebug() const { return persistence->getBoolSetting("demoScannerDebugCheck", true); }

    QString getRemoteServerName() const { return persistence->getStringSetting("remoteServerName", ""); }
    QString getRemoteServerAddress() const { return persistence->getStringSetting("remoteServerAddress", ""); }

private:

    Persistence* persistence;
    static DediServerUI* singletonInstance;
};


#endif // NULL_SERVER_UI_H
