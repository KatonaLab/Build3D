import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import QtQuick.Window 2.0

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
                            var moduleStore = MainStore.moduleStore;
                            var sm = moduleStore.sceneModel;
                            for (var i = 0; i < sm.count; ++i) {
                                var uid = sm.get(i).uid;
                                var node = moduleStore.getNode(uid);
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

                var input0SceneNode = MainStore.moduleStore.getSceneNode(input0Uid);
                var input1SceneNode = MainStore.moduleStore.getSceneNode(input1Uid);
                var input2SceneNode = MainStore.moduleStore.getSceneNode(input2Uid);
                var input3SceneNode = MainStore.moduleStore.getSceneNode(input3Uid);

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

        Button {
            text: "Show Results"
            enabled: nodeApplied
            Layout.fillWidth: true
            onClicked: {
                resultsWindow.updateTable();
                resultsWindow.visible = false;
                resultsWindow.visible = true;
            }
        }

        Button {
            text: "Save as CSV" 
            enabled: nodeApplied
            Layout.fillWidth: true
            onClicked: {
                AppActions.saveAnalysisCsv(box.uid, "defaultname.csv");
            }
        }

        Window {
            id: resultsWindow

            width: 500
            height: 600

            function updateTable() {
                var node = MainStore.moduleStore.getNode(box.uid);
                list.clear();
                for (var i = 0; i < node.nodeParams.count; ++i) {
                    list.append({
                        channelName: node.nodeParams.get(i).channelName,
                        objectId: node.nodeParams.get(i).objectId,
                        volume: node.nodeParams.get(i).volume,
                        sumIntensity: node.nodeParams.get(i).sumIntensity,
                        meanIntensity: node.nodeParams.get(i).meanIntensity,
                        overlapRatio: node.nodeParams.get(i).overlapRatio,
                        intersectingVolume: node.nodeParams.get(i).intersectingVolume,
                        centerX: node.nodeParams.get(i).centerX,
                        centerY: node.nodeParams.get(i).centerY,
                        centerZ: node.nodeParams.get(i).centerZ,
                        intensityWeightCenterX: node.nodeParams.get(i).intensityWeightCenterX,
                        intensityWeightCenterY: node.nodeParams.get(i).intensityWeightCenterY,
                        intensityWeightCenterZ: node.nodeParams.get(i).intensityWeightCenterZ
                    });
                }
            }

            ListModel {
                id: list
            }

            TableView {
                anchors.fill: parent
                model: list
                selectionMode: SelectionMode.ExtendedSelection

                TableViewColumn{ role: "channelName"; title: "channelName" }
                TableViewColumn{ role: "objectId"; title: "objectId" }
                TableViewColumn{ role: "volume"; title: "volume" }
                TableViewColumn{ role: "sumIntensity"; title: "sumIntensity" }
                TableViewColumn{ role: "meanIntensity"; title: "meanIntensity" }
                TableViewColumn{ role: "overlapRatio"; title: "overlapRatio" }
                TableViewColumn{ role: "intersectingVolume"; title: "intersectingVolume" }
                TableViewColumn{ role: "centerX"; title: "centerX" }
                TableViewColumn{ role: "centerY"; title: "centerY" }
                TableViewColumn{ role: "centerZ"; title: "centerZ" }
                TableViewColumn{ role: "intensityWeightCenterX"; title: "intensityWeightCenterX" }
                TableViewColumn{ role: "intensityWeightCenterY"; title: "intensityWeightCenterY" }
                TableViewColumn{ role: "intensityWeightCenterZ"; title: "intensityWeightCenterZ" }
            }
        }
    }
}
