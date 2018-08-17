import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2
import koki.katonalab.a3dc 1.0

import "../components"

Repeater {
    id: root
    property var baseModel
    // TODO: not sure this is neccessary
    property int uid: -1
    property font font

    delegate: inputDelegate

    Component {
        id: inputDelegate

        ColumnLayout {
            property var details: model
            Layout.fillWidth: true

            Label {
                font: root.font
                text: details.name + " (" + details.type + ")"
                Layout.fillWidth: true
            }

            ComboBox {
                id: comboBox
                property var connectedValue: details.value
                property bool valid: connectedValue &&
                    connectedValue.parentUid != -1 &&
                    connectedValue.uid != -1
                
                Layout.fillWidth: true
                displayText: valid ? (currentModule.first.name + "/" + currentPort.first.name) : "-"

                onConnectedValueChanged: {
                    if (connectedValue.parentUid === -1 ||
                        connectedValue.uid === -1) {
                            currentIndex = -1;
                        }
                }

                model: BackendStoreFilter {
                    source: baseModel
                    includeCategory: ["output"]
                    excludeParentUid: [root.uid]
                    includeType: [details.type]
                }

                delegate: ItemDelegate {
                    width: parent.width
                    BackendStoreFilter {
                        id: moduleDetails
                        source: baseModel
                        includeCategory: ["module"]
                        includeUid: [model.parentUid]
                    }
                    text: moduleDetails.first.name + "/" + model.name
                    onClicked: {
                        var success = baseModel.connect(
                            model.parentUid, model.uid,
                            details.parentUid, details.uid);
                        if (success) {
                            comboBox.currentIndex = index;
                        }
                    }
                }

                BackendStoreFilter {
                    id: currentModule
                    source: baseModel
                    includeCategory: ["module"]
                    includeUid: [comboBox.valid ? comboBox.connectedValue.parentUid : -1]
                }

                BackendStoreFilter {
                    id: currentPort
                    source: baseModel
                    includeCategory: ["output"]
                    includeParentUid: [comboBox.valid ? comboBox.connectedValue.parentUid : -1]
                    includeUid: [comboBox.valid ? comboBox.connectedValue.uid : -1]
                }

            }

        }
    }
}

