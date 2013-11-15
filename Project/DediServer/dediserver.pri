
INCLUDEPATH += ../../Src/Server
INCLUDEPATH += ../../Src
INCLUDEPATH += ../../Src/Client

SOURCES += \
../../Src/Server/main.cpp \
../../Src/Server/DediServerUI.cpp \
../../Src/Server/FragServer.cpp \
../../Src/Server/TcpListener.cpp \
../../Src/Server/TcpConnectionWorker.cpp \
../../Src/Server/DemoScanner.cpp \
../../Src/Server/MessageBuilder.cpp \
../../Src/Server/Spectator.cpp \
../../Src/Server/SpectatorCommands.cpp \
../../Src/Server/TcpWorkerManager.cpp \
../../Src/Server/NetStats.cpp 

# Shared ones - but not including the shared file because we don't want all of the shared code
SOURCES += \
../../Src/Persistence.cpp \
../../Src/ThreadManager.cpp \
../../Src/MyThread.cpp \
../../Src/MyDebug.cpp

# Limited Client code to allow repeater functionality
SOURCES += \
../../Src/Client/TcpClient.cpp \
../../Src/Client/MessageParser.cpp \
../../Src/Client/Playback.cpp


HEADERS += \
../../Src/Server/DediServerUI.h \
../../Src/Server/FragServer.h \
../../Src/Server/TcpListener.h \
../../Src/Server/TcpConnectionWorker.h \
../../Src/Server/DemoScanner.h \
../../Src/Server/MessageBuilder.h \
../../Src/Server/Spectator.h \
../../Src/Server/SpectatorCommands.h \
../../Src/Server/TcpWorkerManager.h \
../../Src/Server/NetStats.h

HEADERS +=  \
../../Src/Persistence.h \
../../Src/ThreadManager.h \
../../Src/MyThread.h \
../../Src/MyDebug.h \
../../Src/FragEnums.h

HEADERS +=  \
../../Src/Client/TcpClient.h \
../../Src/Client/MessageParser.h \
../../Src/Client/Playback.h 

