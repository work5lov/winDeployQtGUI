// DeploymentManager.h
#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <QObject>
#include <QProcess>
#include "QtFolderScanner.h"

class DeploymentManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)

public:
    explicit DeploymentManager(QObject *parent = nullptr);

    // Основные методы
    Q_INVOKABLE void setExecutablePath(const QString &path);
    Q_INVOKABLE void setQmlDirectory(const QString &path);
    Q_INVOKABLE void setQtVersion(const QString &version);
    Q_INVOKABLE void startDeployment();
    Q_INVOKABLE void setCompilerPath(const QString &path);
    Q_INVOKABLE void setEnvironmentCommands(const QStringList &path);

    bool isRunning() const { return process.state() != QProcess::NotRunning; }

signals:
    void outputReceived(const QString &message);
    void deploymentFinished(bool success);
    void isRunningChanged();

private:
    QProcess process;
    QString executablePath;
    QStringList environmentCommands;
    QString qmlDir;
    QString qtBinPath;

    QString qtVersion;
    QString compilerPath;
};

#endif // DEPLOYMENTMANAGER_H
