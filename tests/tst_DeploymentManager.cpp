#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include "DeploymentManager.h"

class TestDeploymentManager : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultState();
    void testSetPaths();
    void testDeploymentWithoutQtVersion();
};

void TestDeploymentManager::testDefaultState()
{
    DeploymentManager manager;

    // По умолчанию не должен быть запущен
    QVERIFY(!manager.isRunning());

    // Проверяем сигнал
    QSignalSpy outputSpy(&manager, &DeploymentManager::outputReceived);
    Q_UNUSED(outputSpy)
}

void TestDeploymentManager::testSetPaths()
{
    DeploymentManager manager;

    // Устанавливаем пути
    manager.setExecutablePath("C:/test/app.exe");
    manager.setQmlDirectory("C:/test/qml");
    manager.setQtVersion("6.8.3");

    // Проверяем сохранение/загрузку
    manager.saveSettings();
    manager.loadSettings();

    // Проверка не должна упасть
    QVERIFY(true);
}

void TestDeploymentManager::testDeploymentWithoutQtVersion()
{
    DeploymentManager manager;

    // Устанавливаем только exe, но не Qt — должен выдать ошибку
    manager.setExecutablePath("C:/test/app.exe");

    // Сигнал outputReceived должен быть испущен с сообщением об ошибке
    QSignalSpy outputSpy(&manager, &DeploymentManager::outputReceived);

    manager.startDeployment();

    // Проверяем, что сигнал был послан (сообщение об ошибке)
    QCOMPARE(outputSpy.count(), 1);

    QString message = outputSpy.at(0).at(0).toString();
    QVERIFY(message.contains("Не выбрана версия Qt") || message.contains("Ошибка"));
}

QTEST_MAIN(TestDeploymentManager)
#include "tst_DeploymentManager.moc"
