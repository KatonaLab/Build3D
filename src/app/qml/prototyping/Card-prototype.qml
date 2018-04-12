
import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls.Material 2.2

import "../views/sidebar"

Window {
    visible: true
    width: 1024
    height: 768
    title: qsTr("Card Prototype")

    Material.theme: Material.Light
    Material.accent: Material.Teal

    Card {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 100
        width: 300
        uid: 42
        displayName: "Demo Card"
        font.pointSize: 12

        inputs: ListModel {
            Component.onCompleted: {
                append([{"displayName": "test input", "type": "volume"}]);
                append([{"displayName": "another test input", "type": "number"}]);
            }
        }

        parameters: ListModel {
            Component.onCompleted: {
                append([
                    {"displayName": "Do Other Things", "type": "button"},
                    {"displayName": "Edit Other Things", "type": "edit"},
                    {"displayName": "Select Something",
                        "type": "combobox",
                        "options": [
                            {"text": "Red"},
                            {"text": "Green"},
                            {"text": "Blue"},
                            {"text": "Violet"}
                        ]
                    },
                    {"displayName": "Range That Value", "type": "range"},
                    {"displayName": "Switch That Switch", "type": "switch"},
                    {"displayName": "Switch That Too", "type": "switch"}]);
            }
        }

        outputs: ListModel {
            Component.onCompleted: {
                append([{"displayName": "Fancy Input", "type": "volume"}]);
                append([{"displayName": "Fancy Input 2", "type": "volume"}]);
            }
        }
    }
}
