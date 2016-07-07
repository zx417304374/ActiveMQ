#ifndef ActiveMQModuleWIDGET_H
#define ActiveMQModuleWIDGET_H

#include <QWidget>
#include <QPluginLoader>
#include "interface.h"
#include <QThread>

namespace Ui {
class ActiveMQModuleWidget;
}

class ActiveMQModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActiveMQModuleWidget(QWidget *parent = 0);
    ~ActiveMQModuleWidget();

private:
    void openActiveMQConsumer();
    void loadIni();

private slots:

    void on_mOpenMQBtn__clicked();

    void on_mCloseMQBtn__clicked();
    void showMsg(const QString& aType, const QString& aData);
    void ChangeMQState(const QString& aCurrentMQName, MQState aState);
    void on_mTestSendMsgToServerBtn__clicked();

    void on_mSubNameEdit__textChanged(const QString &);

private:
    Ui::ActiveMQModuleWidget *ui;
    QPluginLoader mLoader_;
    Interface* mInterface;
    QThread m_QThread;

};

#endif // ActiveMQModuleWIDGET_H
