import QtQuick 6.0

Window {
    visible: true
    width: 400
    height: 300
    title: "Тест"
    
    Rectangle {
        anchors.fill: parent
        color: "lightblue"
        
        Text {
            anchors.centerIn: parent
            text: "Тестовий додаток Qt6"
            font.pixelSize: 24
            color: "black"
        }
    }
}
