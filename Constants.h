// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {

// Имена утилит Qt
inline const QString WINDEPLOYQT_EXE = "windeployqt.exe";
inline const QString QT5CORE_DLL = "Qt5Core.dll";
inline const QString QT6CORE_DLL = "Qt6Core.dll";
inline const QString QMAKE_EXE = "qmake.exe";
inline const QString QTENV2_BAT = "qtenv2.bat";

// Префиксы команд
inline const QString SET_PATH_PREFIX = "set PATH=";
inline const QString SET_PREFIX = "set ";
inline const QString FILE_PREFIX = "file:///";

// Ключи QSettings
inline const QString EXCLUDED_DRIVES_KEY = "excludedDrives";

// Папки Qt
inline const QString BIN_DIR = "bin";

// Расширения
inline const QString EXE_EXTENSION = ".exe";

// Шаблоны для регулярных выражений
inline const QString VERSION_REGEX_PATTERN = R"(\d+(\.\d+)+)";
inline const QString VERSION_DIR_PATTERN = R"(\d+\.\d+\.\d+)";

// Название организации и приложения для QSettings
inline const QString ORG_NAME = "OpenSoft";
inline const QString APP_NAME = "windeployqtGUI";

} // namespace Constants

#endif // CONSTANTS_H
