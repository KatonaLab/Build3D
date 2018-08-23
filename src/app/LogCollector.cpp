#include "LogCollector.h"

#include <iostream>
#include <string>

using namespace std;

LogCollector* LogCollector::m_instance = nullptr;

LogCollector& LogCollector::instance()
{
    if (!m_instance) {
        m_instance = new LogCollector;
        qInstallMessageHandler(logMessageHandler);
    }
    return *m_instance;
}

QString LogCollector::unfilteredLog()
{
    return QString::fromStdString(m_log.str());
}

QString newLines(const QString& msg)
{
    QString newOne(msg);
    if (newOne.endsWith("\n")) {
        newOne.remove(newOne.size() - 1, 1);
    }
    return newOne.replace(QString("\n"), QString("<br/>"));
}

void LogCollector::debugMsg(const QString& msg)
{
    m_log << "<span style='color:lightseagreen'>debug: " << newLines(msg).toStdString() << "</span><br/>";
    Q_EMIT logChanged();
}

void LogCollector::infoMsg(const QString& msg)
{
    m_log << "<span style='color:seagreen'>info: " << newLines(msg).toStdString() << "</span><br/>";
    Q_EMIT logChanged();
}

void LogCollector::warningMsg(const QString& msg)
{
    // TODO: I was not able to fix these messages, but after all they seem to
    // mean nothing, since everything works as excepted.
    // This kind of error suppression is a bad idea, fix it.
    // TODO: at least move to these as settings on the LogCollector interface
    // so one can be alerted about log masking is used!
    if (msg == QString("Texture target does not support array layers")) {
        return;
    }
    if (msg == QString("QApplication: invalid style override passed, ignoring it."))
    {
        return;
    }

    m_log << "<span style='color:gold'>warning: " << newLines(msg).toStdString() << "</span><br/>";
    Q_EMIT logChanged();
}

void LogCollector::criticalMsg(const QString& msg)
{
    m_log << "<span style='color:crimson'>critical: " << newLines(msg).toStdString() << "</span><br/>";
    Q_EMIT logChanged();
}

void LogCollector::fatalMsg(const QString& msg)
{
    m_log << "<span style='color:crimson'>fatal: " << newLines(msg).toStdString() << "</span><br/>";
    Q_EMIT logChanged();
}

LogCollector::LogCollector(QObject* parent)
    : QObject(parent)
{
    Q_EMIT logChanged();
}

LogCollector::~LogCollector()
{
    qInstallMessageHandler(0);
}

QObject* singletonLogCollectorProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    // ensure singleton instance is alive
    LogCollector::instance();
    // NOTE: the instance is owned by the QML engine, so destruction is handled
    return LogCollector::m_instance;
}

void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            LogCollector::instance().debugMsg(msg);
            std::cout << msg.toStdString() << "\n";
            break;
        case QtInfoMsg:
            LogCollector::instance().infoMsg(msg);
            std::cout << msg.toStdString() << "\n";
            break;
        case QtWarningMsg:
            LogCollector::instance().warningMsg(msg);
            std::cerr << msg.toStdString() << "\n";
            break;
        case QtCriticalMsg:
            LogCollector::instance().criticalMsg(msg);
            std::cerr << msg.toStdString() << "\n";
            break;
        case QtFatalMsg:
            LogCollector::instance().fatalMsg(msg);
            std::cerr << msg.toStdString() << "\n";
            //abort();
    }
}
