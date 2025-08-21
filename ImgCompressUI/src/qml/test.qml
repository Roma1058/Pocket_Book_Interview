import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Window {
    visible: true
    width: 400
    height: 300
    title: "Тест"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        
        Text {
            text: "Тестовий додаток"
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }
        
        Button {
            text: "Тестова кнопка"
            Layout.alignment: Qt.AlignHCenter
            onClicked: console.log("Кнопка натиснута")
        }
    }
}
