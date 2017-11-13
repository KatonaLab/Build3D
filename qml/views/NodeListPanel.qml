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
                        Layout.fillWidth: true

                        // TODO: fix this componentSource usage
                        componentSource: model.componentSource ? model.componentSource : ""
                        removable: model.componentSource != null
                        name: model.name

                        visibleChecked: model.viewAttributes.visible
                        // TODO: fix DualSlider to be able to set these bindings
                        // lowCutValue: model.viewAttributes.lowCut
                        // highCutValue: model.viewAttributes.highCut
                        nodeColor: model.viewAttributes.color
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