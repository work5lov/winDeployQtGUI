// main.qml — Qt 6.8.3 compatible with full UI improvements
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs
import QtQuick.Layouts 1.15
import Deployment 1.0
import Scanner 1.0

ApplicationWindow {
    id: window
    width: 720
    height: 520
    visible: true

    minimumWidth: 680
    minimumHeight: 480

    title: qsTr("windeployqt GUI")

    QtFolderScanner { id: scanner }
    DeploymentManager { id: deployManager }

    // Модели для ComboBox
    ListModel { id: qtVersionsModel }
    ListModel { id: compilerListModel }

    // Храним выбранную версию Qt отдельно, чтобы избежать race condition
    // при смене индекса qtVersionCombo и компилятора
    property string selectedQtVersion: ""

    // Применяем тему при загрузке
    Component.onCompleted: {
        deployManager.loadSettings()

        // Устанавливаем сохранённые пути
        if (deployManager.savedExePath)
            exePath.text = deployManager.savedExePath
        if (deployManager.savedQmlDir)
            qmlDir.text = deployManager.savedQmlDir

        // Устанавливаем тему
        applyTheme(deployManager.theme)
    }

    // Функция применения темы
    function applyTheme(theme) {
        if (theme === "Dark") {
            Material.theme = Material.Dark
            Material.accent = Material.Teal
        } else {
            Material.theme = Material.Light
            Material.accent = Material.DeepOrange
        }
    }

    // Диалог выбора исполняемого файла (Qt 6)
    FileDialog
    {
        id: exeFileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: [qsTr("Исполняемые файлы (*.exe)")]
        onAccepted: {
            if (selectedFile)
                exePath.text = selectedFile.toString().replace("file:///", "")
        }
    }

    // Диалог выбора QML-папки (Qt 6 — FolderDialog)
    FolderDialog
    {
        id: qmlFolderDialog
        title: qsTr("Выберите QML-директорию")
        onAccepted: {
            if (selectedFolder)
                qmlDir.text = selectedFolder.toString().replace("file:///", "")
        }
    }

    // Диалог сохранения лога
    FileDialog {
        id: saveLogDialog
        fileMode: FileDialog.SaveFile
        title: qsTr("Сохранить лог")
        nameFilters: [qsTr("Текстовые файлы (*.txt *.log)")]
        defaultSuffix: "log"
        onAccepted: {
            if (currentFile)
                deployManager.exportLog(currentFile.toString().replace("file:///", ""), logText.text)
        }
    }

    // Диалог подтверждения сброса настроек
    Dialog {
        id: resetDialog
        title: qsTr("Сбросить настройки")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true

        Label {
            text: qsTr("Вы уверены, что хотите сбросить все настройки?")
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            deployManager.resetAllSettings()
            exePath.text = ""
            qmlDir.text = ""
            qtVersionCombo.currentIndex = -1
            compilerCombo.currentIndex = -1
            selectedQtVersion = ""
            // Сбрасываем исключения дисков
            var emptyMap = {}
            scanner.excludedDrivesMap = emptyMap
        }
    }

    TabBar {
        id: tabBar
        anchors.top: parent.top
        width: parent.width
        currentIndex: stackLayout.currentIndex

        TabButton { text: qsTr("Развертывание") }
        TabButton { text: qsTr("Настройки") }
    }

    SwipeView {
        id: stackLayout
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 5
        currentIndex: tabBar.currentIndex
        interactive: false

        // === Вкладка "Развертывание" ===
        Page
        {
            ScrollView {
                anchors.fill: parent
                clip: true
                padding: 5

                ColumnLayout
                {
                    width: parent.width - 10
                    spacing: 8

                    // Выбор исполняемого файла
                    Label {
                        text: qsTr("Исполняемый файл:")
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5

                        TextField
                        {
                            id: exePath
                            Layout.fillWidth: true
                            placeholderText: qsTr("Путь к .exe например C:\\MyApp\\release\\app.exe")

                            background: Rectangle {
                                border.color: (exePath.text.length > 0 && !exePath.text.toLowerCase().endsWith(".exe"))
                                             ? "red" : (exePath.activeFocus ? Material.accent : "#ccc")
                                border.width: 1
                                radius: 3
                            }
                        }
                        Button {
                            text: qsTr("Обзор")
                            implicitWidth: 80
                            onClicked: exeFileDialog.open()
                        }
                    }

                    // Выбор Qt версии и компилятора
                    Label {
                        text: qsTr("Версия Qt и компилятор:")
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5

                        ComboBox
                        {
                            id: qtVersionCombo
                            Layout.fillWidth: true
                            model: qtVersionsModel
                            textRole: "version"

                            delegate: ItemDelegate {
                                width: parent.width
                                text: model.version
                            }

                            Component.onCompleted: {
                                // Запускаем сканирование при загрузке
                                scanner.scanSystem()
                            }

                            onCurrentIndexChanged: {
                                if (currentIndex >= 0 && qtVersionsModel.count > 0) {
                                    var version = qtVersionsModel.get(currentIndex).version
                                    selectedQtVersion = version
                                    deployManager.setQtVersion(version)
                                    updateCompilers(version)
                                }
                            }
                        }

                        ComboBox {
                            id: compilerCombo
                            Layout.fillWidth: true
                            model: compilerListModel
                            textRole: "text"
                            enabled: compilerListModel.count > 0

                            onCurrentIndexChanged: {
                                if (currentIndex < 0 || compilerListModel.count === 0) return
                                if (!selectedQtVersion) return
                                const compilerPath = compilerListModel.get(currentIndex).text
                                const winDeployPath = scanner.getWinDeployQtPath(
                                    selectedQtVersion, compilerPath)
                                var modifiedString = winDeployPath.split("/").join("\\");
                                deployManager.setWinDeployQtPath(modifiedString)
                                deployManager.setEnvironmentCommands(scanner.getEnvironmentCommands(
                                    selectedQtVersion, compilerPath))
                            }
                        }
                    }

                    // Выбор QML-директории
                    Label {
                        text: qsTr("QML директория:")
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        TextField {
                            id: qmlDir
                            Layout.fillWidth: true
                            placeholderText: qsTr("Путь к папке с QML файлами например C:\\MyApp\\qml")
                        }
                        Button {
                            text: qsTr("Обзор")
                            implicitWidth: 80
                            onClicked: qmlFolderDialog.open()
                        }
                    }

                    // Кнопка запуска и индикатор
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        Button
                        {
                            id: deployButton
                            text: deployManager.isRunning
                                  ? qsTr("Выполняется...")
                                  : qsTr("Запустить развертывание")
                            highlighted: !deployManager.isRunning
                            enabled: !deployManager.isRunning
                            Layout.fillWidth: true

                            onClicked: {
                                deployManager.setExecutablePath(exePath.text)
                                deployManager.setQmlDirectory(qmlDir.text)
                                deployManager.saveSettings()
                                deployManager.startDeployment()
                            }
                        }

                        BusyIndicator {
                            id: runningIndicator
                            running: deployManager.isRunning
                            visible: deployManager.isRunning
                            height: 32
                            width: 32
                        }
                    }

                    // Индикатор сканирования
                    RowLayout {
                        Layout.fillWidth: true
                        visible: scanner.isScanning
                        spacing: 5

                        BusyIndicator { running: true; height: 20; width: 20 }
                        Label {
                            text: qsTr("Сканирование дисков...")
                            color: Material.accent
                            font.italic: true
                        }
                    }

                    // Лог
                    Label {
                        text: qsTr("Лог:")
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    ScrollView
                    {
                        id: logScrollView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumHeight: 120
                        clip: true

                        TextArea
                        {
                            id: logText
                            text: ""
                            readOnly: true
                            wrapMode: TextArea.Wrap
                            font.family: "Courier New"
                            font.pointSize: 9
                            onContentHeightChanged: {
                                if (logScrollView.ScrollBar.vertical)
                                    logScrollView.ScrollBar.vertical.position = 1.0
                            }
                        }
                    }

                    // Кнопка сохранения лога
                    Button {
                        text: qsTr("Сохранить лог")
                        enabled: logText.text.length > 0
                        Layout.fillWidth: true
                        onClicked: saveLogDialog.open()
                    }
                }
            }
        }

        // === Вкладка "Настройки" ===
        Page {
            ScrollView {
                anchors.fill: parent
                clip: true
                padding: 10

                Column {
                    width: parent.width - 20
                    spacing: 12

                    // Раздел "Исключённые диски"
                    Label {
                        text: qsTr("Исключить диски из сканирования:")
                        font.bold: true
                        font.pointSize: 12
                    }

                    Rectangle {
                        width: parent.width
                        height: 150
                        border.color: "#ccc"
                        border.width: 1
                        radius: 4

                        ListView {
                            id: drivesListView
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 2
                            clip: true

                            model: ListModel { id: drivesModel }

                            delegate: CheckBox {
                                text: model.driveName
                                checked: scanner.excludedDrivesMap[model.driveName] || false

                                onCheckedChanged: {
                                    var newMap = scanner.excludedDrivesMap
                                    newMap[model.driveName] = checked
                                    scanner.excludedDrivesMap = newMap
                                }
                            }
                        }
                    }

                    // Раздел "Тема"
                    Label {
                        text: qsTr("Тема оформления:")
                        font.bold: true
                        font.pointSize: 12
                    }

                    Row {
                        spacing: 10

                        RadioButton {
                            text: qsTr("Светлая")
                            checked: deployManager.theme === "Light"
                            onClicked: deployManager.setTheme("Light")
                        }

                        RadioButton {
                            text: qsTr("Тёмная")
                            checked: deployManager.theme === "Dark"
                            onClicked: deployManager.setTheme("Dark")
                        }
                    }

                    // Кнопка "Сбросить настройки"
                    Button {
                        text: qsTr("Сбросить все настройки")
                        highlighted: true
                        Material.background: Material.Red
                        Material.foreground: "white"
                        onClicked: resetDialog.open()
                    }
                }
            }

            // Заполняем модель дисков при загрузке
            Component.onCompleted: {
                const drives = scanner.getDrivesList()
                drivesModel.clear()
                if (drives) {
                    for (const drive of drives)
                        drivesModel.append({ driveName: drive })
                }
            }
        }
    }

    // Обновление списков при сканировании
    Connections {
        target: scanner
        function onScanningChanged(){
            // Обрабатываем только завершение сканирования (isScanning == false)
            if (!scanner.isScanning) {
                qtVersionsModel.clear();
                const versions = scanner.getQtVersions()
                if (versions && versions.length > 0) {
                    for (var i = 0; i < versions.length; ++i)
                        qtVersionsModel.append({ version: versions[i] })
                    // Автоматически выбираем первую найденную версию Qt
                    qtVersionCombo.currentIndex = 0
                }
            }
        }
    }

    // Обновление темы
    Connections {
        target: deployManager
        function onThemeChanged(){
            applyTheme(deployManager.theme)
        }
    }

    // Вспомогательная функция — обновить список компиляторов по текущей версии Qt
    function updateCompilers(version) {
        compilerListModel.clear()
        if (!version) version = selectedQtVersion
        if (!version) return

        const compilers = scanner.getCompilers(version)
        if (compilers && compilers.length > 0) {
            for (var i = 0; i < compilers.length; ++i) {
                compilerListModel.append({ text: compilers[i] })
            }
            // Выбираем первый компилятор по умолчанию
            compilerCombo.currentIndex = 0
        }
    }
}
