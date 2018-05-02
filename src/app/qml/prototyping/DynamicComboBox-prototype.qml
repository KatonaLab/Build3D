import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Controls 2.2

import "../views/components"

Window {
    visible: true
    width: 1024
    height: 768
    title: qsTr("DynamicComboBox Prototype")

    Material.theme: Material.Light
    Material.accent: Material.Red

    function randomItem() {
        var moduleNum = Math.floor(Math.random() * 20);
        var portNum = Math.floor(Math.random() * 20);
        var moduleName = "mod" + moduleNum;
        var portName = "p" + portNum;
        return {
            "targetUid": moduleNum,
            "targetPortId": portNum,
            "targetModuleDisplayName": moduleName,
            "targetPortDisplayName": portName};
    }

    Column {
        anchors.centerIn: parent

        ListModel {
            id: dummyModel
            ListElement {targetUid: 17; targetPortId: 3; targetModuleDisplayName: "module17"; targetPortDisplayName: "port3"}
            ListElement {targetUid: 19; targetPortId: 1; targetModuleDisplayName: "module19"; targetPortDisplayName: "port1"}
        }

        DynamicComboBox {
            id: comboBox
            options: dummyModel
            optionNameGenerator: function (item) {
                return item.targetModuleDisplayName + ":" + item.targetPortDisplayName;
            }
            itemEqualsFunction: function(a, b) {
                return (a.targetUid === b.targetUid) && (a.targetPortId === b.targetPortId);
            }

            hasDefaultOption: true
            defaultOptionName: "- none -"

            onOptionSelected: function (curr, prev) {
                console.debug("onOptionSelected:");
                console.debug("\t", JSON.stringify(curr));
                console.debug("\t", JSON.stringify(prev));
            }

            onOptionRemoved: function (prev) {
                console.debug("onOptionRemoved:");
                console.debug("\t", JSON.stringify(prev));
            }
        }

        Button {
            text: "add"
            onClicked: {
                comboBox.options.append(randomItem());
            }
        }

        Button {
            text: "remove"
            onClicked: {
                comboBox.options.remove(comboBox.options.count - 1);
            }
        }
    }
}
