import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

import ImgCompressUI 1.0
import "." 1.0

Window {
    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: "ImgCompress UI - Стискання зображень"

    property string initialDirectoryFromContext: initialDirectory || ""

    property bool hasInitialDirectory: initialDirectory && initialDirectory.length > 0

    FileListModel {
        id: fileModel

        onDirectoryChanged: {
            currentDirectory = fileModel.getCurrentDirectoryPath()
        }
    }

    CompressionWorker {
        id: compressionWorker

        onOperationCompleted: function(success, message) {
            if (currentFileIndex >= 0) {
                if (success) {                
                    let index = currentFileIndex
                    Qt.callLater(function() {
                        fileModel.updateFileStatus(index, "Завершено", 100)
                    })

                    Qt.callLater(function() {
                        refreshFileListWithoutDoneFile(index)
                    })

                } else {
                    let index = currentFileIndex
                    Qt.callLater(function() {
                        fileModel.updateFileStatus(index, "Помилка", 0)
                    })

                    errorDialog.showMessage("Помилка: " + message)
                }
            }
            currentFileIndex = -1
        }

        onStatusChanged: function(status) {            
            if (currentFileIndex >= 0) {
                let index = currentFileIndex
                let s = status
                Qt.callLater(function() {
                    fileModel.updateFileStatus(index, s, 0)
                })
            } else {
                console.log("currentFileIndex = -1, статус НЕ оновлюється")
            }
        }
    }

    property int currentFileIndex: -1
    property string initialDirectory: ""
    property string currentDirectory: "."

    Rectangle {
        id: dialogBackground
        visible: false
        anchors.fill: parent
        color: "black"
        opacity: 0.5
        z: 999

        MouseArea {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: errorDialog
        visible: false
        width: 400
        height: 200
        color: "#f5f5f5"
        border.color: "#d0d0d0"
        border.width: 1
        radius: 8
        z: 1000
        focus: visible

        anchors.centerIn: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Text {
                text: "Повідомлення"
                font.pixelSize: 18
                font.bold: true
                color: "#2196F3"
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                id: errorText
                text: "Повідомлення"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.fillHeight: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                width: 100
                height: 40
                color: okButton.pressed ? "#cccccc" : "#4CAF50"
                border.color: "#45a049"
                border.width: 1
                radius: 4
                Layout.alignment: Qt.AlignHCenter

                Text {
                    anchors.centerIn: parent
                    text: "OK"
                    color: "white"
                    font.pixelSize: 14
                    font.bold: true
                }

                MouseArea {
                    id: okButton
                    anchors.fill: parent
                    onClicked: {
                        errorDialog.visible = false
                        dialogBackground.visible = false
                    }
                }
            }
        }

        function showMessage(message) {
            errorText.text = message
            dialogBackground.visible = true
            visible = true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Text {
            text: "Стискання зображень"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Поточна директорія:"
                font.pixelSize: 14
                font.bold: true
                color: "#333333"
            }

            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: "#f5f5f5"
                border.color: "#d0d0d0"
                border.width: 1
                radius: 4

                Text {
                    id: directoryPath
                    anchors.fill: parent
                    anchors.margins: 10
                    text: currentDirectory
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    color: "#555555"
                    elide: Text.ElideLeft
                }
            }
            Rectangle {
                width: 100
                height: 40
                color: refreshButton.pressed ? "#cccccc" : "#4CAF50"
                border.color: "#45a049"
                border.width: 1
                radius: 4

                Text {
                    text: "Оновити"
                    anchors.centerIn: parent
                    font.pixelSize: 14
                    color: "white"
                }

                MouseArea {
                    id: refreshButton
                    anchors.fill: parent
                    onClicked: {
                        refreshFileList()
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            border.color: "#d0d0d0"
            border.width: 1
            radius: 4

            ListView {
                id: fileListView
                anchors.fill: parent
                anchors.margins: 10
                model: fileModel
                spacing: 5
                clip: true

                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                    visible: fileListView.contentHeight > fileListView.height
                }

                ScrollBar.horizontal: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                    visible: fileListView.contentWidth > fileListView.width
                }

                delegate: FileItem {
                    width: fileListView.width - 30
                    height: 100
                    onCompressClicked: handleCompressClick(index)
                    onDecompressClicked: handleDecompressClick(index)
                }
            }
        }
    }

    function handleCompressClick(index) {
        currentFileIndex = index
        const filePath = fileModel.getFilePath(index)

        const extension = filePath.split('.').pop().toLowerCase()

        if (extension === "bmp") {
            const outputPath = filePath.replace(".bmp", "_packed.barch")

            if (typeof compressionWorker.setOperation === 'function') {
                compressionWorker.setOperation(CompressionWorker.Compress, filePath, outputPath, index)
            } else {
                errorDialog.showMessage("Помилка: setOperation не є функцією")
            }
        } else if (extension === "png") {
            console.log("PNG файл:", filePath, "- тільки для перегляду")
        } else {
            errorDialog.showMessage("Можна кодувати тільки BMP файли")
        }
    }

    function handleDecompressClick(index) {
        currentFileIndex = index
        const filePath = fileModel.getFilePath(index)

        const extension = filePath.split('.').pop().toLowerCase()

        if (extension === "barch") {
            const outputPath = filePath.replace("_packed.barch", "_unpacked.bmp")

            if (typeof compressionWorker.setOperation === 'function') {
                compressionWorker.setOperation(CompressionWorker.Decompress, filePath, outputPath, index)
            } else {
                errorDialog.showMessage("Помилка: setOperation не є функцією")
            }
        } else if (extension === "png") {
            console.log("PNG файл:", filePath, "- тільки для перегляду")
        } else {
            errorDialog.showMessage("Можна розтискати тільки BARCH файли")
        }
    }

    function refreshFileList() {
        fileModel.loadDirectory(currentDirectory)
    }

    function refreshFileListWithoutDoneFile(index) {
       fileModel.loadDirectoryWitoutFileIndex(index, currentDirectory)
    }

    Component.onCompleted: {
        let path = ""

        if (globalInitialDirectory && globalInitialDirectory.length > 0) {
            path = globalInitialDirectory
        } else if (initialDirectory && initialDirectory.length > 0) {
            path = initialDirectory
        } else {
            path = "."
        }

        console.log("Фінальний path:", path)

        currentDirectory = path

        if (fileModel) {
            fileModel.loadDirectory(path)
        } else {
            console.log("fileModel ще не створена!")
        }
    }
}
