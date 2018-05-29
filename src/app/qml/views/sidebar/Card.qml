import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../../actions"
import "../components"

Pane {
    id: card
    property int uid
    property string displayName
    property var inputs
    property var parameters
    property var outputs
    property alias expanded: headerArrow.checked

    Material.elevation: 4

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

    Container {
        anchors.fill: parent
        leftPadding: 4
        rightPadding: 4
        clip: true
        contentItem: ColumnLayout {
            id: layout

            RowLayout {
                id: header

                Layout.fillWidth: true
                ArrowCheckBox {
                    id: headerArrow    
                    text: card.displayName
                    Layout.fillWidth: true
                    font: card.font
                    // TODO: indicate the output image colors even if the card is closed
                }

                Button {
                    text: "x"
                    onClicked: AppActions.requestRemoveModule(card.uid)
                }
            }

            // TODO: hide when no inputs
            HorizontalDivider {
                Layout.fillWidth: true
                visible: card.inputs.count != 0
            }

            CardInputs {
                id: inputsRepeater
                model: card.inputs
                uid: card.uid
                font: card.font
            }

            // TODO: hide when no params
            HorizontalDivider {
                Layout.fillWidth: true
                visible: card.parameters.count != 0
            }

            CardParameters {
                id: parametersRepeater
                model: card.parameters
                uid: card.uid
                font: card.font
            }

            // TODO: hide when no outputs
            HorizontalDivider {
                Layout.fillWidth: true
                visible: card.outputs.count != 0
            }

            CardOutputs {
                model: card.outputs
                uid: card.uid
                font: card.font
            }
        }
    }
}
