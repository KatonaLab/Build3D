import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.3

import "../components"
import "../../stores"

Repeater {
    id: root
    // TODO: not sure this is neccessary
    property int uid: -1

    delegate: Loader {
        id: loader
        property int uid: root.uid
        property var details: model
        Layout.fillWidth: true

        sourceComponent: {
            switch (model.type) {
                case "int": return intInputDelegate;
                case "enum": return enumDelegate;
                case "float": return floatInputDelegate;
                case "bool": return switchDelegate;
                case "string": return stringDelegate;
                case "url": return filenameDelegate;
                defualt: return unknownControllerDelegate;
            }
        }

        Component {
            id: unknownControllerDelegate
            Label {
                text: "invalid type for property '" + details.name + "'"
                color: Material.color(Material.Red)
                // TODO: action
            }
        }

        Component {
            id: intInputDelegate
            NumberInput {
                validator: IntValidator {}
                details: loader.details
                intType: true
            }
        }

        Component {
            id: floatInputDelegate
            NumberInput {
                validator: DoubleValidator {}
                details: loader.details
                intType: false
            }
        }

        Component {
            id: enumDelegate
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: details.name
                }
                ComboBox {
                    model: details.hints.enumNames
                    currentIndex: details.value.first
                    delegate: ItemDelegate {
                        width: parent.width
                        text: modelData
                        onClicked: {
                            details.value = {"first": index, "second": details.hints.enumValues[index]};
                        }
                    }
                    Component.onCompleted: {
                        if (details.value && details.value.first == -1) {
                            currentIndex = 0;
                        }
                    }
                }
            }
        }

        Component {
            id: switchDelegate
            Switch {
                text: details.name
                checked: details.value
                onCheckedChanged: {
                    details.value = checked;
                }
            }
        }

        Component {
            id: stringDelegate
            RowLayout {
                Label {
                    text: details.name
                }
                TextField {
                    Layout.fillWidth: true
                    text: details.value
                    onEditingFinished: {
                        details.value = text;
                    }
                }
            }
        }

        Component {
            id: filenameDelegate
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: details.name
                }
                Label {
                    id: filenameText
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                    text: details.value ? details.value.toString() : ""
                }
                Button {
                    Layout.fillWidth: true
                    text: details.hints.folder ? "select folder" : "select file"
                    onClicked: {
                        dialog.open();
                    }

                    FileDialog {
                        id: dialog
                        title: details.hints.folder ? "Select Folder" : "Select File"
                        folder: ModuleStore.dialogFolder
                        selectMultiple: details.hints.multipleFiles || false
                        selectFolder: details.hints.folder || false
                        onAccepted: {
                            details.value = dialog.fileUrl;
                            ModuleStore.dialogFolder = dialog.folder;
                        }
                    }
                }
            }
        }
    }
}
