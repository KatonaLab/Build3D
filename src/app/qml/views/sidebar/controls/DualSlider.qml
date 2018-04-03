import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    id: root

    // TODO: fix the height, when no anchoring the knobs are off
    // TODO: the slider bed is not centered horizontally = the knob origin is not at its center

    property bool enabled: true    
    property real lowValue
    property real highValue

    QtObject {
        id: d
        property int knobSize: 17
        property int halfKnobSize: knobSize / 2
        property int bedHeight: 4
        property int indicatorHeight: 4
    }

    anchors.margins: d.halfKnobSize
    implicitWidth: 120
    implicitHeight: d.knobSize

    Rectangle {
        id: sliderBed

        width: parent.width
        height: d.bedHeight
        radius: d.bedHeight / 2
        color: palette.dark

        border {
            color: Qt.lighter(palette.dark, 1.1)
            width: 1
        }

        Rectangle {
            id: rangeIndicator
            height: d.indicatorHeight
            color: palette.highlight
            border {
                color: Qt.darker(palette.highlight, 1.1)
                width: 1
            }
            anchors {
                verticalCenter: parent.verticalCenter
                right: knobRight.left
                left: knobLeft.right
            }
        }

        SliderHandle {
            id: knobLeft
            minX: 0
            maxX: knobRight.x
            enabled: root.enabled
            x: sliderBed.width * lowValue
            onXChanged: {
                lowValue = x / sliderBed.width;
            }
        }

        SliderHandle {
            id: knobRight
            minX: knobLeft.x
            maxX: sliderBed.width
            enabled: root.enabled
            x: sliderBed.width * highValue
            onXChanged: {
                highValue = x / sliderBed.width;
            }
        }
    }

    Palette {id: palette; enabled: root.enabled}
}
