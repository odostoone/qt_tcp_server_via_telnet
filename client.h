#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QTimer>
#include <QRegularExpression>
#include <QMap>
#include <functional>


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QTcpSocket * socket, QObject * parent = nullptr) ;

    QTcpSocket * new_socket = nullptr;

public slots:
    void startRead();

public:
    void showMyOwnConnection();
    void sendMessageToSocket(int targetIndex, const QString &message);
    void sendMessageToAllChat(const QString &message);

    QString  user_name = "";


    void changeName(const QString &username);
    void showHelp();
    void sendPrivateMessageToName(const QString&username, const QString &message);
    bool checkIfNameExists(const QString &username);

    void showCurrentDate();
    void cleanupConnections();
    void disconnectClient();

    QTimer * timer = nullptr;
    int sec = 0;
    int min = 0;
    int hour = 0;

    void startTimer();
    void stopTimer();
    void resetTimer();
    void showTimerTime();
    void showAllConnections(Client *client);

signals:
    void clientSendToAllChat(QString &message);




};


#endif // CLIENT_H
