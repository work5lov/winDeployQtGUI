// QtFolderScanner.h
#ifndef QTFOLDERSCANNER_H
#define QTFOLDERSCANNER_H

#include <QObject>
#include <QMap>
#include <QStringList>

class QtFolderScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY scanningChanged)

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

    bool isScanning() const { return m_isScanning; }
    void scanDrives();


signals:
    void scanningChanged();

private:
    void findQtVersionsAllDrives(const QString &qtFolderPath);
    void findCompilerFiles(const QString& versionFolderPath, const QString& version);
    void findCompilerDirs(const QString &versionFolderPath, const QString &version);

    bool m_isScanning = false;
    QMap<QString, QString> qtInstallations; // Версия -> путь к bin
    QMap<QString, QString> filesMap; // Карта для хранения файлов
    QMap<QString,QVector<QString>> dirMap;

    void scanRegistry();
    void scanDefaultPaths();

    void scanDirectory(const QString &path);
    bool isValidQtDirectory(const QString &path) const;
};

#endif // QTFOLDERSCANNER_H
