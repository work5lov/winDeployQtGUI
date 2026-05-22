# План: Добавление недостающих Qt-китов в cmake-tools-kits.json

## Текущее состояние

Файл: `C:\Users\Vlad\AppData\Local\CMakeTools\cmake-tools-kits.json`

**Уже есть:**
- Qt 6.8.0 MinGW 64-bit (mingw_64)
- Qt 6.8.3 MinGW 64-bit (mingw_64)
- Qt-5.15.16-win32-g++_from_PATH (из msys2, не из C:\Qt)

## Установленные Qt в C:\Qt

| Версия | Поддиректория | Компилятор | Полноценная? |
|--------|---------------|------------|-------------|
| **6.10.3** | mingw_64 | mingw1310_64 | ✅ Да |
| **6.10.3** | msvc2022_64 | MSVC 2022 | ✅ Да |
| **6.10.3** | llvm-mingw_64 | llvm-mingw1706_64 | ✅ Да |
| **6.8.0** | mingw_64 | mingw1310_64 | ✅ Да (уже есть) |
| **6.8.0** | msvc2022_64 | MSVC 2022 | ✅ Да |
| **6.8.0** | msvc2022_arm64 | MSVC 2022 arm64 | ✅ Да |
| **6.8.0** | llvm-mingw_64 | llvm-mingw1706_64 | ✅ Да |
| **6.8.3** | mingw_64 | mingw1310_64 | ✅ Да (уже есть) |
| **6.8.3** | msvc2022_64 | MSVC 2022 | ✅ Да |
| **6.8.3** | msvc2022_arm64 | MSVC 2022 arm64 | ✅ Да |
| **6.8.3** | llvm-mingw_64 | llvm-mingw1706_64 | ✅ Да |
| **5.15.2** | mingw81_64 | mingw810_64 | ✅ Да |
| **5.15.2** | mingw81_32 | mingw810_32 | ✅ Да |
| **5.15.0/5.15.1** | msvc2019_64 | MSVC 2019 | ✅ Да |
| **5.15.0/5.15.1** | msvc2019 | MSVC 2019 x86 | ✅ Да |
| **5.15.0/5.15.1** | mingw73_64 | mingw730_64 | ⚠️ Только Qt3DStudio |
| **5.15.0/5.15.1** | msvc2015_64 | MSVC 2015 | ✅ Да |

## Кандидаты на добавление

### Группа 1: Qt 6.10.3 (новейшая версия)

#### 1. Qt 6.10.3 MinGW 64-bit
- **toolchainFile:** `C:/Qt/6.10.3/mingw_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **Компиляторы:** `C:/Qt/Tools/mingw1310_64/bin/gcc.exe`, `g++.exe`
- **qtpaths:** `C:/Qt/6.10.3/mingw_64/bin/qtpaths.exe`
- **PATH:** `C:/Qt/Tools/mingw1310_64/bin;C:/Qt/6.10.3/mingw_64/bin;...`

#### 2. Qt 6.10.3 MSVC 2022 64-bit
- **toolchainFile:** `C:/Qt/6.10.3/msvc2022_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **visualStudio:** `81020f52` (Visual Studio Community 2022 Release)
- **visualStudioArchitecture:** `x64`
- **qtpaths:** `C:/Qt/6.10.3/msvc2022_64/bin/qtpaths.exe`

#### 3. Qt 6.10.3 LLVM-MinGW 64-bit
- **toolchainFile:** `C:/Qt/6.10.3/llvm-mingw_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **Компиляторы:** `C:/Qt/Tools/llvm-mingw1706_64/bin/clang.exe`, `clang++.exe`
- **qtpaths:** `C:/Qt/6.10.3/llvm-mingw_64/bin/qtpaths.exe`

### Группа 2: Qt 6.8.0 (дополнительные киты)

#### 4. Qt 6.8.0 MSVC 2022 64-bit
- **toolchainFile:** `C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **visualStudio:** `81020f52`
- **visualStudioArchitecture:** `x64`

#### 5. Qt 6.8.0 LLVM-MinGW 64-bit
- **toolchainFile:** `C:/Qt/6.8.0/llvm-mingw_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **Компиляторы:** `C:/Qt/Tools/llvm-mingw1706_64/bin/clang.exe`, `clang++.exe`

#### 6. Qt 6.8.0 MSVC 2022 ARM64
- **toolchainFile:** `C:/Qt/6.8.0/msvc2022_arm64/lib/cmake/Qt6/qt.toolchain.cmake`
- **visualStudio:** `81020f52`
- **visualStudioArchitecture:** `x64` (cross-compile)

### Группа 3: Qt 6.8.3 (дополнительные киты)

#### 7. Qt 6.8.3 MSVC 2022 64-bit
- **toolchainFile:** `C:/Qt/6.8.3/msvc2022_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **visualStudio:** `81020f52`
- **visualStudioArchitecture:** `x64`

#### 8. Qt 6.8.3 LLVM-MinGW 64-bit
- **toolchainFile:** `C:/Qt/6.8.3/llvm-mingw_64/lib/cmake/Qt6/qt.toolchain.cmake`
- **Компиляторы:** `C:/Qt/Tools/llvm-mingw1706_64/bin/clang.exe`, `clang++.exe`

#### 9. Qt 6.8.3 MSVC 2022 ARM64
- **toolchainFile:** `C:/Qt/6.8.3/msvc2022_arm64/lib/cmake/Qt6/qt.toolchain.cmake`
- **visualStudio:** `81020f52`
- **visualStudioArchitecture:** `x64` (cross-compile)

### Группа 4: Qt 5.x (классические версии)

#### 10. Qt 5.15.2 MinGW 64-bit
- **Компиляторы:** `C:/Qt/Tools/mingw810_64/bin/gcc.exe`, `g++.exe`
- **qmake:** `C:/Qt/5.15.2/mingw81_64/bin/qmake.exe`
- **cmakeSettings:** `CMAKE_PREFIX_PATH=C:/Qt/5.15.2/mingw81_64`

#### 11. Qt 5.15.2 MinGW 32-bit
- **Компиляторы:** `C:/Qt/Tools/mingw810_32/bin/gcc.exe`, `g++.exe`
- **qmake:** `C:/Qt/5.15.2/mingw81_32/bin/qmake.exe`
- **cmakeSettings:** `CMAKE_PREFIX_PATH=C:/Qt/5.15.2/mingw81_32`

#### 12. Qt 5.15.0/5.15.1 MSVC 2019 64-bit
- **visualStudio:** `9052826a` (VS Build Tools 2019)
- **visualStudioArchitecture:** `x64`
- **qmake:** `C:/Qt/5.15.1/msvc2019_64/bin/qmake.exe`
- **cmakeSettings:** `CMAKE_PREFIX_PATH=C:/Qt/5.15.1/msvc2019_64`

#### 13. Qt 5.15.0/5.15.1 MSVC 2019 32-bit
- **visualStudio:** `9052826a` (VS Build Tools 2019)
- **visualStudioArchitecture:** `x86`
- **qmake:** `C:/Qt/5.15.1/msvc2019/bin/qmake.exe`
- **cmakeSettings:** `CMAKE_PREFIX_PATH=C:/Qt/5.15.1/msvc2019`

## Важные замечания

1. **Qt5 vs Qt6:** Qt5 использует `CMAKE_PREFIX_PATH` для указания на Qt, а Qt6 использует `toolchainFile` с `qt.toolchain.cmake`.
2. **MSVC-киты** не указывают компиляторы отдельно — они ссылаются на Visual Studio через `visualStudio` + `visualStudioArchitecture`. Компилятор предоставляется самой VS.
3. **LLVM-MinGW-киты** используют компилятор clang из `C:/Qt/Tools/llvm-mingw1706_64/bin/`.
4. **ARM64-киты** предназначены для кросс-компиляции под ARM64.
5. **Qt 5.15.0/5.15.1** используют VS Build Tools 2019 (ID: `9052826a`), а **Qt 6.8.x/6.10.3 MSVC** — Visual Studio Community 2022 (ID: `81020f52`).

## Пошаговый план действий

1. Создать резервную копию текущего `cmake-tools-kits.json`
2. Добавить киты из **Группы 1** (Qt 6.10.3)
3. Добавить киты из **Группы 2** (Qt 6.8.0 дополнительные)
4. Добавить киты из **Группы 3** (Qt 6.8.3 дополнительные)
5. Добавить киты из **Группы 4** (Qt 5.15.x)
6. Проверить валидность JSON
