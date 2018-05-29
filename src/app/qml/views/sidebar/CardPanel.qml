import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml 2.2
import QtQml.Models 2.2
import QtQuick.Controls.Material 2.2

import "../../actions"

Pane {
    id: root
    property var supportedModules
    property alias model: listView.model
    property bool configurationUpToDate: true

    padding: 12

    ListView {
        id: listView

        anchors.fill: parent
        spacing: 8

        // TODO: fix scrollbar hickups: https://forum.qt.io/topic/52484/problem-scrolling-listview-with-many-entries-of-different-height/2
        // ScrollBar.vertical: scrollbar

        delegate: Card {
            uid: model.uid
            displayName: model.displayName
            inputs: model.inputs
            parameters: model.parameters
            outputs: model.outputs
            width: parent.width
            expanded: true
            font.pointSize: 12
        }

        header: Column {
            anchors.horizontalCenter: parent.horizontalCenter
            RoundButton {
                text: "run"
                onClicked: {
                    AppActions.evaluatePlatform();
                }
                Material.background: configurationUpToDate ? Material.LightGreen : Material.Amber
            }

            Rectangle { color: "transparent"; height: 16; width: 1 }
        }

        footer: Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle { color: "transparent"; height: 16; width: 1 }

            RoundButton {
                text: "+"
                onClicked: {
                    menu.open();
                }

                Menu {
                    id: menu
                    width: 300
                    Menu {
                        id: defaultToolsMenu
                        title: "general"
                        MenuItem {
                            text: "import ics"
                            onTriggered: {
                                AppActions.importIcsFile({});
                            }
                        }
                    }
                    MenuSeparator {}

                    Instantiator {
                        model: supportedModules
                        Menu {
                            id: subMenu
                            title: displayName
                            width: 300
                            Component.onCompleted: {
                                // TODO: find a nicer QML-way to generate the submenu items
                                for (var i = 0; i < files.count; ++i) {
                                    insertItem(-1, Qt.createQmlObject('\
                                        import QtQuick 2.9; \
                                        import QtQuick.Controls 2.2; \
                                        import "../../actions"; \
                                        MenuItem { \
                                            text: "' + files.get(i).displayName + '"; \
                                            onTriggered: {AppActions.requestAddModule("' + files.get(i).path + '");} \
                                        }', subMenu));
                                }
                            }
                        }
                        onObjectAdded: menu.insertMenu(2, object)
                        onObjectRemoved: menu.removeMenu(object)
                    }

                    MenuSeparator {}
                    MenuItem {
                        text: "refresh module list"
                        onTriggered: {
                            AppActions.refreshModuleList();
                        }
                    }
                }
            }
        }
    }
}
