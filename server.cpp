#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    tcp_server = new QTcpServer(this);
    tcp_server->listen(QHostAddress::Any, 50000);

    connect(tcp_server,&QTcpServer::newConnection,this,&Server::new_connection);
}

QList<Client*> Server::clienten;

void Server::new_connection()
{
    new_socket = tcp_server->nextPendingConnection();
    new_clienten = new Client(new_socket);
    clienten.append(new_clienten);
}
///////////////////////////////////////////////////////////////////////////////




