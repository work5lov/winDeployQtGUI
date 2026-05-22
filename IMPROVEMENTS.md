# Предложения по улучшению проекта **winDeployQtGUI**

> **Приоритет**: критическое (ошибки, чистота кода, UI → тесты/CI → документация)
> **Целевая версия Qt**: 6.8.3

---

## — Выполнено в текущей итерации (22.05.2026) —

### 1. Исправление критической ошибки: загрузка компиляторов для выбранной версии Qt
- [x] **`findCompilerDirs()`** — заменена проверка `qmake.exe` на `windeployqt.exe` в [`QtFolderScanner.cpp`](QtFolderScanner.cpp:260). Теперь компиляторы определяются по наличию `windeployqt.exe`, а `qmake.exe` стал опциональным (Qt 6.8+ может не содержать qmake).
- [x] **Удалён мёртвый код** — методы `scanDirectory()` и `isValidQtDirectory()` удалены из [`QtFolderScanner.h`](QtFolderScanner.h) и [`QtFolderScanner.cpp`](QtFolderScanner.cpp), так как не использовались.
- [x] **`getEnvironmentCommands()`** — добавлен `break` после нахождения конфигурации в цикле; теперь не продолжает обход без необходимости.
- [x] **`getCompilers()`** — добавлена проверка `dirMap.contains(version)` и логирование при пустом результате.

### 2. Исправление race condition в QML
- [x] В [`main.qml`](main.qml) добавлено свойство `selectedQtVersion`, которое хранит выбранную версию Qt отдельно от `qtVersionCombo.currentText`.
- [x] `compilerCombo.onCurrentIndexChanged` теперь читает `selectedQtVersion`, а не `qtVersionCombo.currentText`, который мог не успеть обновиться.
- [x] `updateCompilers()` принимает параметр `version` и использует его, а не читает из ComboBox.

### 3. Обновление тестов
- [x] Удалён `testIsValidQtDirectory()` — соответствующий метод удалён.
- [x] Добавлен `testFindCompilerDirsWithWindeployqt()` — проверка валидной структуры Qt.
- [x] Добавлен `testFindCompilerDirsWithoutQmake()` — проверка, что компилятор находится без `qmake.exe`.

---

## — Приоритет 1: Миграция на Qt 6.8.3 —

### 1.1 Обновление CMakeLists.txt
- [x] `find_package(QT NAMES Qt6 Qt5 ...)` — уже поддерживает обе версии
- [x] Установить минимальную версию CMake 3.21+ (для Qt 6.8)
- [x] Убрать `if(${QT_VERSION} VERSION_LESS 6.1.0)` — Qt 6.8.3 > 6.1, упростить
- [x] Использовать `qt_standard_project_setup()` (Qt 6)

### 1.2 Правки в C++ коде для Qt 6
- [ ] **`QString::SkipEmptyParts`** в [`DeploymentManager.cpp`](DeploymentManager.cpp:71) — в Qt 6 заменён на `Qt::SkipEmptyParts`
- [ ] Убрать `QT_VERSION < QT_VERSION_CHECK(6, 0, 0)` блок в [`main.cpp`](main.cpp:12) (Qt 6+ не требуется)

### 1.3 Правки в QML для Qt 6
- [ ] **`FileDialog` из Qt Quick Dialogs 1.3** в [`main.qml`](main.qml:4) — deprecated в Qt 6, мигрировать на `QtQuick.Dialogs` (уже частично — используется `QtQuick.Dialogs`, но проверить совместимость)

## — Приоритет 2: Чистота кода и архитектура —

### 2.1 Удаление мёртвого кода
- [ ] Закомментированные аргументы в [`DeploymentManager.cpp`](DeploymentManager.cpp:92-94)
- [ ] Закомментированный блок `startDetached` в [`DeploymentManager.cpp`](DeploymentManager.cpp:121-124)
- [ ] Закомментированные `qDebug()` вызовы по всему проекту

### 2.2 Магические строки → константы/constexpr
- [x] `"windeployqt.exe"`, `"Qt5Core.dll"`, `"Qt6Core.dll"` — имена утилит (вынесены в [`Constants.h`](Constants.h))
- [x] `"qtenv2.bat"`, `"qmake.exe"` — имена файлов Qt (вынесены в [`Constants.h`](Constants.h))
- [x] `"set PATH="`, `"file:///"` — префиксы парсинга (вынесены в [`Constants.h`](Constants.h))
- [x] `"excludedDrives"` — ключ QSettings (вынесен в [`Constants.h`](Constants.h))

### 2.3 Улучшение именования методов
- [x] `DeploymentManager::setCompilerPath()` → переименован в `setWinDeployQtPath()`
- [ ] `DeploymentManager::setEnvironmentCommands()` — добавить комментарий о формате
- [x] `QtFolderScanner::getDirMap()` → переименован в `getCompilerPathsMap()`

### 2.4 Разделение ответственности
- [ ] Выделить парсинг `qtenv2.bat` из `QtFolderScanner` в отдельный метод/класс `EnvironmentParser`
- [ ] `DeploymentManager` освободить от бизнес-логики (оставить только управление `QProcess`)
- [ ] Создать слой сервисов между C++ и QML (пока логика простая — можно отложить)

## — Приоритет 3: Обработка ошибок и надёжность —

### 3.1 Проверка результата `QProcess::start()`
- [x] В [`DeploymentManager::startDeployment()`](DeploymentManager.cpp:119) — проверено возвращаемое значение `start()` и выводится сообщение об ошибке

### 3.2 Таймаут для QProcess
- [x] Установлен таймаут через `QTimer::singleShot()` / `QTimer` с таймером
- [x] При превышении таймаута — убить процесс (`kill()`) и вывести сообщение

### 3.3 Обработка ошибок сканирования
- [x] В [`QtFolderScanner::scanDrives()`](QtFolderScanner.cpp:29) — обработать `QStorageInfo` и доступ к дискам через try-catch или проверку прав
- [x] Выводить warnings, но не прерывать сканирование

### 3.4 Валидация путей на стороне QML
- [x] В [`main.qml`](main.qml) — добавлена проверка `.exe` расширения с подсветкой красным
- [ ] Добавить проверку существования `.exe` файла перед отправкой в C++
- [ ] Показывать сообщение пользователю, если для выбранной версии Qt не найдены компиляторы

### 3.5 Баги в QML (исправление)
- [ ] **race condition при смене версии Qt** — исправлено (см. "Выполнено в текущей итерации")
- [ ] В строке 41: `if (!folder.isEmpty)` — `folder` не определён, должно быть `qmlFolderDialog.folder.toString()`
- [ ] В строке 107: `text: qsTr(model.version)` — в современных QtQml нужно `model.modelData` или использовать `modelData` роль

## — Приоритет 4: Улучшение пользовательского интерфейса —

### 4.1 Индикатор прогресса сканирования
- [x] Привязать `BusyIndicator` к `scanner.isScanning`
- [x] Отображать "Сканирование..." пока `isScanning == true`

### 4.2 Запоминание последних путей (QSettings)
- [x] Сохранять в `QSettings` пути: `exePath`, `qmlDir`, выбранные `qtVersion` и `compiler`
- [x] Восстанавливать их при загрузке приложения

### 4.3 Кнопка "Сбросить настройки"
- [x] На вкладке "Настройки" — добавлена кнопка "Сбросить все настройки"
- [x] Очищает: исключённые диски, сохранённые пути, возвращает всё к дефолту
- [x] Подтверждение перед сбросом (диалог)

### 4.4 Поддержка темы (светлая/тёмная)
- [x] Использовать `QtQuick.Controls` Material тему
- [x] Переключатель на вкладке "Настройки"
- [x] Сохранять выбор в `QSettings`

### 4.5 Экспорт логов в файл
- [x] Кнопка "Сохранить лог" под областью логов
- [x] Сохранять в `.txt` или `.log` файл с меткой времени
- [x] Использовать `FileDialog` для выбора пути сохранения

## — Приоритет 5: Тестирование и CI —

### 5.1 Unit-тесты для QtFolderScanner
- [x] Создан CMakeLists.txt для тестов в `/tests/`
- [x] Использовать `Qt Test` (`QTest`)
- [x] Тесты: `getQtVersions()`, `getCompilers()`, `getDrivesList()`, `getWinDeployQtPath()`
- [ ] Добавить тест для `getEnvironmentCommands()` с мок-файлом `qtenv2.bat`
- [ ] Добавить интеграционный тест: создание полной структуры Qt, вызов `scanSystem()`, проверка `getQtVersions()` и `getCompilers()`

### 5.2 Unit-тесты для DeploymentManager
- [x] Использовать `QSignalSpy` для проверки сигналов
- [x] Тесты: валидация путей, разбор команд окружения, обработка ошибок
- [ ] Добавить тест для `setWinDeployQtPath()` и `getEnvironmentCommands()` в связке

### 5.3 CI/CD (GitHub Actions)
- [x] Файл `.github/workflows/build.yml`
- [ ] Установка Qt 6.8.3 через `aqtinstall` (или другой менеджер)
- [ ] Сборка с CMake + Ninja
- [ ] Запуск тестов (если есть)
- [ ] Артефакт сборки

## — Приоритет 6: Документация —

### 6.1 README.md
- [x] Описание проекта (что это, зачем)
- [ ] Скриншоты GUI
- [x] Требования: Qt 6.8.3, MinGW/MSVC, CMake 3.21+
- [x] Инструкция по сборке:
  ```bash
  mkdir build && cd build
  cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64"
  cmake --build .
  ```
- [x] Пример использования (пошагово)
- [x] Структура проекта

### 6.2 CONTRIBUTING.md
- [ ] Стандарт кода (C++17, QML JS)
- [ ] Правила именования коммитов
- [ ] Процесс ревью
- [ ] Настройка окружения разработчика

## — Приоритет 7: CMake и сборка —

### 7.1 Миграция на современный CMake
- [x] Минимальная версия: 3.21
- [x] Использовать `qt_standard_project_setup()` (Qt 6)
- [x] Упростить ветвление Qt5/Qt6 (убрать Qt5-специфичный код)

### 7.2 CPack для инсталлятора
- [x] Настроить `CPack` с генератором NSIS (Windows)
- [ ] Включить в установку все DLL (через `windeployqt` для самого приложения)
- [ ] Версионирование инсталлятора

### 7.3 Пакетное развертывание (опционально)
- [ ] Поддержка выбора нескольких `.exe` за раз
- [ ] Запуск `windeployqt` последовательно для каждого

---

## — Дополнительные предложения —

### 8.1 Обработка ситуации "не найдены компиляторы"
- [ ] В [`main.qml`](main.qml) при `updateCompilers()` проверять, что `scanner.getCompilers(version)` не пуст, и если пуст — показывать Label с предупреждением "Компиляторы не найдены для выбранной версии Qt"
- [ ] Добавить иконку/цветовое выделение для `compilerCombo` при пустом списке

### 8.2 Выбор компилятора по умолчанию при смене версии Qt
- [ ] Текущая логика: при смене версии Qt выбирается первый компилятор. Можно улучшить: сохранять маппинг "версия Qt → выбранный компилятор" и восстанавливать его.
- [ ] Сохранять в QSettings выбранный компилятор для каждой версии Qt.

### 8.3 Логирование сканирования
- [ ] Добавить сигнал `scanProgress(int percent)` в [`QtFolderScanner`](QtFolderScanner.h) для отображения прогресса сканирования
- [ ] Логировать в консоль/лог найденные пути Qt и компиляторов

### 8.4 Улучшение структуры `QtConfig`
- [ ] В `QtConfig` поле `qmakePath` теперь может быть пустым. Убедиться, что все потребители этого поля (например, `getEnvironmentCommands()`) корректно обрабатывают пустой путь.
- [ ] Рассмотреть замену `QList<QtConfig>` на `QMap<QString /*version*/, QMap<QString /*compilerType*/, QtConfig>>` для более быстрого доступа.

### 8.5 Потенциальные проблемы
- [ ] **`dirMap` и `filesMap` — дублирование информации**: `dirMap` хранит полные пути компиляторов, `filesMap` — пути к установкам. Проверить, оба ли нужны, или можно объединить.
- [ ] **`QMap<QString, QVector<QString>>` как Q_PROPERTY** — не регистрируется в метасистеме Qt, поэтому `getCompilerPathsMap()` помечена как `Q_INVOKABLE`, а не `Q_PROPERTY`. Это нормально, но стоит документировать.

---

*Текущий статус: [x] — выполнено, [ ] — предстоит выполнить*
