#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    new_server = new Server(this);

    connect(new_server, &Server::messageHasReceived,this,&Widget::updateTextBrowser);
    connect(new_server,&Server::clientHasConnected,this,&Widget::updateTextBrowserUser);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::updateTextBrowser(QString &message)
{
    qDebug("was geht");
    ui->textBrowser->insertHtml(message.toHtmlEscaped() + "<br>");
    qDebug() << message;

}

void Widget::updateTextBrowserUser()
{
    ui->textBrowser_user->clear();
    for (int i = 0; i < Server::clienten.size(); ++i) {

        Client *c = Server::clienten[i];
        QString socketInfo = "\vSocket      : " + QString::number(i)+
                      "\r\n   Name         : " + c->user_name.toUtf8() +
                      "\r\n   Peer-Adresse : " + c->new_socket->peerAddress().toString() +
                      "\r\n   Peer-Port    : " + QString::number(c->new_socket->peerPort()) +
                      "\r\n------------------------\r\v";

        ui->textBrowser_user->insertHtml(socketInfo.toHtmlEscaped() +"<br>");
    }

}


