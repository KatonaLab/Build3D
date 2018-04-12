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
    property ListModel inputs
    property ListModel parameters
    property ListModel outputs
    property alias expanded: header.checked

    Material.elevation: 4

    contentWidth: layout.implicitWidth
    contentHeight: header.implicitHeight

    states: State {
        name: "opened"
        when: header.checked
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

            ArrowCheckBox {
                id: header
                text: card.displayName
                Layout.fillWidth: true
                font: card.font
            }

            CardInputs {
                id: inputsRepeater
                model: card.inputs
                uid: card.uid
                font: card.font
            }

            CardParameters {
                id: parametersRepeater
                model: card.parameters
                uid: card.uid
                font: card.font
            }

            CardOutputs {
                model: card.outputs
                uid: card.uid
                font: card.font
            }
        }
    }
}
