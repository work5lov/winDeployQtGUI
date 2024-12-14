#ifndef QTFOLDERSCANNER_H
#define QTFOLDERSCANNER_H

#include <QString>
#include <QMap>

class QtFolderScanner {
public:
    explicit QtFolderScanner(const QString& directory); // Конструктор
    void scan(); // Метод для сканирования папок
    const QMap<QString, QString>& getFilesMap() const; // Возврат сохранённых файлов
    const QMap<QString,QVector<QString>>& getDirMap() const; // Возврат сохранённых файлов

    void scanAllDrives(); // Метод для сканирования всех локальных дисков

private:
    QString rootDirectory; // Корневая директория
    QMap<QString, QString> filesMap; // Карта для хранения файлов
    QMap<QString,QVector<QString>> dirMap;

    void findQtVersions(const QString& qtFolderPath); // Поиск версий Qt в папке
    void findQtVersionsAllDrives(const QString& qtFolderPath); // Поиск версий Qt в папке
    void findCompilerFiles(const QString& versionFolderPath, const QString& version); // Поиск файлов внутри папок компиляторов
    void findCompilerDirs(const QString& versionFolderPath, const QString& version); // Поиск файлов внутри папок компиляторов
    // void findCompilerFolders(const QString& versionFolderPath);
};

#endif // QTFOLDERSCANNER_H
