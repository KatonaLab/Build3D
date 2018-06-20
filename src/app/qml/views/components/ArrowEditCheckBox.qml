import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property bool checked: false
    property string arrowCharacter: "\u25B6"
    property alias text: textField.text
    property alias font: textField.font
    property alias staticText: staticLabel.text
    property int spinDuration: 200
    
    implicitHeight: layout.height
    signal titleTextChanged(string text);

    RowLayout {
        id: layout
        Label {
            id: arrow
            text: arrowCharacter

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    checked = !checked;
                }
            }
        }

        ColumnLayout {
            TextField {
                id: textField
                onTextEdited: {
                    titleTextChanged(textField.text);
                }
            }

            Label {
                id: staticLabel
                font: textField.font
                Component.onCompleted: {
                    font.pixelSize = textField.font.pixelSize - 1;
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
