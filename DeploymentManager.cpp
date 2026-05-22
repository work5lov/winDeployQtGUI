// DeploymentManager.cpp
#include "DeploymentManager.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "Constants.h"

DeploymentManager::DeploymentManager(QObject *parent) : QObject(parent)
{
    connect(&process, &QProcess::readyReadStandardOutput, this, [this](){
        emit outputReceived(QString::fromLocal8Bit(process.readAllStandardOutput()));
    });

    connect(&process, &QProcess::readyReadStandardError, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(process.readAllStandardError()));
    });

    connect(&process, &QProcess::finished, this, [this](int exitCode) {
        timeoutTimer.stop();
        emit deploymentFinished(exitCode == 0);
        emit isRunningChanged();
    });

    // Таймаут
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, this, [this]() {
        emit outputReceived("Ошибка: Превышен таймаут выполнения деплоя!");
        process.kill();
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

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    for (const QString &command : environmentCommands)
    {
        if (command.startsWith(Constants::SET_PATH_PREFIX, Qt::CaseInsensitive))
        {
            QString pathValue = command.mid(Constants::SET_PATH_PREFIX.length())
                                .replace("%PATH%", env.value("PATH"), Qt::CaseInsensitive);
            env.insert("PATH", pathValue);
        }
        else if (command.startsWith(Constants::SET_PREFIX, Qt::CaseInsensitive))
        {
            QStringList parts = command.mid(Constants::SET_PREFIX.length())
                                .split('=', Qt::SkipEmptyParts);
            if (parts.size() >= 2)
            {
                QString key = parts.first().trimmed();
                QString value = parts.mid(1).join("=").trimmed();
                env.insert(key, value);
            }
        }
    }

    process.setProcessEnvironment(env);

    QStringList args;
    args << executablePath;

    if (!qmlDir.isEmpty() && QFile::exists(qmlDir))
    {
        args << "--qmldir" << qmlDir;
    }

    process.setWorkingDirectory(QFileInfo(executablePath).absolutePath());
    process.setProgram(qtBinPath);
    process.setArguments(args);

    disconnect(&process, &QProcess::finished, nullptr, nullptr);
    connect(&process, &QProcess::finished, this, [this](int exitCode) {
        timeoutTimer.stop();
        QString message;
        if (process.exitStatus() == QProcess::NormalExit)
        {
            message = "Деплой успешно завершен. Код выхода: " + QString::number(exitCode);
        }
        else
        {
            message = "Деплой завершился с ошибкой. Код выхода: " + QString::number(exitCode)
                      + " (" + process.errorString() + ")";
        }
        emit outputReceived(message);
        emit isRunningChanged();
    });

    emit outputReceived("Запуск: " + process.program() + " " + args.join(' '));

    process.start();
    if (!process.waitForStarted(5000))
    {
        emit outputReceived("Ошибка: Не удалось запустить процесс! " + process.errorString());
        emit isRunningChanged();
        return;
    }

    timeoutTimer.start(m_timeoutMs);
    emit isRunningChanged();
}

void DeploymentManager::setWinDeployQtPath(const QString &path)
{
    qtBinPath = path;
}

void DeploymentManager::setEnvironmentCommands(const QStringList &commands)
{
    environmentCommands = commands;
}

void DeploymentManager::saveSettings()
{
    QSettings settings;
    settings.setValue("saved/exePath", executablePath);
    settings.setValue("saved/qmlDir", qmlDir);
    settings.setValue("saved/qtVersion", qtVersion);
    settings.setValue("saved/theme", m_theme);
}

void DeploymentManager::loadSettings()
{
    QSettings settings;
    m_savedExePath = settings.value("saved/exePath", "").toString();
    m_savedQmlDir = settings.value("saved/qmlDir", "").toString();
    m_savedQtVersion = settings.value("saved/qtVersion", "").toString();
    m_theme = settings.value("saved/theme", "Light").toString();
    emit themeChanged();
}

void DeploymentManager::resetAllSettings()
{
    QSettings settings;
    settings.remove("saved");

    m_savedExePath.clear();
    m_savedQmlDir.clear();
    m_savedQtVersion.clear();
    m_theme = "Light";
    emit themeChanged();
}

void DeploymentManager::exportLog(const QString &filePath, const QString &logContent)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << "=== winDeployQtGUI Log ===" << "\n";
        stream << "Дата: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        stream << "==========================\n\n";
        stream << logContent;
        file.close();
        emit outputReceived("Лог сохранён: " + filePath);
    }
    else
    {
        emit outputReceived("Ошибка: Не удалось сохранить лог в " + filePath);
    }
}

void DeploymentManager::setTheme(const QString &theme)
{
    if (m_theme != theme)
    {
        m_theme = theme;
        saveSettings();
        emit themeChanged();
    }
}
