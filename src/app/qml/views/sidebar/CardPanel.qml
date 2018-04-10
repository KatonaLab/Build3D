import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

import "../../actions"

ScrollView {
    id: root
    property ListModel model
    property ListModel supportedModules

    ListView {
        id: listView
        model: root.model
        spacing: 4
        delegate: Card {
            uid: model.uid
            displayName: model.displayName
            inputs: model.inputs
            parameters: model.parameters
            outputs: model.outputs
        }

        footer: Button {
            text: "+"
            width: parent.width

            onClicked: {
                menu.popup();
            }

            Menu {
                id: menu
                Instantiator {
                    model: root.supportedModules
                    MenuItem {
                        text: model.displayName
                        onTriggered: {
                            console.log("triggered", model.script);
                            AppActions.requestAddModule(model.script);
                        }
                    }
                    onObjectAdded: menu.insertItem(index, object)
                    onObjectRemoved: menu.removeItem(object)
                }
            }
        }
    }
}