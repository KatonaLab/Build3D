import QtQuick 2.8
import QtQuick.Window 2.0
// import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

import "../../actions"

ScrollView {
    id: root
    property ListModel model
    property ListModel supportedModules

    anchors.margins: 16

    ListView {
        id: listView
        model: root.model
        spacing: 16
        delegate: Card {
            uid: model.uid
            displayName: model.displayName
            inputs: model.inputs
            parameters: model.parameters
            outputs: model.outputs
        }

        footer: RoundButton {
            text: "+"
            // width: parent.width
            
            // anchors.centerIn: parent

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
                            AppActions.requestAddModule(model.scriptPath);
                        }
                    }
                    onObjectAdded: menu.insertItem(index, object)
                    onObjectRemoved: menu.removeItem(object)
                }
            }
        }
    }
}
