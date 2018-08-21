import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml 2.2
import QtQml.Models 2.2
import QtQuick.Controls.Material 2.2
import koki.katonalab.a3dc 1.0

Pane {
    id: root
    property var baseModel

    padding: 12

    BackendStoreFilter {
        id: moduleList
        source: baseModel
        includeCategory: ["module"]
    }

    ListView {
        id: listView

        anchors.fill: parent
        spacing: 8

        model: moduleList
        delegate: Card {
            moduleDetails: model
            baseModel: root.baseModel

            BackendStoreFilter {
                id: inputsModel
                source: baseModel
                includeCategory: ["input"]
                includeParentUid: [model.uid]
            }

            BackendStoreFilter {
                id: parametersModel
                source: baseModel
                includeCategory: ["parameter"]
                includeParentUid: [model.uid]
            }

            BackendStoreFilter {
                id: outputsModel
                source: baseModel
                includeCategory: ["output"]
                includeParentUid: [model.uid]
            }

            inputs: inputsModel
            parameters: parametersModel
            outputs: outputsModel
            width: parent.width
            expanded: true
            font.pointSize: 11
        }

        header: Column {
            anchors.horizontalCenter: parent.horizontalCenter
            RoundButton {
                text: "run"
                onClicked: {
                    baseModel.evaluate(-1);
                }
                // TODO:
                //Material.background: configurationUpToDate ? Material.LightGreen : Material.Amber
            }

            Rectangle { color: "transparent"; height: 16; width: 1 }
        }

        footer: Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle { color: "transparent"; height: 16; width: 1 }

            RoundButton {
                text: "+"
                onClicked: {
                    rootMenu.open();
                }

                Menu {
                    id: rootMenu
                    Instantiator {
                        id: groupsInstantiator
                        model: baseModel.availableModules
                        Menu {
                            id: itemMenu
                            title: modelData.name
                            Repeater {
                                id: menuItemRepeater
                                model: modelData.files
                                MenuItem {
                                    id: itemMenu
                                    text: modelData.name
                                    onTriggered: {
                                        baseModel.addModule(modelData.path);
                                    }
                                }
                            }
                        }
                        onObjectAdded: rootMenu.insertMenu(index, object)
                        onObjectRemoved: rootMenu.removeMenu(object)
                    }
                    MenuSeparator {}
                    MenuItem {
                        text: "refresh module list"
                        onTriggered: {
                            baseModel.refreshAvailableModules();
                        }
                    }
                }
            }
        }
    }
}
