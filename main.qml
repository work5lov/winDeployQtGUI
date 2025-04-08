// main.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import Deployment 1.0
import Scanner 1.0

ApplicationWindow {
    width: 540
    height: 350
    visible: true

    minimumWidth: width
    minimumHeight: height

    maximumWidth: width
    maximumHeight: height

    title: qsTr("windeployqt GUI")

    QtFolderScanner { id: scanner }
    DeploymentManager { id: deployManager }

    // Объявляем диалоги отдельно
    FileDialog
    {
        id: exeFileDialog
        selectExisting: true
        nameFilters: [qsTr("Исполняемые файлы (*.exe)")]
        onAccepted: exePath.text = fileUrl.toString().replace("file:///", "")
    }

    FileDialog
    {
        id: qmlFolderDialog
        selectFolder: true // Разрешаем выбор папок
        nameFilters: []
        onAccepted:
        {
            if (!folder.isEmpty)
            {
                qmlDir.text = folder.toString().replace("file:///", "")
            }
        }
    }

    TabBar {
        id: tabBar
        currentIndex: stackLayout.currentIndex
        TabButton { width: implicitWidth; height: implicitHeight; text: qsTr("Развертывание") }
        TabButton { width: implicitWidth; height: implicitHeight; text: qsTr("Настройки") }
    }

    StackLayout
    {
        id: stackLayout
        currentIndex: tabBar.currentIndex
        width: parent.width
        height: parent.height - tabBar.height - 5

        anchors.top:parent.top
        anchors.topMargin: tabBar.height + 5

        Page
        {
            Column
            {
                width: parent.width * 0.8
                height:  parent.height
                spacing: 10

                anchors.left: parent.left
                anchors.leftMargin: 5

                // Выбор исполняемого файла
                Row {
                    spacing: 5
                    TextField
                    {
                        id: exePath;
                        width: parent.width * 0.8
                        placeholderText: qsTr("Путь к .exe")
                    }
                    Button
                    {
                        text: qsTr("Выбрать")
                        onClicked: exeFileDialog.open() // Правильный вызов
                    }
                }

                Row
                {
                    spacing: 5

                    // Выбор версии Qt
                    ComboBox
                    {
                        id: qtVersionCombo
                        model: ListModel { id: qtVersionsModel }
                        textRole: "version"
                        // width: 35

                        delegate: ItemDelegate
                        {
                            // width: parent.width
                            text: qsTr(model.version)
                        }

                        Component.onCompleted:
                        {
                            scanner.scanSystem()
                            qtVersionsModel.clear();
                            const versions = scanner.getQtVersions()
                            versions.forEach(version =>{
                                qtVersionsModel.append({ version: version })
                            })
                            // console.log("Загружены версии Qt:", versions)
                        }

                        onCurrentIndexChanged:
                        {
                            if (currentIndex >= 0)
                            {
                                var selectedVersion = qtVersionsModel.get(currentIndex).version
                                deployManager.setQtVersion(selectedVersion)
                                // console.log("Выбранная версия Qt:", selectedVersion)
                            }
                        }
                    }

                    // Комбобокс выбора компилятора
                    ComboBox {
                        // width:parent.width
                        id: compilerCombo
                        model: scanner.getCompilers(qtVersionCombo.currentText) || []

                        onCurrentTextChanged:
                        {
                            const compilerPath = currentText


                            // Получаем путь к windeployqt
                            const winDeployPath = scanner.getWinDeployQtPath(
                                qtVersionCombo.currentText,
                                compilerPath
                            )

                            var modifiedString = winDeployPath.split("/").join("\\");

                            deployManager.setCompilerPath(modifiedString)
                        }
                    }
                }

                // Выбор QML-директории
                Row {
                    spacing: 5
                    TextField
                    {
                        id: qmlDir;
                        width: parent.width * 0.8
                        placeholderText: qsTr("QML-директория")
                    }
                    Button {
                        text: qsTr("Выбрать")
                        onClicked: qmlFolderDialog.open() // Правильный вызов
                    }
                }

                // Кнопка запуска
                Button
                {
                    text: qsTr("Запустить развертывание")
                    enabled: !deployManager.isRunning
                    onClicked:
                    {
                        deployManager.setExecutablePath(exePath.text)
                        deployManager.setQmlDirectory(qmlDir.text)
                        deployManager.startDeployment()
                    }
                }

                // Лог
                ScrollView
                {
                    id: logScrollView
                    width: parent.width
                    height: 100
                    clip: true

                    Text
                    {
                        id: logText
                        width: logScrollView.width
                        text: qsTr("")
                        wrapMode: Text.Wrap // Перенос текста по словам
                    }

                    Connections
                    {
                        target: deployManager
                        function onOutputReceived(msg)
                        {
                            logText.text += msg + "\n"; // Добавляем новую строку
                            // Убедитесь, что flickableItem существует перед обращением к нему
                            if (logScrollView.flickableItem)
                            {
                                logScrollView.flickableItem.contentY = logScrollView.flickableItem.contentHeight; // Прокручиваем вниз
                            }
                        }
                    }
                }
            }
        }

        Page {
            ScrollView {
                anchors.fill: parent
                contentWidth: parent.width
                implicitHeight: contentItem.childrenRect.height

                Label {
                    id: label
                    text: qsTr("Исключить диски из сканирования:")
                    font.bold: true
                }

                ListView {
                    width: parent.width
                    spacing: 10
                    // padding: 20
                    clip:true
                    height: parent.height - label.height - 10

                    anchors.top:parent.top
                    anchors.topMargin: label.height + 5

                    model: ListModel { id: drivesModel }

                    // Динамически генерируем чекбоксы для каждого диска
                    delegate: CheckBox {
                        text: qsTr(model.driveName)
                        checked: scanner.excludedDrivesMap[model.driveName] || false

                        onCheckedChanged: {
                            var newMap = scanner.excludedDrivesMap
                            newMap[model.driveName] = checked
                            scanner.excludedDrivesMap = newMap
                        }
                    }
                }
            }

            // Заполняем модель при загрузке
            Component.onCompleted: {
                const drives = scanner.getDrivesList()
                drivesModel.clear()

                for (const drive of drives) {
                    drivesModel.append({
                        driveName: drive
                    })
                }
            }
        }
    }

    Connections{
        target: scanner

        function onScanningChanged(){
            qtVersionsModel.clear();
            const versions = scanner.getQtVersions()
            versions.forEach(version =>{
                qtVersionsModel.append({ version: version })
            })
        }
    }
}
