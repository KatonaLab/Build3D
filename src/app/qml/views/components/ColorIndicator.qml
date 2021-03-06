import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Rectangle {
    id: root
    property string title: "Choose a color"

    signal selectedColorChanged(color c)

    ColorDialog {
        id: dialog
        modality: Qt.WindowModal
        title: root.title
        color: root.color
        showAlphaChannel: false
        onCurrentColorChanged: {
            root.selectedColorChanged(currentColor);
        }

        onAccepted: {
            root.selectedColorChanged(color);
        }

        onRejected: {
            root.selectedColorChanged(color);
        }
    }

    implicitWidth: 24
    implicitHeight: 24
    radius: width * 0.5

    MouseArea {
        anchors.fill: parent
        onClicked: { dialog.open(); }
    }
}

