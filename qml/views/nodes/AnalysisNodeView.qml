import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

import "../controls"
import "../../actions"
import "../../stores"

GroupBox {
    id: box

    property int uid: -1
    property string nodeName
    property var nodeViewParams
    property bool nodeApplied

    Action {
        id: removeAction
        text: "Remove"
        onTriggered: {
            AppActions.removeNode(uid);
        }
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: nodeName
                Layout.fillWidth: true
            }
            FontelloButton {
                text: "\uE822"
                action: removeAction
            }
        }

        NodeViewOptions {
            uid: box.uid
            enabled: nodeApplied
            width: 0 // breaking a binding loop (?), TODO
            nodeViewParams: box.nodeViewParams
            Layout.fillWidth: true
        }

        GroupBox {
            enabled: !nodeApplied
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 2
                anchors.fill: parent
                
                Label {
                    text: "Inputs"
                }

                ComboBox {
                    id: input0Selector
                    Layout.fillWidth: true
                    textRole: "text"
                    model: ListModel {
                        Component.onCompleted: {
                            var nodeStore = MainStore.nodeStore;
                            var sm = nodeStore.sceneModel;
                            for (var i = 0; i < sm.count; ++i) {
                                var uid = sm.get(i).uid;
                                var node = nodeStore.getNode(uid);
                                append({text: node.nodeName, uid: uid});
                            }
                        }
                    }
                }

                ComboBox {
                    id: input1Selector
                    Layout.fillWidth: true
                    textRole: "text"
                    model: input0Selector.model
                }

                Label {
                    text: "Segmented Inputs"
                }

                ComboBox {
                    id: input2Selector
                    Layout.fillWidth: true
                    textRole: "text"
                    model: input0Selector.model
                }

                ComboBox {
                    id: input3Selector
                    Layout.fillWidth: true
                    textRole: "text"
                    model: input0Selector.model
                }
            }
        }

        Button {
            text: "Apply"
            enabled: !nodeApplied
            Layout.fillWidth: true
            onClicked: {
                var input0Uid = input0Selector.model.get(input0Selector.currentIndex).uid;
                var input1Uid = input1Selector.model.get(input1Selector.currentIndex).uid;
                var input2Uid = input2Selector.model.get(input2Selector.currentIndex).uid;
                var input3Uid = input3Selector.model.get(input3Selector.currentIndex).uid;

                var input0SceneNode = MainStore.nodeStore.getSceneNode(input0Uid);
                var input1SceneNode = MainStore.nodeStore.getSceneNode(input1Uid);
                var input2SceneNode = MainStore.nodeStore.getSceneNode(input2Uid);
                var input3SceneNode = MainStore.nodeStore.getSceneNode(input3Uid);

                var input0Data = input0SceneNode.data;
                var input1Data = input1SceneNode.data;
                var input2Data = input2SceneNode.data;
                var input3Data = input3SceneNode.data;

                var analysisParams = {
                    data0: input0Data, data1: input1Data,
                    segData0: input2Data, segData1: input3Data
                };
                console.log("nodeview sent");
                AppActions.applyAnalysisNode(uid, analysisParams);
            }
        }
    }
}