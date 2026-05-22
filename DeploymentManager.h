// DeploymentManager.h
#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QSettings>
#include "QtFolderScanner.h"

class DeploymentManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    explicit DeploymentManager(QObject *parent = nullptr);

    Q_INVOKABLE void setExecutablePath(const QString &path);
    Q_INVOKABLE void setQmlDirectory(const QString &path);
    Q_INVOKABLE void setQtVersion(const QString &version);
    Q_INVOKABLE void startDeployment();
    Q_INVOKABLE void setWinDeployQtPath(const QString &path);
    Q_INVOKABLE void setEnvironmentCommands(const QStringList &commands);

    // Сохранение/загрузка настроек
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void resetAllSettings();

    // Экспорт лога в файл
    Q_INVOKABLE void exportLog(const QString &filePath, const QString &logContent);

    // Тема
    QString theme() const { return m_theme; }
    void setTheme(const QString &theme);

    bool isRunning() const { return process.state() != QProcess::NotRunning; }
    void setTimeoutMs(int ms) { m_timeoutMs = ms; }

    // Геттеры для сохранённых путей
    Q_INVOKABLE QString savedExePath() const { return m_savedExePath; }
    Q_INVOKABLE QString savedQmlDir() const { return m_savedQmlDir; }
    Q_INVOKABLE QString savedQtVersion() const { return m_savedQtVersion; }

signals:
    void outputReceived(const QString &message);
    void deploymentFinished(bool success);
    void isRunningChanged();
    void themeChanged();

private:
    QProcess process;
    QTimer timeoutTimer;
    QString executablePath;
    QStringList environmentCommands;
    QString qmlDir;
    QString qtBinPath;
    QString qtVersion;
    QString m_theme = "Light";
    QString m_savedExePath;
    QString m_savedQmlDir;
    QString m_savedQtVersion;

    int m_timeoutMs = 120000; // 120 секунд
};

#endif // DEPLOYMENTMANAGER_H
