import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml 2.2

import "../../actions"

Pane {
    property ListModel supportedModules
    property alias model: listView.model

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
                    Instantiator {
                        model: supportedModules
                        MenuItem {
                            text: model.displayName
                            onTriggered: {
                                AppActions.requestAddModule(model.scriptPath);
                            }
                        }
                        onObjectAdded: menu.insertItem(index, object)
                        onObjectRemoved: menu.removeItem(object)
                    }
                    MenuItem {
                        text: "import ics"
                        onTriggered: {
                            AppActions.importIcsFile({});
                        }
                    }
                }
            }
        }
    }
}
