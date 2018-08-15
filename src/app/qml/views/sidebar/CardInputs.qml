import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2
import koki.katonalab.a3dc 1.0

import "../../stores"
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
            property var details: model
            Layout.fillWidth: true

            Label {
                font: root.font
                text: details.name
                Layout.fillWidth: true
            }

            ComboBox {
                id: comboBox
                Layout.fillWidth: true
                model: BackendStoreFilter {
                    source: MainStore.moduleStore.model
                    includeCategory: ["output"]
                    excludeParentUid: [root.uid]
                    includeType: [details.type]
                }
                delegate: ItemDelegate {
                    width: parent.width
                    BackendStoreFilter {
                        id: moduleDetails
                        source: MainStore.moduleStore.model
                        includeCategory: ["module"]
                        includeUid: [model.parentUid]
                    }
                    text: moduleDetails.first.name + " : " + model.name + " - " + model.type + " " + model.category
                    onClicked: {
                        var success = MainStore.moduleStore.model.connect(
                            model.parentUid, model.uid,
                            details.parentUid, details.uid);
                        if (success) {
                            comboBox.currentIndex = index;
                            comboBox.displayText = Qt.binding(function() { return text; });
                        }
                    }
                }
            }

        }
    }
}

