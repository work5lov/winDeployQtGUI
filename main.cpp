#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QSettings>
#include "DeploymentManager.h"
#include "QtFolderScanner.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    // Установите название организации и приложения
    QCoreApplication::setOrganizationName("OpenSoft");
    QCoreApplication::setApplicationName("windeployqtGUI");

    // Инициализация QSettings
    QSettings settings;

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    qmlRegisterType<DeploymentManager>("Deployment", 1, 0, "DeploymentManager");
    qmlRegisterType<QtFolderScanner>("Scanner", 1, 0, "QtFolderScanner");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
