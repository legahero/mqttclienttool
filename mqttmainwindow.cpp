#include "mqttmainwindow.h"
#include "ui_mqttmainwindow.h"
#include <QtCore/QDateTime>
#include <QtWidgets/QMessageBox>
#include "qloadtestrun.h"
#include <QThreadPool>
#include <QDebug>

MqttMainWindow::MqttMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MqttMainWindow)
{
    ui->setupUi(this);

    m_client             =  new QMqttClient(this);

    connect(m_client, &QMqttClient::stateChanged, this, &MqttMainWindow::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &MqttMainWindow::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" Received Topic: ")
                    + topic.name()
                    + QLatin1String(" Message: ")
                    + message
                    + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });
}

MqttMainWindow::~MqttMainWindow()
{

    delete ui;
}

void MqttMainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change(0-disconnect,1-connecting,2-connected):")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    ui->editLog->insertPlainText(content);
}

void MqttMainWindow::brokerDisconnected()
{
    ui->leHost->setEnabled(true);
    ui->sbPort->setEnabled(true);
    ui->connButton->setText(tr("Connect"));
}

void MqttMainWindow::on_connButton_clicked()
{
    //未连接服务器则连接
    if (m_client->state() == QMqttClient::Disconnected) {
        if(!ui->leUser->text().isEmpty())
            m_client->setUsername(ui->leUser->text());
        if(!ui->lePasswd->text().isEmpty())
            m_client->setPassword(ui->lePasswd->text());
        m_client->setKeepAlive(ui->sbKeepalive->value());

        m_client->setCleanSession(ui->cbCleanSess->isChecked());

        //Use directly setWillTopic and setWillMessage.
        if(!ui->leWillTopic->text().isEmpty() && !ui->teWillMsg->toPlainText().isEmpty()) {
            //New
            //API: void setWillTopic(const QString& willTopic);
            //API: void setWillQos(const quint8 willQos);
            //API: void setWillRetain(const bool willRetain);
            //API: void setWillMessage(const QString& willMessage);
            m_client->setWillTopic(ui->leWillTopic->text());
            m_client->setWillMessage(ui->teWillMsg->toPlainText().toUtf8());
        }
        m_client->setClientId(ui->leClientId->text());
        ui->connButton->setText(tr("Disconnect"));
        m_client->setHostname(ui->leHost->text());
        m_client->setPort(ui->sbPort->text().toInt());
        ui->leHost->setEnabled(false);
        ui->sbPort->setEnabled(false);
        m_client->connectToHost();
    } else {//断开连接
        ui->connButton->setText(tr("Connect"));
        ui->leHost->setEnabled(true);
        ui->sbPort->setEnabled(true);
        m_client->disconnectFromHost();
    }
}

void MqttMainWindow::on_pubButton_clicked()
{
    quint8 qos = ui->qOSComboBox->currentIndex();

    if (m_client->publish(ui->topicLineEdit->text(), ui->payloadEdit->toPlainText().toUtf8(),qos) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

void MqttMainWindow::on_subButton_clicked()
{
    quint8 qos = ui->qosComboBox->currentIndex();
    auto subscription = m_client->subscribe(ui->topicLineEdit_2->text(),qos);
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
}

void MqttMainWindow::on_buttonPing_clicked()
{
    ui->buttonPing->setEnabled(false);
    m_client->requestPing();
}

void MqttMainWindow::on_unsubButton_clicked()
{
    QString topic = ui->topicLineEdit_2->text();
    m_client->unsubscribe(topic);
}

void MqttMainWindow::on_loadtestButton_clicked()
{
    QString user=ui->leUser->text();
    QString pwd=ui->lePasswd->text();
    QString host=ui->leHost->text();
    int port=ui->sbPort->text().toInt();

    int Keepalive=ui->sbKeepalive->value();

    QMqttClient * client[100];//mqtt client指针
    for (int i = 1; i < 100; i++) {

        client[i]             =  new QMqttClient(this);

        client[i]->setClientId("CC"+QString::number(i));

        if(!user.isEmpty())
            client[i]->setUsername(user);
        if(!pwd.isEmpty())
            client[i]->setPassword(pwd);
        client[i]->setKeepAlive(Keepalive);

        client[i]->setCleanSession(0);

        client[i]->setHostname(host);
        client[i]->setPort(port);


        //qDebug()<<"client "<<QString::number(i)<<" to "<<host<<":"<<QString::number(port);
        client[i]->connectToHost();

        if (client[i]->state() == QMqttClient::Disconnected) {
            qDebug()<<"client "<<QString::number(i)<<" not connected, stop";
            return;
        }

        qDebug()<<"client"<< QString::number(i)<<" state:"<<client[i]->state();



        QLoadtestRun *task = new QLoadtestRun(i,client[i]); // 创建任务
        QThreadPool::globalInstance()->start(task); // 提交任务给线程池，在线程池中执行
    }


}

void MqttMainWindow::on_pubMuiButton_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected)
    {
        ui->connButton->click();
    }

    QString content1 = QDateTime::currentDateTime().toString()
                    + QLatin1String(":send begin ")
                     + QLatin1Char('\n');
    ui->editLog->insertPlainText(content1);

    quint8 qos = ui->qOSComboBox->currentIndex();

    int i = 1;
    for (i = 1; i < 100; i++) {
        QString topic="test/"+QString::number(i);
        QString payload="test----"+QString::number(i);

        qint32 iRet=m_client->publish(topic, payload.toUtf8(),qos);
        if( iRet== -1)
        {
            qDebug()<<"publish error,topic:"<<topic<<" payload:"<<payload;
        }else {
            qDebug()<<"publish ok,topic:"<<topic<<" payload:"<<payload;
        }
        QThread::msleep(50 + qrand() % 100);
    }

    QString content2 = QDateTime::currentDateTime().toString()
                    + QLatin1String(":send complite num")
                    + QString::number(i)
                    + QLatin1Char('\n');
    ui->editLog->insertPlainText(content2);
}
