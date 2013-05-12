#include "dialogtcpserver.h"
#include "ui_dialogtcpserver.h"

#define DEFAULT_PORT   31217

DialogTcpServer::DialogTcpServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTcpServer),
    tcpServer(new QTcpServer(this)),
    tcpClient(0),
    parserState(0),
    chkSum(0)
{
    ui->setupUi(this);
    qRegisterMetaType<ControlData>("ControlData");
    ui->btnClose->setEnabled(false);
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    foreach(QHostAddress addr , addrs){
        ui->serverIPList->addItem(addr.toString());
    }
    ui->serverPort->setText(tr("31217"));
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_new_connection()));
}

DialogTcpServer::~DialogTcpServer()
{
    delete ui;
}

void DialogTcpServer::on_btnListen_clicked()
{
    if(tcpServer->isListening()){
        tcpServer->close();
    }else{
        QString port = ui->serverPort->text();
        QString ip = ui->serverIPList->currentText();
        bool ok = false;
        int p = port.toInt(&ok);
        if(!ok) p = DEFAULT_PORT;
        ok = tcpServer->listen(QHostAddress(ip), p);
        if(ok){
            qDebug()<<QString::fromUtf8("监听")<<":"<<ip<<":"<<port<<" "<<QString::fromUtf8("成功");
        }else{
            qDebug()<<QString::fromUtf8("监听")<<":"<<ip<<":"<<port<<" "<<QString::fromUtf8("失败");
        }
    }

    if(tcpServer->isListening()){
        ui->btnListen->setText(QString::fromUtf8("关闭"));
    }else{
        ui->btnListen->setText(QString::fromUtf8("监听"));
    }
}

void DialogTcpServer::on_btnClose_clicked()
{
    if(tcpClient){
        tcpClient->close();
    }
}

void DialogTcpServer::tcp_new_connection()
{
    while(tcpServer->hasPendingConnections()){
        QTcpSocket* client = tcpServer->nextPendingConnection();
        if(tcpClient){
            qDebug()<<QString::fromUtf8("客户端已经连接，关闭新连接");
            client->close();
        }else{
            tcpClient = client;
            ui->clientIP->setText(tcpClient->peerAddress().toString());
            ui->clientPort->setText(QString("%1").arg(tcpClient->peerPort()));
            ui->btnClose->setEnabled(true);
            connect(tcpClient, SIGNAL(disconnected()), this, SLOT(tcp_client_disconnected()));
            connect(tcpClient, SIGNAL(readyRead()), this, SLOT(tcp_client_readyRead()));
        }
    }
}

void DialogTcpServer::tcp_client_disconnected()
{
    tcpClient = 0;
    ui->btnClose->setEnabled(false);
}

void DialogTcpServer::tcp_client_readyRead()
{
    if(!tcpClient) return;
    //parser_tcp_data(tcpClient->readAll());
    emit tcp_data(tcpClient->readAll());
}

union short_char{
    short s;
    struct{
        char l;
        char h;
    };
};

void DialogTcpServer::send_tcp_data(const QByteArray& data)
{
    if(tcpClient){
        tcpClient->write(data);
    }
}

void DialogTcpServer::parser_tcp_data(const QByteArray& data)
{
    foreach(char c, data){
        chkSum += (unsigned char)c;
        switch(parserState){
        case 0:
            if(c == 0x55){
                chkSum = 0x55;
                parserState = 1;
            }
            break;
        case 1:
            parserState = c == (char)0xaa ? 2 : 0;
            break;
        default:
            if(parserState>=4){
                int i = (parserState - 4) / 2;
                if(parserState&1){
                    ((short_char*)&currentData)[i].h = c;
                }else{
                    ((short_char*)&currentData)[i].l = c;
                }
            }
            parserState++;
            if(parserState>=16){
                if((chkSum & 0xff) == 0){
                    emit controlDataReady(currentData);
                }
                parserState = 0;
            }
            break;
        }
    }
}


