#ifndef MQTTMAINWINDOW_H
#define MQTTMAINWINDOW_H

#include <QMainWindow>
#include <QtMqtt/QMqttClient>

namespace Ui {
class MqttMainWindow;
}

class MqttMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MqttMainWindow(QWidget *parent = nullptr);
    ~MqttMainWindow();

private slots:
    void on_connButton_clicked();
    void updateLogStateChange();
    void brokerDisconnected();
    void on_pubButton_clicked();

    void on_subButton_clicked();

    void on_buttonPing_clicked();

    void on_unsubButton_clicked();

    void on_loadtestButton_clicked();

    void on_pubMuiButton_clicked();

private:
    Ui::MqttMainWindow *ui;
     QMqttClient * m_client;//mqtt client指针
};

#endif // MQTTMAINWINDOW_H
