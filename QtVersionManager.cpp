#include "QtVersionManager.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QtVersionManager::QtVersionManager(QObject *parent)
    : QObject(parent) {}

QMap<QString, QString> QtVersionManager::qtVersions() const {
    return m_qtVersions;
}

void QtVersionManager::scanQtVersions() {
    m_qtVersions.clear();

    // Предполагаемый путь к ярлыкам Qt
    QString startMenuPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QDir startMenuDir(startMenuPath + "/Microsoft/Windows/Start Menu/Programs");

    if (startMenuDir.exists()) {
        scanForQtVersions(startMenuDir.absolutePath());
    }

    if (m_qtVersions.isEmpty()) {
        // Получаем список всех локальных дисков
        QList<QFileInfo> drives = QDir::drives();
        for (const QFileInfo &drive : drives) {
            if (drive.isDir()) {
                scanForQtVersions(drive.absoluteFilePath());
            }
        }
    }

    emit qtVersionsChanged();
}

void QtVersionManager::scanForQtVersions(const QString &directory) {
    QDir dir(directory);
    auto entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &entry : entries) {
        if (entry.isDir()) {
            scanForQtVersions(entry.absoluteFilePath());
        } else if (entry.fileName().contains("Qt") && entry.fileName().contains(".lnk")) {
            QString version = entry.fileName().remove(".lnk").simplified();
            QString path = entry.absoluteFilePath();
            m_qtVersions.insert(version, path);
        }
    }
}

void QtVersionManager::saveVersions(const QString &filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    for (auto it = m_qtVersions.begin(); it != m_qtVersions.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }
}

void QtVersionManager::loadVersions(const QString &filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    m_qtVersions.clear();
    for (const QString &key : settings.allKeys()) {
        m_qtVersions.insert(key, settings.value(key).toString());
    }
    emit qtVersionsChanged();
}

QString QtVersionManager::recommendQtVersion(const QString &projectPath) {
    QDir projectDir(projectPath);
    while (projectDir.cdUp()) {
        QString folderName = projectDir.dirName();
        if (folderName.contains("Qt")) {
            return folderName;
        }
    }
    return {};
}
