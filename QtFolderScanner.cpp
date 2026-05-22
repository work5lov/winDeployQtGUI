// QtFolderScanner.cpp
#include "QtFolderScanner.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QStorageInfo>
#include <QRegularExpression>
#include <QDirIterator>

QtFolderScanner::QtFolderScanner(QObject *parent) : QObject(parent) {loadSettings();}

void QtFolderScanner::scanSystem()
{
    if (m_isScanning) return;

    m_isScanning = true;
    emit scanningChanged();
    qtInstallations.clear();
    filesMap.clear();
    dirMap.clear();

    scanDrives();

    m_isScanning = false;
    emit scanningChanged();
}

void QtFolderScanner::scanDrives()
{
    foreach (const QFileInfo& drive, QDir::drives())
    {
        QString drivePath = drive.absoluteFilePath();
        drivePath = drivePath.replace("\\", "/");

        // Проверяем исключения
        if (m_excludedDrivesMap.value(drivePath, true).toBool())
        {
            continue;
        }

        // Проверяем доступность диска через QStorageInfo
        QStorageInfo storageInfo(drivePath);
        if (!storageInfo.isReady())
        {
            qWarning() << "Диск не готов (нет носителя):" << drivePath;
            continue;
        }

        QDir driveDir(drivePath);
        if (!driveDir.exists())
        {
            continue;
        }

        // Защищённый блок для чтения диска
        QStringList folders;
        try {
            folders = driveDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        } catch (...) {
            qWarning() << "Ошибка доступа к диску:" << drivePath;
            continue;
        }

        for (const QString& folder : folders)
        {
            if (folder.contains("Qt", Qt::CaseInsensitive))
            {
                QString fullPath = driveDir.absoluteFilePath(folder);
                findQtVersionsAllDrives(fullPath);
            }
        }
    }
}

QStringList QtFolderScanner::getQtVersions() const
{
    return dirMap.keys();
}

QMap<QString, QString> QtFolderScanner::getInstallations() const
{
    return filesMap;
}

QString QtFolderScanner::findQmlDirectory(const QString &projectPath) const
{
    QDirIterator it(projectPath, QStringList() << "*.qml",
                    QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        return QFileInfo(it.filePath()).absolutePath();
    }
    return QString();
}

const QMap<QString,QVector<QString>> &QtFolderScanner::getCompilerPathsMap() const
{
    return dirMap;
}

QStringList QtFolderScanner::getCompilers(const QString &version) const
{
    if (!dirMap.contains(version) || dirMap.value(version).isEmpty())
    {
        qInfo() << "Компиляторы не найдены для версии:" << version;
        return {};
    }

    QStringList result;
    for (const QString& item : dirMap.value(version).toList())
    {
        QStringList parts = item.split('/');
        if (!parts.isEmpty())
        {
            result.append(parts.last());
        }
    }
    return result;
}

QString QtFolderScanner::getWinDeployQtPath(const QString &version, const QString &compilerPath) const
{
    QString path;
    for (const QString& item : dirMap.value(version).toList())
    {
        if (item.endsWith(compilerPath))
        {
            path = item;
        }
    }

    QDir compilerDir(path);
    if (compilerDir.cd(Constants::BIN_DIR))
    {
        QString fullPath = compilerDir.filePath(Constants::WINDEPLOYQT_EXE);
        return QFile::exists(fullPath) ? fullPath : QString();
    }
    return QString();
}

QStringList QtFolderScanner::getDrivesList()
{
    QStringList drives;
    for (const QFileInfo &driveInfo : QDir::drives())
    {
        QString path = driveInfo.absoluteFilePath();
        path = path.replace("\\", "/");

        // Проверяем доступность диска
        QStorageInfo storageInfo(path);
        if (!storageInfo.isReady())
        {
            continue;
        }

        drives.append(path);
    }
    return drives;
}

QStringList QtFolderScanner::getEnvironmentCommands(const QString &version, const QString &compilerPath)
{
    QString path;
    for (const QString& item : dirMap.value(version).toList())
    {
        if (item.endsWith(compilerPath)) {
            path = item;
        }
    }

    QStringList out;
    QDir compilerDir(path);
    for (QtConfig &cfg : config)
    {
        if (cfg.compilerPath == compilerDir.absolutePath() &&
            cfg.version == version)
        {
            out = cfg.environmentCommands;
            break;
        }
    }
    return out;
}

void QtFolderScanner::findQtVersionsAllDrives(const QString &qtFolderPath)
{
    QDir qtDir(qtFolderPath);
    if (!qtDir.exists())
    {
        return;
    }

    QRegularExpression versionRegex(Constants::VERSION_REGEX_PATTERN);

    for (const QString& folderName : qtDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (versionRegex.match(folderName).hasMatch())
        {
            QString versionFolderPath = qtDir.filePath(folderName);
            findCompilerDirs(versionFolderPath, folderName);
        }
    }
}

void QtFolderScanner::findCompilerDirs(const QString &versionFolderPath, const QString &version)
{
    QDir versionDir(versionFolderPath);
    QStringList compilerFolders;
    try {
        compilerFolders = versionDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    } catch (...) {
        qWarning() << "Ошибка доступа к папке:" << versionFolderPath;
        return;
    }

    for (const QString& folder : compilerFolders)
    {
        if (folder == "Src") continue;

        QString folderPath = versionDir.absoluteFilePath(folder);
        QString qtenvPath = folderPath + "/bin/" + Constants::QTENV2_BAT;

        if (!QFile::exists(qtenvPath))
        {
            qtenvPath = folderPath + "/" + Constants::QTENV2_BAT;
        }

        // Проверяем наличие windeployqt.exe — он обязателен для развёртывания
        QString deployToolPath = folderPath + "/bin/" + Constants::WINDEPLOYQT_EXE;
        if (!QFile::exists(deployToolPath))
        {
            qWarning() << "windeployqt.exe не найден в" << folderPath << "- пропускаем";
            continue;
        }

        // qmake.exe опционален в Qt 6.8+, проверяем только для информации
        QString qmakePath = folderPath + "/bin/" + Constants::QMAKE_EXE;
        if (!QFile::exists(qmakePath))
        {
            qInfo() << "qmake.exe не найден в" << folderPath << "(опционально, не требуется)";
        }

        QtConfig cfg;
        cfg.version = version;
        cfg.qmakePath = qmakePath;

        if (!qtenvPath.isEmpty())
        {
            QFile file(qtenvPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream stream(&file);
                while (!stream.atEnd())
                {
                    QString line = stream.readLine().trimmed();
                    if (!line.isEmpty() && !line.startsWith("echo") && !line.startsWith("cd"))
                        cfg.environmentCommands.append(line);
                }
                file.close();
            }
        }

        if (folder.contains("mingw", Qt::CaseInsensitive))
        {
            cfg.compilerType = "mingw";
            cfg.compilerPath = folderPath;
        }
        else if (folder.contains("llvm", Qt::CaseInsensitive)) {
            cfg.compilerType = "llvm-mingw";
            cfg.compilerPath = folderPath;
        }
        else if (folder.contains("msvc", Qt::CaseInsensitive)) {
            cfg.compilerType = "msvc";
            cfg.compilerPath = folderPath;
        }
        else if (folder.contains("android", Qt::CaseInsensitive)) {
            cfg.compilerType = "android";
            cfg.compilerPath = folderPath;
        }
        else {
            cfg.compilerType = "unknown";
        }

        dirMap[version].append(folderPath);
        config.append(cfg);
    }
}

void QtFolderScanner::loadSettings()
{
    QSettings settings;
    QVariantMap savedMap = settings.value(Constants::EXCLUDED_DRIVES_KEY).toMap();

    if (savedMap.isEmpty())
    {
        QStringList drives = getDrivesList();
        for (const QString &drive : drives)
        {
            savedMap[drive] = false;
        }
    }

    QStringList currentDrives = getDrivesList();
    for (const QString &drive : currentDrives)
    {
        if (!savedMap.contains(drive))
        {
            savedMap[drive] = false;
        }
    }

    m_excludedDrivesMap = savedMap;
}

void QtFolderScanner::saveSettings() const
{
    QSettings().setValue(Constants::EXCLUDED_DRIVES_KEY, m_excludedDrivesMap);
}
