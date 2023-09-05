#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QTimer>
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
    const QString allChatMassage;

public slots:
    void new_connection();
    void getMessageFromClient(QString &message);

signals:
    void messageHasReceived(QString &message);
    void clientHasConnected();
    void clientHasDisconnected();


};


#endif // SERVER_H
