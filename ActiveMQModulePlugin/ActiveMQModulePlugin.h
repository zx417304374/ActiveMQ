#ifndef ACTIVEMQMODULEPLUGIN_H
#define ACTIVEMQMODULEPLUGIN_H

#include "Interface.h"
#include <QPluginLoader>
#include <qplugin.h>
#include <QThread>

class ActiveMQModulePlugin : public Interface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "ActiveMQModulePlugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(Interface)

public:
    explicit ActiveMQModulePlugin();
    ~ActiveMQModulePlugin();
    QObject* createObject(const QString& aPluginName);

private:
    void setPluginName(const QString& aPluginName);

public slots:
    QVariantMap callFunc(QVariantMap& aMap);

    QVariantMap startConsumer(const QString& aPluginName, const QString& aClientId,
                              const QString &aSubName);
    QVariantMap endConsumer(const QString& aPluginName);
    QVariantMap sendMsgToServer(const QString& aPluginName, const QString& aMessage);
    void reveivedConMessage(const QString& aCurrentMQName, const QString& aMessage);
    void reveivedProMessage(const QString& aCurrentMQName, const QString& aMessage);

private:
    //按插件名称保存所有的插件对象
    QMap<QString, QPluginLoader*>  m_pluginLoaderMap;
    QMap<QString, QObject*>  m_mqObjectMap;
    QVariantMap m_outData;
    Interface* m_currentMQPlugin;
    QPluginLoader* m_currentPluginLoad;
//    QThread m_QThread;

};

#endif // ACTIVEMQMODULEPLUGIN_H
