#include "server.h"

QList<Client*> Server::clienten;

Server::Server(QObject *parent) : QObject(parent)
{
    tcp_server = new QTcpServer(this);
    tcp_server->listen(QHostAddress::Any, 50000);

    connect(tcp_server,&QTcpServer::newConnection,this,&Server::new_connection);

    qDebug("step1.0");


}

void Server::new_connection()
{
    new_socket = tcp_server->nextPendingConnection();
    new_clienten = new Client(new_socket);
    clienten.append(new_clienten);

    connect(Server::clienten[clienten.indexOf(new_clienten)],&Client::clientSendToAllChat,this,&Server::getMessageFromClient);
    emit clientHasConnected();

}

void Server::getMessageFromClient(QString &message)
{
    emit messageHasReceived(message);
}


///////////////////////////////////////////////////////////////////////////////




