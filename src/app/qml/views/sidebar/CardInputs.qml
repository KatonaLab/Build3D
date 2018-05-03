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
                delayed: false
            }

            Binding {
                target: comboBox
                property: "optionForceReset"
                value: inputOptionForceReset
                // NOTE: pretty important, see CardStore's handler
                // of ActionTypes.module_input_changed_notification
                // TODO: find a cleaner way
                delayed: false
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
                    AppActions.requestModuleInputChange(uid, portId, curr);
                }

                onOptionRemoved: function (prev) {
                    // TOOD: nothing to do?
                }
            }
        }
    }
}

