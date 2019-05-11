#ifndef QLOADTESTRUN_H
#define QLOADTESTRUN_H

#include <QRunnable>
#include <QtMqtt/QMqttClient>

class QLoadtestRun : public QRunnable
{
public:
    QLoadtestRun(int id,QMqttClient * client);

    ~QLoadtestRun();
    void run() Q_DECL_OVERRIDE;
private:
    int m_id; // 线程的 ID
    QMqttClient * m_client;//mqtt client指针
};

#endif // QLOADTESTRUN_H
