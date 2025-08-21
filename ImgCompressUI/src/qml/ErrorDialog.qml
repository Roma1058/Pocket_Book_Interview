import QtQuick 6.0
import QtQuick.Controls 6.0

import QtQuick.Layouts 6.0


Dialog {
    id: errorDialog
    title: "Помилка"
    modal: true
    visible: false
    
    width: 400
    height: 200
    
    function showError(message) {
        errorText.text = message
        visible = true
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        Text {
            id: errorText
            text: "Невідомий файл"
            font.pixelSize: 16
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        
        Button {
            id: okButton
            text: "Ок"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 100
            Layout.preferredHeight: 40
            
            background: Rectangle {
                color: okButton.pressed ? "#1976D2" : "#2196F3"
                radius: 4
                
                Behavior on color {
                    ColorAnimation { duration: 150 }
                }
            }
            
            contentItem: Text {
                text: okButton.text
                color: "white"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            onClicked: {
                errorDialog.visible = false
            }
        }
    }
}
