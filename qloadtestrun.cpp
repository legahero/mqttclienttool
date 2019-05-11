#include "qloadtestrun.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>

QLoadtestRun::QLoadtestRun(int id,QMqttClient * client) : m_id(id)
{    
    m_client=client;
}
QLoadtestRun::~QLoadtestRun() {
    //delete m_client;
    qDebug().noquote() << QString("~Task() with ID %1").arg(m_id); // 方便查看对象是否被 delete
}
void QLoadtestRun::run() {

    //m_client->moveToThread(QThread::currentThread());


    while(m_client->state() == QMqttClient::Connecting)
    {
        QThread::msleep(1000); //等待连接
    }
    qDebug()<<"client"<< QString::number(m_id)<<" state:"<<m_client->state();


    qDebug().noquote() << QString("Start thread %1 at %2").arg(m_id).arg(QDateTime::currentDateTime().toString("mm:ss.z"));

    qDebug()<<"client"<< QString::number(m_id)<<" state:"<<m_client->state();
    int qos=0;
    for (int i = 1; i <= 10; ++i) {
        QString topic="test/"+QString::number(m_id);
        QString payload="test----"+QString::number(i);
        qint32 iRet=m_client->publish(topic, payload.toUtf8());
        if( iRet== -1)
        {
            qDebug()<<"Could not publish message,topic:"<<topic<<" payload:"<<payload;
        }else {
            qDebug()<<"publish topic:"<<topic<<" payload:"<<payload;
        }
        QThread::msleep(50 + qrand() % 100); // 每个 run() 函数随机执行 [55, 1050] 毫秒，模拟耗时任务
    }


    qDebug().noquote() << QString("End thread %1 at %2").arg(m_id).arg(QDateTime::currentDateTime().toString("mm:ss.z"));
    delete m_client;
}
