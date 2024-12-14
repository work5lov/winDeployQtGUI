#pragma once

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QVariant> // Для работы с QVariant

// Q_DECLARE_METATYPE(QMap<QString, QString>)

class QtVersionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QMap<QString, QString> qtVersions READ qtVersions NOTIFY qtVersionsChanged)

public:
    explicit QtVersionManager(QObject *parent = nullptr);

    QMap<QString, QString> qtVersions() const;
    Q_INVOKABLE void scanQtVersions();
    Q_INVOKABLE void saveVersions(const QString &filePath);
    Q_INVOKABLE void loadVersions(const QString &filePath);
    Q_INVOKABLE QString recommendQtVersion(const QString &projectPath);

signals:
    void qtVersionsChanged();

private:
    QMap<QString, QString> m_qtVersions;
    void scanForQtVersions(const QString &directory);
};
