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
            visible: nodeApplied
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
                    text: "Input"
                }

                ComboBox {
                    id: inputSelector
                    Layout.fillWidth: true
                    model: MainStore.nodeStore.model
                    textRole: "nodeName"
                }

                Label {
                    text: "Method"
                }
                
                ComboBox {
                    id: methodSelector
                    enabled: !(manualCheckbox.checked)
                    Layout.fillWidth: true
                    model: ListModel {
                        ListElement {text: "Method 1"; key: "m1"}
                        ListElement {text: "Method 2"; key: "m2"}
                        ListElement {text: "Method 3"; key: "m3"}
                        ListElement {text: "Method 4"; key: "m4"}
                    }
                }

                CheckBox {
                    id: manualCheckbox
                    text: "Use Input LUT"
                }
            }
        }

        Button {
            text: "Apply"
            Layout.fillWidth: true
            onClicked: {
                var inputUid = MainStore.nodeStore.model.get(inputSelector.currentIndex).uid;
                var inputSceneNode = MainStore.nodeStore.getSceneNode(inputUid);
                var inputData = inputSceneNode.data;
                var param0 = inputSceneNode.nodeViewParams.lowCut;
                var param1 = inputSceneNode.nodeViewParams.highCut;
                var method = methodSelector.model.get(methodSelector.currentIndex).key;
                var segmentParams = {
                    data: inputData, method: method, param0: param0, param1: param1
                };
                AppActions.applySegmentNode(uid, segmentParams);
            }
        }
    }
}