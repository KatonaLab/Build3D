import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property bool checked: false
    property string arrowCharacter: "\u25B6"
    property alias text: label.text
    property alias font: label.font
    property int spinDuration: 200
    
    RowLayout {

        Label {
            id: arrow
            text: arrowCharacter
        }

        Label {
            id: label
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                checked = !checked;
            }
        }

        states:  State {
            name: "checked"
            when: checked
            PropertyChanges {
                target: arrow
                rotation: 90
            }
        }

        transitions: Transition {
            from: ""
            to: "checked"
            reversible: true
            NumberAnimation {
                target: arrow
                property: "rotation"
                duration: spinDuration
                easing.type: Easing.InOutBack
            }
        }
    }
}
