#include "client.h"
#include "server.h"

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

    /*
    QList<QRegularExpression> regexList;
    regexList.append(QRegularExpression("^\\s*show\\s*user\\s*list\\\r\n\\s*$"));
    */
    // using ActionFunkction = std::function<void(QString, QString)>;
    //std::map<QString, ActionFunkction> regexActionMap;
    QMap<QString, std::function<void()>> regexActionMap;

    regexActionMap.insert("^\\s*show\\s*user\\s*list\\\r\n\\s*$",[&](){
        qDebug() << "hellp";
        show_all_connections(this);
    });
    regexActionMap.insert("^\\s*show\\s*my\\s*socket\\\r\n\\s*$",[&](){
        show_my_own_connection();
    });
    regexActionMap.insert("^\\s*show\\s*date\\\r\n\\s*$",[&](){
        show_date();
    });
    regexActionMap.insert("^\\s*help\\\r\n\\s*$",[&](){
        show_help();
    });
    regexActionMap.insert("^\\s*exit\\\r\n\\s*$",[&](){
        user_disconect();
    });
    regexActionMap.insert("^\\s*timer\\s*start\\\r\n\\s*$",[&](){
        timer_start();
    });
    regexActionMap.insert("^\\s*timer\\s*stop\\\r\n\\s*$",[&](){
        timer_stop();
    });
    regexActionMap.insert("^\\s*timer\\s*reset\\\r\n\\s*$",[&](){
        timer_reset();
    });
    regexActionMap.insert("^\\s*timer\\s*time\\\r\n\\s*$",[&](){
        show_timer_time();
    });
    regexActionMap.insert("^\\s*all\\a*:\\\r\\n\\s*$",[&](){

    });
    /*
    QMapIterator<QString, std::function<void()>> it(regexActionMap);

    while(it.hasNext()){
        it.next();
        const QString& pattern = it.key();
        qDebug() << "pattern: " << pattern;
        }
    */
    while(new_socket->canReadLine()){

        input_data = QString(new_socket->readLine());
        text_input_list.append(input_data);
        if(text_input_list.last().startsWith("send to name ")){

            QString username = text_input_list.last().split(" ")[3];
            //QString message = text_input_list.last().split(" ")[4];
            QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();

            send_private_to_name(username,message);

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

        for(const QString &text: text_input_list){
            for(const QString &regexStr : regexActionMap.keys()){
                QRegularExpression regex(regexStr);
                QRegularExpressionMatch match = regex.match(text);

                if(match.hasMatch()){
                    regexActionMap.value(regexStr)();
                    break;
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

    if(!check_if_user_exist(username)){
        user_name = username;
        QByteArray data = (user_name.toUtf8()+"\r\n");
        new_socket->write(data);
    }
    new_socket->write("Der Benutzer ist bereits belegt versuch einen anderen");

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
    //QString cleanedMessage = message.mid(QString(" ").length()).trimmed();


    QString cleanedMessage = message.mid(QString("send to name %1").arg(username).length()-4).trimmed();
    qDebug() << cleanedMessage;
    QString myname = QString(this->user_name).toUtf8();

    for (int i = 0; i < Server::clienten.size(); ++i) {

        Client *c = Server::clienten[i];
        if(c->user_name.toUtf8() == username){

            QByteArray data = ("["+myname+"]"+cleanedMessage + "\r\n").toUtf8();
            c->new_socket->write(data +"\r\n");
        }
    }
}

bool Client::check_if_user_exist(const QString &username)
{
    for (int i = 0; i < Server::clienten.size(); ++i) {

        Client *c = Server::clienten[i];
        if(c->user_name.toUtf8() == username){
            return true;
        }

    }
    return false;
}

void Client::show_date()
{
    new_socket->write(QDate::currentDate().toString().toUtf8()+"\r\n");
}

void Client::closed_client_connection()
{
    int myIndex = Server::clienten.indexOf(this);

    qDebug("wird gesloescht");
    new_socket->deleteLater();
    Server::clienten.remove(myIndex);
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

void Client::show_all_connections(Client *client)
{
    QString socketInfo;
    int count = 0;

    for (int i = 0; i < Server::clienten.size(); ++i) {

        Client *c = Server::clienten[i];
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




