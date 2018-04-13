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

    delegate: ColumnLayout {
        Layout.fillWidth: true
        property var moduleModel: model

        Label {
            font: root.font
            text: model.displayName
            Layout.fillWidth: true
        }

        ComboBox {
            font: root.font
            Layout.fillWidth: true
            textRole: "displayName"

            model: ListModel {
                id: comboModel
                ListElement {
                    displayName: "module2/port0"
                    targetUid: 2
                    targetOutputIndex: 0
                }
                ListElement {
                    displayName: "module4/port1"
                    targetUid: 4
                    targetOutputIndex: 1
                }
            }

            onCurrentIndexChanged: {
                var item = comboModel.get(currentIndex);
                var values = {
                    targetUid: item.targetUid,
                    targetOutputIndex: item.targetOutputIndex
                };
                AppActions.requestModulePropertyChange(root.uid, moduleModel.index, values);
            }
        }
    }
}

