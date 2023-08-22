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
Client::Client(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    this->new_socket = socket;
    socket->write("Hallo Client\r\v");

    connect(new_socket,&QTcpSocket::readyRead,this,&Client::start_read);
    connect(new_socket,&QTcpSocket::disconnected,this,&Client::closed_client_connection);

}

void Client::start_read()
{
    QStringList text_input_list;
    QString input_data;

    QList<QRegularExpression> regexList;
    regexList.append(QRegularExpression("^\\s*show\\s*user\\s*list\\\r\n\\s*$"));

    while(new_socket->canReadLine()){

        input_data = QString(new_socket->readLine());
        text_input_list.append(input_data);
        /*
        if(text_input_list.contains(QRegularExpression("^\\s*show\\s*user\\s*list\\\r\n\\s*$")))
        {
            Server::show_all_connections(this);
        }

        if(text_input_list.contains("show my socket\r\n"))
        {
            show_my_own_connection();
        }
        if(text_input_list.contains("show date\r\n")){
            show_date();
        }

        if (text_input_list.last().startsWith("send private to "))
        {
            int targetIndex = text_input_list.last().split(" ")[3].toInt();
            QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();

            send_private_message(targetIndex, message);
        }
        if(text_input_list.last().startsWith("all: "))
        {

            QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();
            send_to_all(message);
        }
        if(text_input_list.last().startsWith("setname: "))
        {
            QString username = text_input_list.join("").remove(0, text_input_list.last().indexOf(" ")).trimmed();
            set_name(username);
        }
        if(text_input_list.contains("help\r\n")){
            show_help();
        }
        if(text_input_list.contains("exit\r\n"))
        {
            user_disconect();
        }
        if(text_input_list.contains("timer start\n")){
            qDebug("hallo");
            timer_start();
        }
        if(text_input_list.contains("timer stop\r\n")){
            timer_stop();
        }
        if(text_input_list.contains("timer reset\r\n")){
            timer_reset();
        }
        if(text_input_list.contains("timer time\r\n")){
            show_timer_time();
        }
        */
        for(const QRegularExpression &regex: regexList){
            for(const QString &text : text_input_list){
                if(regex.match(text).hasMatch()){
                  int index = regexList.indexOf(regex);
                  switch (index) {
                  case 0:
                      Server::show_all_connections(this);
                      break;
                  default:
                      break;
                  }
                }
            }
        }
    }

}

void Client::show_my_own_connection()
{
    int myIndex = Server::clienten.indexOf(this);

    if (myIndex != -1) {
        QString socketInfo;
        socketInfo += "\r\n   Meine Position in der Liste: " + QString::number(myIndex) +
                "\r\n   Zustand      : " + QString::number(new_socket->state()) +
                "\r\n   Peer-Adresse : " + new_socket->peerAddress().toString() +
                "\r\n   Peer-Port    : " + QString::number(new_socket->peerPort()) +
                "\r\n------------------------\r\v";

        QByteArray data = socketInfo.toUtf8();
        new_socket->write(data);
    } else {
        new_socket->write("nicht  gefunden");
    }
}

void Client::send_private_message(int targetIndex, const QString &message)
{
    if (targetIndex >= 0 && targetIndex < Server::clienten.size() && targetIndex != Server::clienten.indexOf(this)) {

        Client *targetClient = Server::clienten[targetIndex];

        QString senderInfo = "Private Nachricht von " + new_socket->peerAddress().toString() +
                ":" + QString::number(new_socket->peerPort()) +
                " (Absender: " + QString::number(Server::clienten.indexOf(this)) + ")\r\n";

        QString cleanedMessage = message.mid(QString("send private to %1").arg(targetIndex).length()-4).trimmed();

        QByteArray data = (senderInfo + cleanedMessage + "\r\n").toUtf8();
        targetClient->new_socket->write(data);

    } else {
        new_socket->write("Zielclient nicht gefunden");
    }
}

void Client::send_to_all(const QString &message)
{
    for (int i = 0; i < Server::clienten.size() ; ++i){
        Client * c = Server::clienten[i];

        QString cleantext = message;


        QByteArray data = ("["+QString(this->user_name).toUtf8()+"]: "+ cleantext.toUtf8() + "\r\n");
        c->new_socket->write(data);
    }
}

void Client::set_name(const QString &username)
{
    //int myIndex = Server::clienten.indexOf(this);

    user_name = username;
    QByteArray data = (user_name.toUtf8()+"\r\n");
    new_socket->write(data);

}

void Client::show_help()
{
    QString help_messeg = "============================\r\n"
                          "=            HELP          =\r\n"
                          "============================\r\n"


                          "=  setname: Your Name xD   =\r\n"
                          "=  show user list          =\r\n"
                          "=  send private to         =\r\n"
                          "=  show my socket          =\r\n"
                          "=  show user list          =\r\n"
                          "=  all:                    =\r\n"
                          "=  timer start             =\r\n"
                          "=  timer stop              =\r\n"
                          "=  timer reset             =\r\n"
                          "=  timer time              =\r\n"
                          "=  exit                    =\r\n"
                          "============================\r\n";
    new_socket->write(help_messeg.toUtf8());
}

void Client::send_private_to_name(const QString &username, const QString &message)
{

}

void Client::show_date()
{
    new_socket->write(QDate::currentDate().toString().toUtf8()+"\r\n");
}

void Client::closed_client_connection()
{
    qDebug("wird gesloescht");
    new_socket->deleteLater();
    delete this;
}

void Client::user_disconect()
{
    new_socket->disconnectFromHost();
}

void Client::timer_start()
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        sec += 1;
        if(sec == 60){
            sec = 0;
            min += 1;
            if(min == 60){
                min = 0;
                hour += 1;
                if(hour == 60){

                }
            }
        }
        qDebug()<<"time: "<<hour<<" : "<<min<<" : "<<sec;
    });
    timer->start(1000);
}

void Client::timer_stop()
{
    timer->stop();
}

void Client::timer_reset()
{
    timer->stop();
    sec = 0;
    min = 0;
    hour = 0;
}

void Client::show_timer_time()
{
    QString timer_time = QString::number(hour) + " : " + QString::number(min) + " : "+ QString::number(sec)+"\r\n";
    QByteArray data = timer_time.toUtf8();
    new_socket->write(data);
}




