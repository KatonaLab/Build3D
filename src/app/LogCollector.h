#ifndef _app_LogCollector_h_
#define _app_LogCollector_h_

#include <QtCore>
#include <QQmlEngine>
#include <QJSEngine>
#include <sstream>

class LogCollector : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString unfilteredLog READ unfilteredLog NOTIFY logChanged)
    friend QObject* singletonLogCollectorProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
    static LogCollector& instance();
    QString unfilteredLog();
    void debugMsg(const QString& msg);
    void infoMsg(const QString& msg);
    void warningMsg(const QString& msg);
    void criticalMsg(const QString& msg);
    void fatalMsg(const QString& msg);
    virtual ~LogCollector();
Q_SIGNALS:
    void logChanged();
private:
    explicit LogCollector(QObject* parent = nullptr);
    std::stringstream m_log;
    static LogCollector* m_instance;
};

QObject* singletonLogCollectorProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif
