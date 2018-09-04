import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property bool checked: false
    property string arrowCharacter: "\u25B6"
    property alias text: textField.text
    property alias staticText: staticLabel.text
    property int spinDuration: 200
    
    implicitHeight: layout.height

    RowLayout {
        id: layout
        Label {
            id: arrow
            text: arrowCharacter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    checked = !checked;
                }
            }
        }

        RowLayout {
            TextField {
                id: textField
            }

            Label {
                id: staticLabel
                Component.onCompleted: {
                    font.pointSize = textField.font.pointSize - 1;
                }
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
