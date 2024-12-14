#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include "QtVersionManager.h"
#include "QtFolderScanner.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");

    // Укажите путь для сканирования
    QString directory = R"(C:\Users\Vlad\AppData\Roaming\Microsoft\Windows\Start Menu\Programs)";
    QtFolderScanner scanner(directory);

    scanner.scanAllDrives();

    // Получение и вывод сохранённых данных
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
