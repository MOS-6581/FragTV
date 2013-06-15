#include "FragClient.h"
#include "MyBrowser.h"

#include <QApplication>
#include <QtCore>


int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    application.setOrganizationName("FragTV");
    application.setApplicationName("FragClient");
    application.setApplicationVersion("0.1");


    QStringList args = application.arguments();

    if(args.contains("-browser", Qt::CaseInsensitive))
    {
        int ipcKeyPos = args.indexOf("-ipckey");
        QString ipcKey = args.value(ipcKeyPos + 1);

        int decodeTimeoutLimitPos = args.indexOf("-decodeTimeout");
        QString decodeTimeoutLimit = args.value(decodeTimeoutLimitPos + 1);


        MyBrowser* browser = new MyBrowser(ipcKey, decodeTimeoutLimit.toInt(), false);
        Q_UNUSED(browser);
    }
    else
    {
        FragClient* fragClient = new FragClient();
        Q_UNUSED(fragClient);
    }


    int exitCode = application.exec();

    return exitCode;
}
