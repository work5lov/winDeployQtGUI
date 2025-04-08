// QtFolderScanner.h
#ifndef QTFOLDERSCANNER_H
#define QTFOLDERSCANNER_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <qdebug.h>
#include <qvariant.h>
#include <QSettings>
#include <QAbstractItemModel>

class QtFolderScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantMap excludedDrivesMap READ excludedDrivesMap WRITE setExcludedDrivesMap NOTIFY excludedDrivesMapChanged)

public:
    explicit QtFolderScanner(QObject *parent = nullptr);

    // Основные методы
    Q_INVOKABLE void scanSystem();
    Q_INVOKABLE QStringList getQtVersions() const;
    Q_INVOKABLE QMap<QString, QString> getInstallations() const; // Новый метод
    Q_INVOKABLE QString findQmlDirectory(const QString &projectPath) const;
    Q_INVOKABLE const QMap<QString,QVector<QString>>& getDirMap() const; // Возврат сохранённых файлов
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
        saveSettings();  // Автоматическое сохранение
    }

signals:
    void scanningChanged();
    void excludedDrivesMapChanged();

private:

    struct QtConfig {
        QString version;
        QString compilerType; // mingw, llvm-mingw, msvc, android, wasm и т.д.
        QString qmakePath;
        QString compilerPath; // Для MinGW/LLVM
        QStringList environmentCommands; // Новые команды из qtenv2.bat
    };

    QList<QtConfig> config;

    void findQtVersionsAllDrives(const QString &qtFolderPath);
    void findCompilerDirs(const QString &versionFolderPath, const QString &version);
    void loadSettings();
    void saveSettings() const;

    bool m_isScanning = false;
    QMap<QString, QString> qtInstallations; // Версия -> путь к bin
    QMap<QString, QString> filesMap; // Карта для хранения файлов
    QMap<QString,QVector<QString>> dirMap;

    QVariantMap m_excludedDrivesMap;

    void scanDirectory(const QString &path);
    bool isValidQtDirectory(const QString &path) const;
};

#endif // QTFOLDERSCANNER_H
