import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import com.myapp 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Qt Deployment Tool"

    QtObject {
        id: backend
        property var qtManager: QtVersionManager {}
    }

    Column {
        spacing: 20
        // padding: 10

        // Выбор исполняемого файла
        FileDialog {
            id: exeFileDialog
            nameFilters: ["Executables (*.exe)"]
        }

        Button {
            text: "Выбрать исполняемый файл"
            onClicked: exeFileDialog.open()
        }

        Label {
            text: exeFileDialog.fileUrl
        }

        // Рекомендуемая версия Qt
        TextField {
            id: recommendedQtVersion
            text: backend.recommendQtVersion(exeFileDialog.fileUrl)
            placeholderText: "Рекомендуемая версия Qt"
        }

        // Выбор версии Qt
        ComboBox {
            id: qtVersionSelector
            model: backend.qtManager.qtVersions.keys()
        }

        // Режим QML
        CheckBox {
            id: qmlMode
            text: "Включить режим QML"
        }

        FileDialog {
            id: folderDialog
            title: "Выберите папку"
            selectFolder: true
            folder: shortcuts.home
            onAccepted: {
                console.log("Выбранная папка:", folderDialog.folder)
            }
            onRejected: {
                console.log("Выбор папки отменен")
            }
        }


        // FolderDialog {
        //     id: qmlFolderDialog
        // }

        Button {
            visible: qmlMode.checked
            text: "Выбрать папку с QML"
            onClicked: folderDialog.open()
        }

        // Кнопка запуска команды windeployqt
        Button {
            text: "Запустить windeployqt"
            onClicked: {
                // TODO: реализовать запуск команды
            }
        }
    }
}
