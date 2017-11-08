import QtQuick 2.0

Rectangle {
    id: root

    property bool enabled: true
    property int minX
    property int maxX

    width: d.knobSize
    height: d.knobSize
    radius: d.halfKnobSize
    color: palette.base
    anchors.verticalCenter: parent.verticalCenter

    border {
        color: Qt.rgba(0, 0, 0, 0.2)
        width: 1
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        enabled: root.enabled
        drag {
            target: parent
            axis: Drag.XAxis
            smoothed: false
            threshold: 0
            minimumX: root.minX
            maximumX: root.maxX
        }
    }

    Palette {id: palette; enabled: root.enabled}
}