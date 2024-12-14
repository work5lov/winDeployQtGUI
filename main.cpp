#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include "QtVersionManager.h"
#include "QtFolderScanner.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");

    // QString shortcutPath = "C:/Users/Vlad/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Qt/6.8.0/LLVM MinGW 17.0.6 (64-bit)/Qt 6.8.0 (llvm-mingw 17.0.6 64-bit).lnk";

    // QProcess process;
    // // Используем "start" для открытия .lnk файла, Windows обрабатывает ярлык и запускает связанный файл
    // if (process.startDetached("cmd.exe", {"/c", "start", shortcutPath})) {
    //     qDebug() << "Console launched successfully.";
    // } else {
    //     qDebug() << "Failed to launch console.";
    // }

    // Укажите путь для сканирования
    QString directory = R"(C:\Users\Vlad\AppData\Roaming\Microsoft\Windows\Start Menu\Programs)";
    QtFolderScanner scanner(directory);

    // Запуск сканирования
    // scanner.scan();

    scanner.scanAllDrives();

    // Получение и вывод сохранённых данных
    // const QMap<QString, QString>& filesMap = scanner.getDirMap();
    const QMap<QString,QVector<QString>> filesMap = scanner.getDirMap();
    qDebug() << "\nСохранённые файлы:";
    for (auto it = filesMap.begin(); it != filesMap.end(); ++it) {
        qDebug() << "Файл:" << it.key() << "Путь:" << it.value();
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    qmlRegisterType<QtVersionManager>("com.myapp", 1, 0, "QtVersionManager");
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    // engine.load(url);

    return app.exec();
}
