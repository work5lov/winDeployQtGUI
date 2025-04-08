// QtFolderScanner.cpp
#include "QtFolderScanner.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QStorageInfo>
#include <QRegularExpression>
#include <qdiriterator.h>

QtFolderScanner::QtFolderScanner(QObject *parent) : QObject(parent) {loadSettings();}

void QtFolderScanner::scanSystem() {
    if (m_isScanning) return;

    m_isScanning = true;
    emit scanningChanged();
    qtInstallations.clear();
    filesMap.clear(); // Карта для хранения файлов
    dirMap.clear();

    scanDrives();

    m_isScanning = false;
    emit scanningChanged();
}

void QtFolderScanner::scanDrives()
{
    foreach (const QFileInfo& drive, QDir::drives()) {
        QString drivePath = drive.absoluteFilePath();

        // Нормализуем путь для сравнения
        drivePath = drivePath.replace("\\", "/");

        // Проверяем исключения
        if (m_excludedDrivesMap.value(drivePath, true).toBool()) {
            continue;
        }

        QDir driveDir(drivePath);
        if (!driveDir.exists()) {
            continue;
        }

        QStringList folders = driveDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const QString& folder : folders) {
            if (folder.contains("Qt", Qt::CaseInsensitive)) {
                QString fullPath = driveDir.absoluteFilePath(folder);
                // qDebug() << "Found Qt folder in drive:" << fullPath;
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
        return QFile::exists(path) ? path : QString();
    }
    return QString();
}

QStringList QtFolderScanner::getDrivesList()
{
    QStringList drives;
    for (const QFileInfo &driveInfo : QDir::drives()) {
        QString path = driveInfo.absoluteFilePath();
        // Нормализуем путь для хранения
        path = path.replace("\\", "/");
        drives.append(path);
    }
    return drives;
}

QStringList QtFolderScanner::getEnvironmentCommands(const QString &version, const QString &compilerPath)
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
    for (QtConfig &cfg : config) {
        // Нормализуем пути для корректного сравнения
        QString normalizedConfigPath = QDir::cleanPath(cfg.compilerPath);

        if (cfg.version == version &&
            normalizedConfigPath == compilerDir.absolutePath())
        {
            // qDebug() << cfg.environmentCommands;
            return cfg.environmentCommands;
        }
        else return QStringList();
    }
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

void QtFolderScanner::findCompilerDirs(const QString &versionFolderPath, const QString &version)
{
    QDir versionDir(versionFolderPath);
    QStringList compilerFolders = versionDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& folder : compilerFolders) {
        if (folder == "Src") continue;

        QString folderPath = versionDir.absoluteFilePath(folder);
        QString qtenvPath = folderPath + "/bin/qtenv2.bat"; // Проверяем в поддиректории bin

        // Если не найден в bin, ищем в корне компилятора
        if (!QFile::exists(qtenvPath)) {
            qtenvPath = folderPath + "/qtenv2.bat";
        }

        QString qmakePath = folderPath + "/bin/qmake.exe";

        // Проверяем, что это валидная Qt-сборка
        if (!QFile::exists(qmakePath)) continue;

        QtConfig cfg;

        cfg.version = version;
        cfg.qmakePath = qmakePath;

        // Сохраняем команды из qtenv2.bat
        if (!qtenvPath.isEmpty()) {
            QFile file(qtenvPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                while (!stream.atEnd()) {
                    QString line = stream.readLine().trimmed();
                    if (!line.isEmpty() && !line.startsWith("echo") && !line.startsWith("cd"))
                        cfg.environmentCommands.append(line);
                }
                file.close();
            }
        }

        // Определяем тип компилятора
        if (folder.contains("mingw", Qt::CaseInsensitive)) {
            cfg.compilerType = "mingw";
            // Ищем путь к MinGW (например, D:/Qt/Tools/mingw810_64/bin)
            // QDir toolsDir("D:/Qt/Tools"); // Укажите ваш реальный путь к Tools
            QString mingwFolder = "mingw" + folder.mid(4, 3) + "_64"; // mingw81_64 → mingw810_64
            cfg.compilerPath = folderPath;
        }
        else if (folder.contains("llvm", Qt::CaseInsensitive)) {
            cfg.compilerType = "llvm-mingw";
            cfg.compilerPath = folderPath; // Укажите реальный путь
        }
        else if (folder.contains("msvc", Qt::CaseInsensitive)) {
            cfg.compilerType = "msvc";
            cfg.compilerPath = folderPath; // Для MSVC компилятор встроен в систему
        }
        else if (folder.contains("android", Qt::CaseInsensitive)) {
            cfg.compilerType = "android";
            cfg.compilerPath = folderPath; // Для MSVC компилятор встроен в систему
        }
        else {
            cfg.compilerType = "unknown";
        }

        // Добавляем валидную конфигурацию
        if (!cfg.compilerType.isEmpty()) {
            // qDebug() << "Found Qt" << version << "with" << cfg.compilerType << "at" << folderPath;
            // dirMap[version].append(config);
        }

        dirMap[version].append(folderPath);

        // Выводим полную информацию
        // qDebug() << "\n*** Найдена конфигурация Qt ***";
        // qDebug() << "Версия Qt:" << cfg.version;
        // qDebug() << "Тип компилятора:" << cfg.compilerType;
        // qDebug() << "Путь к qmake:" << cfg.qmakePath;
        // qDebug() << "Путь к компилятору:" << cfg.compilerPath;
        // qDebug() << "Команды окружения из qtenv2.bat:";
        // for (const QString &cmd : cfg.environmentCommands) {
        //     qDebug() << "  " << cmd;
        // }
        // qDebug() << "*******************************\n";

        config.append(cfg);
    }
}

void QtFolderScanner::loadSettings()
{
    QSettings settings;
    QVariantMap savedMap = settings.value("excludedDrives").toMap();

    // Если настроек нет - инициализируем все диски как false
    if (savedMap.isEmpty()) {
        QStringList drives = getDrivesList();
        for (const QString &drive : drives) {
            savedMap[drive] = false;
        }
    }

    // Добавляем новые диски, которых не было в настройках
    QStringList currentDrives = getDrivesList();
    for (const QString &drive : currentDrives) {
        if (!savedMap.contains(drive)) {
            savedMap[drive] = false;
        }
    }

    m_excludedDrivesMap = savedMap;
}

void QtFolderScanner::saveSettings() const
{
    QSettings().setValue("excludedDrives", m_excludedDrivesMap);
}
