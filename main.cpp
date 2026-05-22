// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include "DeploymentManager.h"
#include "QtFolderScanner.h"
#include "Constants.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");

    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName(Constants::ORG_NAME);
    QCoreApplication::setApplicationName(Constants::APP_NAME);

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
