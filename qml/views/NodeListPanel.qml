import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "../actions"
import "../stores"
import "controls"

GroupBox {

    Action {
        id: addSegmentationNode
        text: "Add Segmentation"
        onTriggered: AppActions.addSegmentNode(AppActions.generateUid())
    }

    Action {
        id: addAnalysisNode
        text: "Add Analysis"
        onTriggered: AppActions.addAnalysisNode(AppActions.generateUid())
    }

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            width: parent.parent.width

            ColumnLayout {
                anchors.fill: parent
                spacing: 4

                Repeater {
                    model: MainStore.nodeStore.model
                    delegate: NodeListItem {
                        uid: model.uid
                        componentSource: model.componentSource ? model.componentSource : ""
                        Layout.fillWidth: true
                    }
                }
            }

            FontelloButton {
                text: "\uE827"
                Layout.fillWidth: true
                onClicked: {
                    menu.popup();
                }

                Menu {
                    id: menu
                    MenuItem { action: addSegmentationNode }
                    MenuItem { action: addAnalysisNode }
                }
            }
        }
    }
}