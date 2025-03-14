// QtFolderScanner.cpp
#include "QtFolderScanner.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QStorageInfo>
#include <QRegularExpression>
#include <qdiriterator.h>

QtFolderScanner::QtFolderScanner(QObject *parent) : QObject(parent) {}

void QtFolderScanner::scanSystem() {
    if (m_isScanning) return;

    m_isScanning = true;
    emit scanningChanged();
    qtInstallations.clear();

    qDebug() << "Начинаю сканирование системы...";

    scanRegistry();
    scanDefaultPaths();
    scanDrives();

    qDebug() << "Найденные версии Qt:" << dirMap.keys();

    m_isScanning = false;
    emit scanningChanged();
}

void QtFolderScanner::scanRegistry() {
    QSettings settings(R"(HKEY_CURRENT_USER\Software\QtProject\QtCreator\Profiles)",
                       QSettings::NativeFormat);
    for (const QString &group : settings.childGroups()) {
        settings.beginGroup(group);
        QString path = settings.value("Path").toString().trimmed();
        if (!path.isEmpty()) {
            QDir dir(path);
            if (dir.cd("bin") && isValidQtDirectory(dir.absolutePath())) {
                QString version = dir.dirName().replace("bin", "").trimmed();
                if (!version.isEmpty() && !qtInstallations.contains(version)) {
                    qtInstallations.insert(version, dir.absolutePath());
                    qDebug() << "Найдено в реестре:" << version << "=>" << dir.absolutePath();
                }
            }
        }
        settings.endGroup();
    }
}

void QtFolderScanner::scanDefaultPaths() {
    QStringList defaultPaths = {
        "C:/Qt",
        "D:/Qt",
        "C:/Program Files/Qt",
        "C:/Program Files (x86)/Qt",
        QDir::homePath() + "/AppData/Local/Qt"
    };

    for (const QString &basePath : defaultPaths) {
        QDir dir(basePath);
        if (dir.exists()) {
            scanDirectory(dir.absolutePath());
        }
    }
}

void QtFolderScanner::scanDrives() {
    foreach (const QFileInfo& drive, QDir::drives()) {
        QString drivePath = drive.absoluteFilePath();
        QDir driveDir(drivePath);
        if (!driveDir.exists()) {
            continue;
        }

        QStringList folders = driveDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const QString& folder : folders) {
            if (folder.contains("Qt", Qt::CaseInsensitive)) {
                QString fullPath = driveDir.absoluteFilePath(folder);
                qDebug() << "Found Qt folder in drive:" << fullPath;
                findQtVersionsAllDrives(fullPath);
            }
        }
    }
}

void QtFolderScanner::scanDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) return;

    // Проверяем текущий путь
    if (isValidQtDirectory(path)) {
        QString version = QFileInfo(path).fileName();
        if (!qtInstallations.contains(version)) {
            qtInstallations.insert(version, path);
            qDebug() << "Найдено в директории:" << version << "=>" << path;
            return;
        }
    }

    // Рекурсивный поиск
    QRegularExpression versionPattern(R"(\d+\.\d+\.\d+)");
    for (const QString &entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (versionPattern.match(entry).hasMatch()) {
            QString fullPath = dir.absoluteFilePath(entry);
            scanDirectory(fullPath);
        }
    }
}

bool QtFolderScanner::isValidQtDirectory(const QString &path) const {
    QDir binDir(path);
    if (!binDir.cd("bin")) {
        return false;
    }

    return QFile::exists(binDir.filePath("windeployqt.exe")) &&
           QFile::exists(binDir.filePath("Qt5Core.dll"));
}

QStringList QtFolderScanner::getQtVersions() const {
    return dirMap.keys();
}

QMap<QString, QString> QtFolderScanner::getInstallations() const
{
    return filesMap;
}

QString QtFolderScanner::findQmlDirectory(const QString &projectPath) const {
    QDirIterator it(projectPath, QStringList() << "*.qml",
                    QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        return QFileInfo(it.filePath()).absolutePath();
    }
    return QString();
}

const QMap<QString,QVector<QString>> &QtFolderScanner::getDirMap() const
{
    return dirMap;
}

QStringList QtFolderScanner::getCompilers(const QString &version) const
{
    // Список для хранения последних частей
    QList<QString> result;

    // Проходим по каждому элементу списка
    for (const QString& item : dirMap.value(version).toList()) {
        // Разделяем строку на части по символу '/'
        QStringList parts = item.split('/');

        // Берем последний элемент после разделения
        if (!parts.isEmpty()) {
            result.append(parts.last());
        }
    }

    // Выводим результат
    qDebug() << "Result:" << result;
    return result;
}

QString QtFolderScanner::getWinDeployQtPath(const QString &version, const QString &compilerPath) const
{
    QString path;
    for (const QString& item : dirMap.value(version).toList()) {
        // Разделяем строку на части по символу '/'
        // QStringList parts = item.split('/');

        // Берем последний элемент после разделения
        if (item.endsWith(compilerPath)) {
            path = item;
        }
    }

    QDir compilerDir(path);
    if (compilerDir.cd("bin")) {
        QString path = compilerDir.filePath("windeployqt.exe");
        qDebug() << path;
        return QFile::exists(path) ? path : QString();
    }
    return QString();
}

void QtFolderScanner::findQtVersionsAllDrives(const QString &qtFolderPath)
{
    QDir qtDir(qtFolderPath);
    if (!qtDir.exists()) {
        return;
    }

    // Регулярное выражение для поиска версий (цифры с точками, например, 5.15.2)
    QRegularExpression versionRegex(R"(\d+(\.\d+)+)");

    // Перебор всех папок внутри найденной папки Qt
    for (const QString& folderName : qtDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (versionRegex.match(folderName).hasMatch()) {
            QString versionFolderPath = qtDir.filePath(folderName);
            // qDebug() << "  Найдена версия Qt:" << folderName;
            findCompilerDirs(versionFolderPath, folderName);
        }
    }
}

void QtFolderScanner::findCompilerFiles(const QString &versionFolderPath, const QString &version)
{
    QDir versionDir(versionFolderPath);
    if (!versionDir.exists()) {
        return;
    }

    // Перебор всех папок компиляторов внутри папки версии Qt
    for (const QString& compilerFolder : versionDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString compilerFolderPath = versionDir.filePath(compilerFolder);
        QDir compilerDir(compilerFolderPath);

        // Поиск файлов, начинающихся с "Qt <версия>"
        for (const QString& fileName : compilerDir.entryList(QDir::Files)) {
            if (fileName.startsWith("Qt " + version)) {
                QString fullPath = compilerDir.filePath(fileName);
                // qDebug() << "    Найден файл:" << fileName << "по пути:" << fullPath;

                // Сохранение файла и пути в QMap
                filesMap.insert(fileName, fullPath);
            }
        }
    }
}

void QtFolderScanner::findCompilerDirs(const QString &versionFolderPath, const QString &version)
{
    QDir versionDir(versionFolderPath);
    QStringList compilerFolders = versionDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& folder : compilerFolders) {
        if (folder != "Src") {
            QString folderPath = versionDir.absoluteFilePath(folder);
            // qDebug() << "Found compiler or additional folder:" << folderPath;
            dirMap[version].append(folderPath);
        }
    }
}
