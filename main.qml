// main.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import Deployment 1.0
import Scanner 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true

    QtFolderScanner { id: scanner }
    DeploymentManager { id: deployManager }

    // Объявляем диалоги отдельно
    FileDialog {
        id: exeFileDialog
        selectExisting: true
        nameFilters: ["Исполняемые файлы (*.exe)"]
        onAccepted: exePath.text = fileUrl.toString().replace("file:///", "")
    }

    FileDialog {
        id: qmlFolderDialog
        selectFolder: true // Разрешаем выбор папок
        nameFilters: []
        onAccepted: {
            if (!folder.isEmpty) {
                qmlDir.text = folder.toString().replace("file:///", "")
            }
        }
    }

    Column {
        anchors.centerIn: parent
        // width: parent.width
        spacing: 10

        // Выбор исполняемого файла
        Row {
            spacing: 5
            TextField { id: exePath; placeholderText: "Путь к .exe" }
            Button {
                text: "Выбрать"
                onClicked: exeFileDialog.open() // Правильный вызов
            }
        }

        // Выбор версии Qt
        ComboBox {
            id: qtVersionCombo
            model: ListModel { id: qtVersionsModel }
            textRole: "version"

            delegate: ItemDelegate {
                width: parent.width
                text: model.version
            }

            Component.onCompleted: {
                scanner.scanSystem()
                const versions = scanner.getQtVersions()
                versions.forEach(version => {
                    qtVersionsModel.append({ version: version })
                })
                // console.log("Загружены версии Qt:", versions)
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0) {
                    var selectedVersion = qtVersionsModel.get(currentIndex).version
                    deployManager.setQtVersion(selectedVersion)
                    // console.log("Выбранная версия Qt:", selectedVersion)
                }
            }
        }

        // Комбобокс выбора компилятора
        ComboBox {
            width:parent.width
            id: compilerCombo
            model: scanner.getCompilers(qtVersionCombo.currentText) || []

            onCurrentTextChanged: {
                const compilerPath = currentText


                // Получаем путь к windeployqt
                const winDeployPath = scanner.getWinDeployQtPath(
                    qtVersionCombo.currentText,
                    compilerPath
                )

                var modifiedString = winDeployPath.split("/").join("\\");

                deployManager.setCompilerPath(modifiedString)
                // console.log("WinDeployQt path:", winDeployPath)
            }
        }

        // Выбор QML-директории
        Row {
            spacing: 5
            TextField { id: qmlDir; placeholderText: "QML-директория" }
            Button {
                text: "Выбрать"
                onClicked: qmlFolderDialog.open() // Правильный вызов
            }
        }

        // Кнопка запуска
        Button {
            text: "Запустить windeployqt"
            enabled: !deployManager.isRunning
            onClicked: {
                deployManager.setExecutablePath(exePath.text)
                deployManager.setQmlDirectory(qmlDir.text)
                deployManager.startDeployment()
            }
        }

        // Лог
        ScrollView {
            id: logScrollView
            width: parent.width
            height: 200
            clip: true

            Text {
                id: logText
                width: logScrollView.width
                text: ""
                wrapMode: Text.Wrap // Перенос текста по словам
            }

            Connections {
                target: deployManager
                function onOutputReceived(msg) {
                    logText.text += msg + "\n"; // Добавляем новую строку
                    // Убедитесь, что flickableItem существует перед обращением к нему
                    if (logScrollView.flickableItem) {
                        logScrollView.flickableItem.contentY = logScrollView.flickableItem.contentHeight; // Прокручиваем вниз
                    }
                }
            }
        }
    }
}
