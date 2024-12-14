#include "QtFolderScanner.h"
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QRegularExpression>

// Конструктор
QtFolderScanner::QtFolderScanner(const QString& directory)
    : rootDirectory(directory) {}

// Возврат карты с файлами
const QMap<QString, QString>& QtFolderScanner::getFilesMap() const {
    return filesMap;
}

const QMap<QString,QVector<QString>> &QtFolderScanner::getDirMap() const
{
    return dirMap;
}

void QtFolderScanner::scanAllDrives()
{
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

// Реализация метода scan
void QtFolderScanner::scan() {
    QDir dir(rootDirectory);
    if (!dir.exists()) {
        qWarning() << "Директория не существует:" << rootDirectory;
        return;
    }

    qDebug() << "Сканирование папки:" << rootDirectory;

    QDirIterator it(rootDirectory, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        // Проверка на наличие слова "Qt" в имени папки (без учета регистра)
        if (it.fileName().contains("Qt", Qt::CaseInsensitive)) {
            // qDebug() << "Найдена папка с именем, содержащим 'Qt':" << it.filePath();
            findQtVersions(it.filePath());
        }
    }
}

// Реализация метода findQtVersions
void QtFolderScanner::findQtVersions(const QString& qtFolderPath) {
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
            findCompilerFiles(versionFolderPath, folderName);
        }
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

// Реализация метода findCompilerFiles
void QtFolderScanner::findCompilerFiles(const QString& versionFolderPath, const QString& version) {
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
