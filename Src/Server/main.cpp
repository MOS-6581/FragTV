#include "FragServer.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    application.setOrganizationName("FragTV");
    application.setApplicationVersion("0.1.2");
    application.setApplicationName("FragServer");


    FragServer* fragServer = new FragServer(); 
    
    Q_UNUSED(fragServer);


    int exitCode = application.exec();

    return exitCode;
}
