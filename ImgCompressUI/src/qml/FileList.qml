import QtQuick 6.0
import QtQuick.Controls 6.0


ListView {
    id: fileListView
    clip: true
    
    delegate: FileItem {
        width: fileListView.width
        height: 60
    }
    
    ScrollBar.vertical: ScrollBar {
        active: true
    }
}
