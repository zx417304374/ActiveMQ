#include "ActiveMQModuleWidget.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <activemq/library/ActiveMQCPP.h>
//#ifdef _DEBUG
//#include "vld.h"
//#endif

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

void outputMessage(QtMsgType type,const QMessageLogContext &context , const QString &msg)
{
    Q_UNUSED(context);
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
        abort();
    }
    QString message = QString("[%1]%2 %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))
                                          .arg(text)
                                          .arg(msg);

    QDir dir(QDir::currentPath());
    if(!dir.exists("log"))
    {
        dir.mkdir("log");
    }
    dir.cd("log");

    QString logfile;
    logfile = QDateTime::currentDateTime().toString("yyyyMMddhh");
    logfile = dir.filePath(QString("%1.log").arg(logfile));

    QFile file(logfile);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << endl;
    file.flush();
    file.close();

    mutex.unlock();
}
int main(int argc, char *argv[])
{
//    qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);
//    activemq::library::ActiveMQCPP::initializeLibrary();
//    QSharedMemory shareMem(QString("ActiveMQModuleExe"));
//    volatile short i = 2;
//     while (i--)
//     {
//         if (shareMem.attach(QSharedMemory::ReadOnly))
//         {
//             shareMem.detach();
//         }
//     }

//    if (shareMem.create(1))
//    {
        ActiveMQModuleWidget w;
        w.show();
//        w.hide();
        a.exec();
//        if (shareMem.isAttached())
//             shareMem.detach();
//    }
//    else
//    {
//        QMessageBox::warning(0, "警告", "该服务已存在!");
//    }
//    activemq::library::ActiveMQCPP::shutdownLibrary();
    return 0;
}
