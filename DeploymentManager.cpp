// DeploymentManager.cpp
#include "DeploymentManager.h"
#include <QDebug>
#include <QDir>

DeploymentManager::DeploymentManager(QObject *parent) : QObject(parent)
{
    connect(&process, &QProcess::readyReadStandardOutput, this, [this](){
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

void DeploymentManager::setExecutablePath(const QString &path)
{
    executablePath = QDir::toNativeSeparators(path);
}

void DeploymentManager::setQmlDirectory(const QString &path)
{
    qmlDir = QDir::toNativeSeparators(path);
}

void DeploymentManager::setQtVersion(const QString &version)
{
    qtVersion = version;
}

void DeploymentManager::startDeployment()
{
    if (process.state() != QProcess::NotRunning)
    {
        emit outputReceived("Ошибка: Деплой уже запущен!");
        return;
    }

    if (executablePath.isEmpty() || !QFile::exists(executablePath))
    {
        emit outputReceived("Ошибка: Неверный путь к исполняемому файлу!");
        return;
    }

    if (qtBinPath.isEmpty())
    {
        emit outputReceived("Ошибка: Не выбрана версия Qt!");
        return;
    }

    // Создаем среду
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // Применяем команды из qtenv2.bat
    for (const QString &command : environmentCommands)
    {
        if (command.startsWith("set PATH=", Qt::CaseInsensitive))
        {
            QString pathValue = command.mid(9).replace("%PATH%", env.value("PATH"), Qt::CaseInsensitive);
            env.insert("PATH", pathValue);
        }
        else if (command.startsWith("set ", Qt::CaseInsensitive))
        {
            QStringList parts = command.mid(4).split('=', QString::SkipEmptyParts);
            if (parts.size() >= 2)
            {
                QString key = parts.first().trimmed();
                QString value = parts.mid(1).join("=").trimmed(); // Поддержка значений с '='
                env.insert(key, value);
            }
        }
    }

    // Устанавливаем среду для процесса
    process.setProcessEnvironment(env);

    QStringList args;
    args << executablePath;

    if (!qmlDir.isEmpty() && QFile::exists(qmlDir))
    {
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
    disconnect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
               nullptr, nullptr);
    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        QString message;
        if (exitStatus == QProcess::NormalExit)
        {
            message = "Деплой успешно завершен. Код выхода: " + QString::number(exitCode);
        }
        else
        {
            message = "Деплой завершился с ошибкой. Код выхода: " + QString::number(exitCode);
        }
        emit outputReceived(message);
        emit isRunningChanged();
    });

    emit outputReceived("Запуск: " + process.program() + " " + args.join(' '));
    process.start();
    emit isRunningChanged();
    // if (!process.startDetached()) { // Используем startDetached для асинхронного выполнения
    //     emit outputReceived("Ошибка запуска процесса!");
    //     emit isRunningChanged();
    // }
}

void DeploymentManager::setCompilerPath(const QString &path)
{
    qtBinPath = path;
}

void DeploymentManager::setEnvironmentCommands(const QStringList &path)
{
    environmentCommands = path;
    qDebug() << environmentCommands;
    // environmentCommands = path.toStringList();
}
