import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0
import "ui-controls"

Rectangle {
    id: root    

    property alias text: visibleCheck.text
    property alias from: slider.lowValue
    property alias to: slider.highValue
    property alias channelColor: colorSelect.color
    property alias channelVisible: visibleCheck.checked

    width: 200
    implicitHeight: 56
    radius: 4
    border {
        color: Qt.rgba(0, 0, 0, 0.1)
        width: 1
    }
    color: Qt.rgba(0,0,0,0)

    CheckBox {
        id: visibleCheck
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width - colorSelect.width
        anchors.margins: 8
    }

    ColorIndicator {
        id: colorSelect
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 8
    }

    DualSlider {
        id: slider
        anchors.left: parent.left
        anchors.top: visibleCheck.bottom
        anchors.topMargin: 8
        width: parent.width - 17*2
        anchors.margins: 8
    }
}