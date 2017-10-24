import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.0

Rectangle {
    id: root

    ColorDialog {
        id: dialog
        modality: Qt.WindowModal
        title: "Choose a color"
        color: root.color
        showAlphaChannel: false
        onAccepted: { root.color = color }
    }

    implicitWidth: 16
    implicitHeight: 16
    radius: width * 0.5

    MouseArea {
        anchors.fill: parent
        onClicked: { dialog.open(); }
    }
}