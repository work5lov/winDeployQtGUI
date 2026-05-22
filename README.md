# winDeployQtGUI

**Графическая обёртка над `windeployqt`** для удобного развёртывания Qt-приложений под Windows.

![Qt 6.8](https://img.shields.io/badge/Qt-6.8.3-green)
![CMake 3.21+](https://img.shields.io/badge/CMake-3.21%2B-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

---

## Возможности

- 🔍 **Автоматическое сканирование дисков** — поиск установленных версий Qt и компиляторов
- 🎯 **Выбор `windeployqt`** — автоматическое определение пути к утилите развёртывания
- 📁 **Поддержка QML** — развёртывание QML-приложений с ключом `--qmldir`
- ⚙️ **Настройка окружения** — автоматический парсинг `qtenv2.bat` для корректного окружения
- 🎨 **Тёмная/светлая тема** — переключение через настройки
- 💾 **Сохранение настроек** — запоминание последних путей и выбранных опций

## Требования

| Компонент | Версия |
|-----------|--------|
| Qt | 6.8.3+ (или 5.15 для обратной совместимости) |
| CMake | 3.21+ |
| Компилятор | MinGW 11+ / MSVC 2022 |
| Система | Windows 10 / 11 |

## Сборка

### Из командной строки

```bash
# Клонирование
git clone https://github.com/your-username/winDeployQtGUI.git
cd winDeployQtGUI

# Конфигурация
cmake -B build -G "Ninja" ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64" ^
    -DCMAKE_BUILD_TYPE=Release

# Сборка
cmake --build build --config Release

# Запуск
build/Release/winDeployQtGUI.exe
```

### Из Qt Creator

1. Открыть `CMakeLists.txt` как проект
2. Выбрать kit (например, `Qt 6.8.3 for MinGW 64-bit`)
3. Нажать **Configure Project**
4. Собрать (Ctrl+B) и запустить (Ctrl+R)

## Использование

1. **Запустите приложение** — оно автоматически просканирует диски в поиске Qt-установок
2. **Выберите `.exe` файл** вашего приложения для развёртывания
3. **Выберите версию Qt** и компилятор из найденных
4. **Укажите QML-директорию** (если приложение использует QML)
5. **Нажмите "Запустить развертывание"** — `windeployqt` скопирует все необходимые DLL

## Структура проекта

```
winDeployQtGUI/
├── CMakeLists.txt              # Конфигурация сборки
├── Constants.h                 # Константы и магические строки
├── main.cpp                    # Точка входа
├── main.qml                    # Пользовательский интерфейс (Qt Quick)
├── DeploymentManager.h/.cpp    # Управление процессом windeployqt
├── QtFolderScanner.h/.cpp      # Сканирование Qt-установок
├── qml.qrc                     # Ресурсы QML
├── tests/                      # Unit-тесты
│   ├── CMakeLists.txt
│   ├── tst_QtFolderScanner.cpp
│   └── tst_DeploymentManager.cpp
├── .github/workflows/          # CI/CD
│   └── build.yml
└── IMPROVEMENTS.md             # План улучшений
```

## Лицензия

Проект распространяется под лицензией [MIT](LICENSE).
