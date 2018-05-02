import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../../actions"
import "../components"

Repeater {
    id: root
    property int uid: -1
    property font font

    delegate: inputDelegate

    Component {
        id: inputDelegate

        ColumnLayout {
            Layout.fillWidth: true

            Label {
                font: root.font
                text: model.displayName
                Layout.fillWidth: true
            }

            Binding {
                target: comboBox
                property: "options"
                value: options
                delayed: true
            }

            DynamicComboBox {
                id: comboBox
                
                hasDefaultOption: true
                defaultOptionName: "- none -"
                Layout.fillWidth: true

                optionNameGenerator: function (item) {
                    return item.targetModuleDisplayName + ":" + item.targetPortDisplayName;
                }
                itemEqualsFunction: function(a, b) {
                    return (a.targetUid === b.targetUid) && (a.targetPortId === b.targetPortId);
                }

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
        }
    }
}

