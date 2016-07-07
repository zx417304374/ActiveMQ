#include "ActiveMQModuleWidget.h"
#include "ui_ActiveMQModuleWidget.h"
#include <QSettings>
#include <QDebug>
//#include <QMessageBox>
#include <QDateTime>
#include <QElapsedTimer>

const QString REG_RUN = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

ActiveMQModuleWidget::ActiveMQModuleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActiveMQModuleWidget),
//    mLoader_("E:/Work/NewTask/Task19(ActiveMQ)/ExeTest/Plugin/NoticeMQ/NoticeMQPlugin.dll")
    mLoader_(QCoreApplication::applicationDirPath() + "/Plugin/ActiveMQModulePlugin.dll")
{
    ui->setupUi(this);

    mInterface = qobject_cast<Interface*>(mLoader_.instance());
//    mInterface->moveToThread(&m_QThread);
    qDebug() << "mInterface: "<< mInterface;
    qDebug() << "ActiveMQModulePlugin.isLoaded(): "<< mLoader_.isLoaded();
    ui->mOpenMQBtn_->setEnabled(true);
    ui->mCloseMQBtn_->setEnabled(false);
    connect(mInterface, SIGNAL(MQConsumerMsgReceived(const QString&, const QString&)),
            this, SLOT(showMsg(const QString&, const QString&)));
    connect(mInterface, SIGNAL(MQProducterMsgReceived(const QString&, const QString&)),
            this, SLOT(showMsg(const QString&, const QString&)));

    connect(mInterface, SIGNAL(MQStateChanged(const QString&, MQState)),
            this, SLOT(ChangeMQState(const QString&, MQState)));
    loadIni();

    //TestCode
    ui->mCLientIdEdit_->setText(QString("C" + ui->mSubNameEdit_->text()));
    QString selectorStr = QString("U%1='C%2'").arg((ui->mSubNameEdit_->text())).arg((ui->mSubNameEdit_->text()));
    ui->mSelectorEdit_->setText(selectorStr);
}

ActiveMQModuleWidget::~ActiveMQModuleWidget()
{
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
    mLoader_.unload();
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
    delete ui;
}

void ActiveMQModuleWidget::loadIni()
{
    QString pluginPath = QCoreApplication::applicationDirPath() + "/Plugin/NoticeMQ/"  ;
    QString pluginName = pluginPath + "/" + "NoticeMQPlugin.ini";
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << pluginName;

    QSettings settings(pluginName,QSettings::IniFormat);
    ui->mBrokerURIEdit_->setText(settings.value("brokerURI","failover:(tcp://localhost:61616/)").toString());
    ui->mIsUseTopicsCheckBox_->setChecked(settings.value("useTopics",false).toBool());
    ui->mSessionTransactedCheckBox_->setChecked(settings.value("sessionTransacted",false).toBool());
    ui->groupBox_topic_persistent->setChecked(settings.value("persistent",false).toBool());
    ui->mTopicNameEdit_->setText(settings.value("topicname","topic://NoticeTopic").toString());
    ui->mQueueNameEdit_->setText(settings.value("queuename","queue://NoticeQueue").toString());
    ui->lineEdit_username->setText(settings.value("username","").toString());
    ui->lineEdit_password->setText(settings.value("password","").toString());

}
void ActiveMQModuleWidget::openActiveMQConsumer()
{
    QVariantMap mqFuncArgMap;
    mqFuncArgMap["FuncName"] = "startConsumer";
    mqFuncArgMap["ArgCounts"] = 3;
    mqFuncArgMap["Arg1"] = "NoticeMQ";//插件名称去掉Plugin
    mqFuncArgMap["Arg2"] = ui->mCLientIdEdit_->text();
    mqFuncArgMap["Arg3"] = ui->mSubNameEdit_->text();

    QVariantMap retMap = mInterface->callFunc(mqFuncArgMap);
}

void ActiveMQModuleWidget::on_mOpenMQBtn__clicked()
{
    openActiveMQConsumer();
}

void ActiveMQModuleWidget::on_mCloseMQBtn__clicked()
{
    QVariantMap mqFuncArgMap;
    mqFuncArgMap["FuncName"] = "endConsumer";
    mqFuncArgMap["ArgCounts"] = 1;
    mqFuncArgMap["Arg1"] = "NoticeMQ";//插件名称去掉Plugin
    QVariantMap retMap = mInterface->callFunc(mqFuncArgMap);
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;

}

void ActiveMQModuleWidget::showMsg(const QString& aType, const QString& aData)
{
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__<<aType<<aData;
    QString str= QString("[%1][%2]: %3").arg(aType).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")).arg(aData);
    ui->mReceivedMsgList_->insertItem(0,str);
    if(ui->mReceivedMsgList_->count()>100)
        ui->mReceivedMsgList_->removeItemWidget(ui->mReceivedMsgList_->item(100));
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__<< "showMSg End";
}

void ActiveMQModuleWidget::ChangeMQState(const QString& aCurrentMQName, MQState aState)
{
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__<<aCurrentMQName<< aState;
    switch(aState)
    {
    case MQOPEN:
        showMsg(aCurrentMQName, QStringLiteral("MQ已启动"));
        ui->mOpenMQBtn_->setEnabled(false);
        ui->mCloseMQBtn_->setEnabled(true);
        break;
    case MQERROR:
        showMsg(aCurrentMQName, QStringLiteral("MQ发生错误"));
        ui->mOpenMQBtn_->setEnabled(true);
        ui->mCloseMQBtn_->setEnabled(false);
        break;
    case MQINTERRUPTED:
        showMsg(aCurrentMQName, QStringLiteral("MQ已中断连接，正在重连......"));
        break;
    case MQRESUMED:
        showMsg(aCurrentMQName, QStringLiteral("MQ已成功重连"));
        break;
    case MQCLOSE:
        showMsg(aCurrentMQName, QStringLiteral("MQ已关闭"));
        ui->mOpenMQBtn_->setEnabled(true);
        ui->mCloseMQBtn_->setEnabled(false);
        break;
    default:
        showMsg(aCurrentMQName, QStringLiteral("MQ状态异常"));
        break;
    }

}

void ActiveMQModuleWidget::on_mTestSendMsgToServerBtn__clicked()
{
    QVariantMap mqFuncArgMap;
//    QVariantMap retMap = mInterface->callFunc(mqFuncArgMap);
    for(int i=0; i< 100;++i)
    {
        mqFuncArgMap.clear();
    mqFuncArgMap["FuncName"] = "sendMsgToServer";
    mqFuncArgMap["ArgCounts"] = 2;
    mqFuncArgMap["Arg1"] = "NoticeMQ";//插件名称去掉Plugin
    //发送获取指定用户的未读消息数量
        mqFuncArgMap["Arg2"] = "4028c7f843710b7b0143710cc8070004,GetUnReadMsgCount" + QString::number(i);
    showMsg("ActiveMQModuleWidget", mqFuncArgMap["Arg2"].toString());
        mInterface->callFunc(mqFuncArgMap);
    }
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;

}

void ActiveMQModuleWidget::on_mSubNameEdit__textChanged(const QString &/*arg1*/)
{
    ui->mCLientIdEdit_->setText(QString("C" + ui->mSubNameEdit_->text()));
    QString selectorStr = QString("U%1='C%2'").arg((ui->mSubNameEdit_->text())).arg((ui->mSubNameEdit_->text()));
    ui->mSelectorEdit_->setText(selectorStr);
}
