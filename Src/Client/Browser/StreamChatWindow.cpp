#include "StreamChatWindow.h"
#include "clientWindow.h"

#include <QDesktopWidget>
#include <QWebView>
#include <QWebSettings>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>


StreamChatWindow::StreamChatWindow(QWidget* parent) : MyWebView(parent)
{
    this->setObjectName("StreamChatWindow");

    this->setWindowTitle("Chat");
    this->setWindowIcon(QIcon(":/mainWindow/Discussion.ico"));
    this->setWindowFlags(Qt::Dialog);

    loadGeometry();


    if(!isCustomSizeActive)
    {
        QDesktopWidget desktop;
        QRect desktopSize = desktop.screenGeometry();

        this->setGeometry(desktopSize.width()-500, desktopSize.height()-800, 500, 400);

        isCustomSizeActive = true;
    }


    // Content changed signals were unreliable so pull for chat lines instead
    QTimer* parseChatTimer = new QTimer(this);
    parseChatTimer->start(500);
    
    connect(parseChatTimer , SIGNAL(timeout()), 
            this           , SLOT(parseChat()));


    connect(this->page()   , SIGNAL(loadFinished(bool)), 
            this           , SLOT(twitchOptimizations()));
}
StreamChatWindow::~StreamChatWindow()
{
}

void StreamChatWindow::twitchOptimizations()
{
    if(this->url().host() == "www.twitch.tv")
    {
        // kill the stupid javascript window size warning
        QWebElement warningElement = this->page()->mainFrame()->findFirstElement("p[id=\"small_notice\"]");
        warningElement.removeFromDocument();

        // Remove extra scrollbars
        this->page()->currentFrame()->setScrollBarPolicy(Qt::Horizontal , Qt::ScrollBarAlwaysOff);
        this->page()->currentFrame()->setScrollBarPolicy(Qt::Vertical   , Qt::ScrollBarAlwaysOff);
    }
}

void StreamChatWindow::parseChat()
{
    if(this->url().host() != "www.twitch.tv")
    {
        return;
    }


    QWebElement chatList = this->page()->mainFrame()->findFirstElement("ul[id=\"chat_line_list\"]");
    QWebElementCollection chatLineList = chatList.findAll("li");

    QStringList newChatLines;


    int i = std::max(0, chatLineList.count()-10); // only process the last 10 lines.. 

    for(; i < chatLineList.count(); i++)
    {
        QWebElement entry = chatLineList.at(i);

        QString id = entry.findFirst("a[class=\"nick\"]").attribute("id");
        QWebElementCollection chatLines = entry.findAll("[class=\"chat_line\"]");


        for(int j=0; j < chatLines.count(); j++)
        {
            QString lineId = id + "_" + QString::number(j);

            if(!oldLines.contains(lineId))
            {
                oldLines << lineId;

                QString text = chatLines.at(j).toPlainText();
                QString nick = entry.findFirst("a[class=\"nick\"]").toPlainText();

                newChatLines << ">" + nick + ": " + text;
            }
        }
    }


    if(!newChatLines.isEmpty())
    {
        newChatLines.prepend("writeChat"); // IPC message header..

        emit this->writeChat(newChatLines);
    }
}
