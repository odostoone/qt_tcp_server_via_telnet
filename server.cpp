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
    //test xD
}

void Server::show_all_connections(Client *client)
{
    //qDebug() << "Alle Verbindungen für Client" << client->new_socket->peerAddress().toString() << ":" << client->new_socket->peerPort();

    QString socketInfo;
    int count = 0;

    for (int i = 0; i < clienten.size(); ++i) {

        Client *c = clienten[i];
        socketInfo += "\vSocket      : " + QString::number(count)+
                "\r\n   Name         : " + c->user_name.toUtf8() +
                "\r\n   Peer-Adresse : " + c->new_socket->peerAddress().toString() +
                "\r\n   Peer-Port    : " + QString::number(c->new_socket->peerPort()) +
                "\r\n------------------------\r\v";
        count += 1;
    }

    QByteArray data = socketInfo.toUtf8();
    client->new_socket->write(data);


}
///////////////////////////////////////////////////////////////////////////////




