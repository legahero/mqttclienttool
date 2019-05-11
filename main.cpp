#include "mqttmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MqttMainWindow w;
    w.show();

    return a.exec();
}
