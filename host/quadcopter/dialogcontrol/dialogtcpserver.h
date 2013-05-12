#ifndef DIALOGTCPSERVER_H
#define DIALOGTCPSERVER_H

#include <QDialog>
#include <QtNetwork>


struct ControlData{
    short button;
    short x1;
    short y1;
    short x2;
    short y2;
};


namespace Ui {
class DialogTcpServer;
}

class DialogTcpServer : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogTcpServer(QWidget *parent = 0);
    ~DialogTcpServer();
signals:
    void controlDataReady(ControlData controlData);
    void tcp_data(const QByteArray& data);
private slots:
    void on_btnListen_clicked();
    void tcp_new_connection();
    void tcp_client_disconnected();
    void tcp_client_readyRead();
    void on_btnClose_clicked();
public slots:
    void send_tcp_data(const QByteArray& data);

private:
    Ui::DialogTcpServer *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    int parserState;
    unsigned int chkSum;
    ControlData currentData;
    void parser_tcp_data(const QByteArray& data);
};

#endif // DIALOGTCPSERVER_H
