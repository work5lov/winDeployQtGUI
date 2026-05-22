// QtFolderScanner.h
#ifndef QTFOLDERSCANNER_H
#define QTFOLDERSCANNER_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QDebug>
#include <QVariant>
#include <QSettings>
#include <QAbstractItemModel>
#include "Constants.h"

class QtFolderScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantMap excludedDrivesMap READ excludedDrivesMap WRITE setExcludedDrivesMap NOTIFY excludedDrivesMapChanged)

public:
    explicit QtFolderScanner(QObject *parent = nullptr);

    Q_INVOKABLE void scanSystem();
    Q_INVOKABLE QStringList getQtVersions() const;
    Q_INVOKABLE QMap<QString, QString> getInstallations() const;
    Q_INVOKABLE QString findQmlDirectory(const QString &projectPath) const;
    Q_INVOKABLE const QMap<QString,QVector<QString>>& getCompilerPathsMap() const;
    Q_INVOKABLE QStringList getCompilers(const QString &version) const;
    Q_INVOKABLE QString getWinDeployQtPath(const QString &version, const QString &compilerPath) const;
    Q_INVOKABLE QStringList getDrivesList();
    Q_INVOKABLE QStringList getEnvironmentCommands(const QString &version, const QString &compilerPath);

    void scanDrives();
    bool isScanning() const { return m_isScanning; }
    QVariantMap excludedDrivesMap() const { return m_excludedDrivesMap; }
    void setExcludedDrivesMap(const QVariantMap &map){
        if (m_excludedDrivesMap != map) {
            m_excludedDrivesMap = map;
            emit excludedDrivesMapChanged();
        }
        scanSystem();
        saveSettings();
    }

signals:
    void scanningChanged();
    void excludedDrivesMapChanged();

private:

    struct QtConfig {
        QString version;
        QString compilerType;
        QString qmakePath;
        QString compilerPath;
        QStringList environmentCommands;
    };

    QList<QtConfig> config;

    void findQtVersionsAllDrives(const QString &qtFolderPath);
    void findCompilerDirs(const QString &versionFolderPath, const QString &version);
    void loadSettings();
    void saveSettings() const;

    bool m_isScanning = false;
    QMap<QString, QString> qtInstallations;
    QMap<QString, QString> filesMap;
    QMap<QString,QVector<QString>> dirMap;

    QVariantMap m_excludedDrivesMap;
};

#endif // QTFOLDERSCANNER_H
