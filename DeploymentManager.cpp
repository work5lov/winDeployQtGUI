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
    // Обновляем путь при смене версии
    if (!compilerPath.isEmpty()) {
        QtFolderScanner scanner;
        QString winDeployPath = scanner.getWinDeployQtPath(version, compilerPath);
        if (!winDeployPath.isEmpty()) {
            qtBinPath = winDeployPath;
        }
    }
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
    args << "--no-compiler-runtime"
         << "--no-angle"
         << "--no-opengl-sw";

    process.setWorkingDirectory(QFileInfo(executablePath).absolutePath());
    process.setProgram(qtBinPath + "/windeployqt.exe");
    process.setArguments(args);

    emit outputReceived("Запуск: " + process.program() + " " + args.join(' '));
    process.start();
    emit isRunningChanged();
}

void DeploymentManager::setCompilerPath(const QString &path)
{
    compilerPath = path;
    qDebug() << compilerPath;
    // Обновляем путь к windeployqt при смене компилятора
    if (!compilerPath.isEmpty() && !qtVersion.isEmpty()) {
        QtFolderScanner scanner;
        QString winDeployPath = scanner.getWinDeployQtPath(qtVersion, compilerPath);
        if (!winDeployPath.isEmpty()) {
            qtBinPath = winDeployPath;
        }
    }
}
