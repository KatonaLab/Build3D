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
                text: displayName
                Layout.fillWidth: true
            }

            ComboBox {
                property int currentUid: -1
                property int currentPort: -1

                font: root.font
                Layout.fillWidth: true
                textRole: "displayName"
                model: options

                onActivated: {
                    var item = model.get(currentIndex);
                    currentUid = values.targetUid;
                    currentPort = values.targetPortId;
                }

                onCurrentIndexChanged: {
                    var item = model.get(currentIndex);
                    var values = {
                        targetUid: item.targetUid,
                        targetPortId: item.targetPortId
                    };
                    AppActions.requestModuleInputChange(root.uid, portId, values);
                }
            }
        }
    }
}

