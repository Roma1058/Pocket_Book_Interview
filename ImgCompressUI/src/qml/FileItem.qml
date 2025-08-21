import QtQuick 6.0
import QtQuick.Controls 6.0

import QtQuick.Layouts 6.0

Rectangle {
    id: fileItem
    border.color: "#d0d0d0"
    border.width: 1
    radius: 4
    height: 100
    
    signal compressClicked()
    signal decompressClicked()
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Item {
            Layout.preferredWidth: 40
            Layout.fillHeight: true

            Rectangle {
                width: 50
                height: 50
                radius: 4
                anchors.centerIn: parent
                color: {
                    const ext = model.extension
                    if (ext === "bmp") return "#4CAF50"  // Зелений для BMP
                    if (ext === "barch") return "#2196F3" // Синій для BARCH
                    if (ext === "png") return "#FF9800"   // Помаранчевий для PNG
                    return "#9E9E9E" // Сірий для інших
                }

                Text {
                    anchors.centerIn: parent
                    text: model.extension.toUpperCase()
                    color: "white"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: model.name
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Text {
                text: "Розмір: " + model.size
                font.pixelSize: 12
                color: "#666666"
            }
        }

        Text {
            id: statusText
            Layout.alignment: Qt.AlignVCenter
            text: model.status
            font.pixelSize: 17
            color: {
                if (model.status === "Кодується" || model.status === "Розкодовується") return "#FF9800"
                if (model.status === "Помилка") return "#F44336"
                if (model.status === "Готово") return "#4CAF50"
                return "#666666"
            }
            font.bold: model.status === "Кодується" || model.status === "Розкодовується"
            visible: model.extension !== "png"
        }
        
        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            width: 80
            height: 50
            color: compressButton.pressed ? "#cccccc" : "#4CAF50"
            border.color: "#737373"
            border.width: 1
            radius: 4
            visible: model.extension === "bmp"  // Тільки для BMP
            
            Text {
                anchors.centerIn: parent
                text: "Стиснути"
                color: "white"
                font.pixelSize: 12
                font.bold: true
            }
            
            MouseArea {
                id: compressButton
                anchors.fill: parent
                onClicked: {
                    fileItem.compressClicked()
                }
            }
        }
        
        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            width: 80
            height: 50
            color: decompressButton.pressed ? "#cccccc" : "#2196F3"
            border.color: "#737373"
            border.width: 1
            radius: 4
            visible: model.extension === "barch"  // Тільки для BARCH
            
            Text {
                anchors.centerIn: parent
                text: "Розтиснути"
                color: "white"
                font.pixelSize: 12
                font.bold: true
            }
            
            MouseArea {
                id: decompressButton
                anchors.fill: parent
                onClicked: {
                    fileItem.decompressClicked()
                }
            }
        }
    }
}
