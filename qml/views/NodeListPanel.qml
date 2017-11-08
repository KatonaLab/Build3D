import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

GroupBox {
    id: root

    property QtObject nodeManager;

    function addNode(protoUrl, removable)
    {
        var settingsComponent = Qt.createComponent(protoUrl);
        var itemComponent = Qt.createComponent("NodeListItem.qml");
        var itemObject = itemComponent.createObject(nodeViewList,
            {"Layout.fillWidth": true,
            "nodeSettingsComponent": settingsComponent,
            "removable": removable});
    }

    Action {
        id: addSourceNode
        text: "Add Source"
        onTriggered: addNode(undefined, false)
    }

    Action {
        id: addSegmentationNode
        text: "Add Segmentation"
        // onTriggered: addNode("qrc:/qml/nodes/SegmentNode.ui.qml", true)
        onTriggered: addNode("../nodes/SegmentNode.ui.qml", true)
    }

    Action {
        id: addAnalysisNode
        text: "Add Analysis"
        // onTriggered: addNode("qrc:/qml/nodes/AnalysisNode.ui.qml", true)
        onTriggered: addNode("../nodes/AnalysisNode.ui.qml", true)
    }

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            width: parent.parent.width

            ColumnLayout {
                id: nodeViewList
                
                anchors.fill: parent
                spacing: 4
            }

            Button {
                text: "+"
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