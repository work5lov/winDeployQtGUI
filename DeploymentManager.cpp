// DeploymentManager.cpp
#include "DeploymentManager.h"
#include <QDebug>
#include <QDir>

DeploymentManager::DeploymentManager(QObject *parent) : QObject(parent) {
    connect(&process, &QProcess::readyReadStandardOutput, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(process.readAllStandardOutput()));
    });

    connect(&process, &QProcess::readyReadStandardError, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(process.readAllStandardError()));
    });

    connect(&process, QOverload<int>::of(&QProcess::finished), this, [this](int exitCode) {
        emit deploymentFinished(exitCode == 0);
        emit isRunningChanged();
    });
}

void DeploymentManager::setExecutablePath(const QString &path) {
    executablePath = QDir::toNativeSeparators(path);
}

void DeploymentManager::setQmlDirectory(const QString &path) {
    qmlDir = QDir::toNativeSeparators(path);
}

void DeploymentManager::setQtVersion(const QString &version) {
    qtVersion = version;
}

void DeploymentManager::startDeployment() {
    if (process.state() != QProcess::NotRunning) {
        emit outputReceived("Ошибка: Деплой уже запущен!");
        return;
    }

    if (executablePath.isEmpty() || !QFile::exists(executablePath)) {
        emit outputReceived("Ошибка: Неверный путь к исполняемому файлу!");
        return;
    }

    if (qtBinPath.isEmpty()) {
        emit outputReceived("Ошибка: Не выбрана версия Qt!");
        return;
    }

    QStringList args;
    args << executablePath;

    if (!qmlDir.isEmpty() && QFile::exists(qmlDir)) {
        args << "--qmldir" << qmlDir;
    }

    // Добавляем другие необходимые аргументы
    // args << "--no-compiler-runtime"
         // << "--no-angle"
         // << "--no-opengl-sw";

    process.setWorkingDirectory(QFileInfo(executablePath).absolutePath());
    process.setProgram(qtBinPath);
    process.setArguments(args);

    // Подключение сигнала finished
    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        QString message;
        if (exitStatus == QProcess::NormalExit) {
            message = "Деплой успешно завершен. Код выхода: " + QString::number(exitCode);
        } else {
            message = "Деплой завершился с ошибкой. Код выхода: " + QString::number(exitCode);
        }
        emit outputReceived(message);
        emit isRunningChanged();
    });

    emit outputReceived("Запуск: " + process.program() + " " + args.join(' '));
    process.start();
    emit isRunningChanged();
}

void DeploymentManager::setCompilerPath(const QString &path)
{
    qtBinPath = path;
}
