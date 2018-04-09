import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "controls"

GroupBox {
    id: root
    property ListModel model
    
    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            width: parent.width
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 4
                Repeater {
                    anchors.fill: parent
                    model: root.model
                    delegate: Card {
                        title: model.title
                    }
                }
            }

            FontelloButton {
                text: "\uE827"
                Layout.fillWidth: true
                onClicked: {
                    menu.popup();
                }
                Menu {
                    id: menu
                }
            }
        }
    }
}
