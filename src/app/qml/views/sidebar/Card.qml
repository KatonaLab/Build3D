import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../components"

Pane {
    id: card
    property var baseModel
    property var moduleDetails
    property var inputs
    property var parameters
    property var outputs
    property alias expanded: headerArrow.checked

    // from: https://stackoverflow.com/questions/7616461/generate-a-hash-from-string-in-javascript
    function stringHash(str) {
        var hash = 0, i, chr;
        if (str.length === 0) return hash;
        for (i = 0; i < str.length; i++) {
            chr   = str.charCodeAt(i);
            hash  = ((hash << 5) - hash) + chr;
            hash |= 0; // Convert to 32bit integer
        }
        return Math.abs(hash);
    }

    Material.elevation: 2

    contentWidth: layout.implicitWidth
    contentHeight: header.implicitHeight

    states: State {
        name: "opened"
        when: headerArrow.checked
        PropertyChanges {
            target: card
            contentHeight: layout.implicitHeight
        }
    }

    transitions: Transition {
        from: ""; to: "opened"; reversible: true
        NumberAnimation {
            properties: "contentHeight"
            easing.type: Easing.OutBack
            duration: 150
        }
    }

    Component.onCompleted: {
        headerArrow.checked = false;
    }

    RowLayout {
        anchors.fill: parent
        Rectangle {
            color: Qt.hsla((stringHash(moduleDetails.type) % 256)/256.0, 0.6, 0.7, 1.0)
            width: 4
            radius: 2
            Layout.fillHeight: true
        }

        Container {
            Layout.fillHeight: true
            Layout.fillWidth: true
            leftPadding: 2
            rightPadding: 2
            clip: true
            contentItem: ColumnLayout {
                id: layout
                RowLayout {
                    id: header
                    Layout.fillWidth: true
                    
                    ArrowEditCheckBox {
                        id: headerArrow
                        Layout.fillWidth: true
                        text: moduleDetails.name
                        staticText: moduleDetails.type
                        Binding {
                            target: moduleDetails
                            property: "name"
                            value: headerArrow.text
                        }
                        // TODO: indicate the output image colors even if the card is closed
                    }

                    ToolButton {
                        id: removeButton
                        // TODO: use fontellico icon for this
                        font.family: "fontello"
                        text: "\uE806"
                        onClicked: baseModel.removeModule(moduleDetails.uid)
                        visible: baseModel.editorMode
                    }
                }

                CardInputs {
                    id: inputsRepeater
                    model: card.inputs
                    uid: moduleDetails.uid
                    baseModel: card.baseModel
                }

                CardParameters {
                    id: parametersRepeater
                    model: card.parameters
                    uid: moduleDetails.uid
                }

                CardOutputs {
                    model: card.outputs
                    uid: moduleDetails.uid
                }
            }
        }
    }
}
