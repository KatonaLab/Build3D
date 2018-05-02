import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Controls 2.2

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

        DynamicComboBox {
            id: comboBox
            options: [
                {targetUid: 17, targetPortId: 3, targetModuleDisplayName: "module17", targetPortDisplayName: "port3"},
                {targetUid: 19, targetPortId: 1, targetModuleDisplayName: "module19", targetPortDisplayName: "port1"}]
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
                var newList = comboBox.options;
                newList.push(randomItem());
                comboBox.options = newList;
            }
        }

        Button {
            text: "remove"
            onClicked: {
                var newList = comboBox.options;
                newList.splice(-1, 1);
                comboBox.options = newList;
            }
        }
    }
}
