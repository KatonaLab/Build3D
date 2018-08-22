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

    Container {
        anchors.fill: parent
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
                    font: card.font
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
                    text: "x"
                    onClicked: baseModel.removeModule(moduleDetails.uid)
                }
            }

            CardInputs {
                id: inputsRepeater
                model: card.inputs
                uid: moduleDetails.uid
                font: card.font
                baseModel: card.baseModel
            }

            CardParameters {
                id: parametersRepeater
                model: card.parameters
                uid: moduleDetails.uid
                font: card.font
            }

            CardOutputs {
                model: card.outputs
                uid: moduleDetails.uid
                font: card.font
            }
        }
    }
}
