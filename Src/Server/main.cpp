#include "FragServer.h"

#ifdef QT_GUI_LIB
#include <QApplication>
#endif


int main(int argc, char *argv[])
{
#ifdef QT_GUI_LIB
    QApplication application(argc, argv);
#else
    QCoreApplication application(argc, argv);
#endif // QT_GUI_LIB

    application.setOrganizationName("FragTV");
    application.setApplicationVersion("0.1");
    application.setApplicationName("FragServer");


    FragServer* fragServer = new FragServer(); 
    
    Q_UNUSED(fragServer);


    int exitCode = application.exec();

    return exitCode;
}
