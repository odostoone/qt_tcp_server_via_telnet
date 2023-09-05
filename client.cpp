#include "client.h"
#include "server.h"

Client::Client(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    this->new_socket = socket;
    socket->write("Hallo Client\r\v");

    connect(new_socket,&QTcpSocket::readyRead,this,&Client::startRead);
    connect(new_socket,&QTcpSocket::disconnected,this,&Client::cleanupConnections);
}

void Client::startRead()
{
    QStringList text_input_list;
    QString input_data;

    QMap<QString, std::function<void()>> regexActionMap;

    regexActionMap.insert("^\\s*show\\s*user\\s*list\\\r\n\\s*$",[&](){
        qDebug() << "hellp";
        showAllConnections(this);
    });
    regexActionMap.insert("^\\s*show\\s*my\\s*socket\\\r\n\\s*$",[&](){
        showMyOwnConnection();
    });
    regexActionMap.insert("^\\s*show\\s*date\\\r\n\\s*$",[&](){
        showCurrentDate();
    });
    regexActionMap.insert("^\\s*help\\\r\n\\s*$",[&](){
        showHelp();
    });
    regexActionMap.insert("^\\s*exit\\\r\n\\s*$",[&](){
        disconnectClient();
    });
    regexActionMap.insert("^\\s*timer\\s*start\\\r\n\\s*$",[&](){
        startTimer();
    });
    regexActionMap.insert("^\\s*timer\\s*stop\\\r\n\\s*$",[&](){
        stopTimer();
    });
    regexActionMap.insert("^\\s*timer\\s*reset\\\r\n\\s*$",[&](){
        resetTimer();
    });
    regexActionMap.insert("^\\s*timer\\s*time\\\r\n\\s*$",[&](){
        showTimerTime();
    });


    while(new_socket->canReadLine()){

        input_data = QString(new_socket->readLine());
        text_input_list.append(input_data);
        if(text_input_list.last().startsWith("send to name ")){

            QString username = text_input_list.last().split(" ")[3];
            //QString message = text_input_list.last().split(" ")[4];
            QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();

            sendPrivateMessageToName(username,message);

        }
        if (text_input_list.last().startsWith("send private to "))
        {
            int targetIndex = text_input_list.last().split(" ")[3].toInt();
            QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();

            sendMessageToSocket(targetIndex, message);
        }
        if(text_input_list.last().startsWith("all: "))
        {
            if(user_name != ""){
                QString message = text_input_list.join(" ").remove(0, text_input_list.last().indexOf(" ")).trimmed();
                sendMessageToAllChat(message);
                return;
            }
            this->new_socket->write("setze erst einen namen\r\n");

        }
        if(text_input_list.last().startsWith("setname: "))
        {

            QString username = text_input_list.join("").remove(0, text_input_list.last().indexOf(" ")).trimmed();
            changeName(username);
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

void Client::showMyOwnConnection()
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

void Client::sendMessageToSocket(int targetIndex, const QString &message)
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

void Client::sendMessageToAllChat(const QString &message)
{
    QString allChatString = QString("["+QTime::currentTime().toString().toUtf8()+"]["+this->user_name + "]\t"+
                                    message);
    emit this->clientSendToAllChat(allChatString);

    for (int i = 0; i < Server::clienten.size() ; ++i){
        Client * c = Server::clienten[i];

        QString cleantext = message;

        QByteArray data = ("["+QString(this->user_name).toUtf8()+"]: "+ cleantext.toUtf8() + "\r\n");
        c->new_socket->write(data);
    }
}

void Client::changeName(const QString &username)
{

    if(!checkIfNameExists(username)){
        user_name = username;
        QByteArray data = (user_name.toUtf8()+"\r\n");
        new_socket->write(data);

        return;
    }
    new_socket->write("Der Benutzer ist bereits belegt versuch einen anderen\r\n");

}

void Client::showHelp()
{
    QString help_messeg ="============================\r\n"
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

void Client::sendPrivateMessageToName(const QString &username, const QString &message)
{
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

bool Client::checkIfNameExists(const QString &username)
{
    for (int i = 0; i < Server::clienten.size(); ++i) {

        Client *c = Server::clienten[i];
        if(c->user_name.toUtf8() == username){
            return true;
        }

    }
    return false;
}

void Client::showCurrentDate()
{
    new_socket->write(QDate::currentDate().toString().toUtf8()+"\r\n");
}

void Client::cleanupConnections()
{
    int myIndex = Server::clienten.indexOf(this);

    qDebug("wird gesloescht");
    new_socket->deleteLater();
    Server::clienten.remove(myIndex);
    delete this;
}

void Client::disconnectClient()
{
    new_socket->disconnectFromHost();
}

void Client::startTimer()
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

void Client::stopTimer()
{
    timer->stop();
}

void Client::resetTimer()
{
    timer->stop();
    sec = 0;
    min = 0;
    hour = 0;
}

void Client::showTimerTime()
{
    QString timer_time = QString::number(hour) + " : " + QString::number(min) + " : "+ QString::number(sec)+"\r\n";
    QByteArray data = timer_time.toUtf8();
    new_socket->write(data);
}

void Client::showAllConnections(Client *client)
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





