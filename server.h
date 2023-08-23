#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QTimer>
#include <QRegularExpression>
#include <QMap>
#include <functional>
#include "client.h"


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    QTcpServer * tcp_server = nullptr;
    static QList<Client*> clienten;

    QTcpSocket * new_socket = nullptr;

    QPointer<Client>  new_clienten;

    static void show_all_connections(Client *client);


public slots:
        void new_connection();
public:


signals:

};


#endif // SERVER_H
