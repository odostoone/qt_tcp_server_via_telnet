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
    explicit Client(QTcpSocket * socket, QObject * parent = nullptr);

    QTcpSocket * new_socket = nullptr;

public slots:
    void start_read();


public:
    void show_my_own_connection();
    void send_private_message(int targetIndex, const QString &message);
    void send_to_all(const QString &message);

    QString user_name;
    void set_name(const QString &username);

    void show_help();
    void send_private_to_name(const QString&username, const QString &message);
    void show_date();

    void closed_client_connection();
    void user_disconect();

    QTimer * timer = nullptr;

    int sec = 0;
    int min = 0;
    int hour = 0;



    void timer_start();
    void timer_stop();
    void timer_reset();
    void show_timer_time();



};


#endif // CLIENT_H
