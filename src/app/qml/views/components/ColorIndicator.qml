import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    property string title: "Choose a color"
//    property color color: dialog.currentColor

    ColorDialog {
        id: dialog
        modality: Qt.WindowModal
        title: root.title
        color: root.color
        showAlphaChannel: false
        onCurrentColorChanged: {
            root.color = currentColor;
        }

        onAccepted: {
            root.color = color;
        }

        onRejected: {
            root.color = color;
        }
    }

    implicitWidth: 16
    implicitHeight: 16
    radius: width * 0.5

    MouseArea {
        anchors.fill: parent
        onClicked: { dialog.open(); }
    }
}

