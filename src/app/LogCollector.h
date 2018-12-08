#ifndef _app_LogCollector_h_
#define _app_LogCollector_h_

#include <QtCore>
#include <QQmlEngine>
#include <QJSEngine>
#include <QMutex>
#include <sstream>

class LogCollector : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString unfilteredLog READ unfilteredLog NOTIFY logChanged)
    friend QObject* singletonLogCollectorProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
    static LogCollector& instance();
    QString unfilteredLog();
    inline void debugMsg(const QString& msg);
    inline void infoMsg(const QString& msg);
    inline void warningMsg(const QString& msg);
    inline void criticalMsg(const QString& msg);
    inline void fatalMsg(const QString& msg);
    virtual ~LogCollector();
Q_SIGNALS:
    void logChanged();
private:
    explicit LogCollector(QObject* parent = nullptr);
    std::stringstream m_log;
    QReadWriteLock m_rwLock;
    static LogCollector* m_instance;

    static QString newLines(const QString& msg);
    inline void log(const QString& msg, const std::string& color, const std::string& prefix);
};

QObject* singletonLogCollectorProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif
