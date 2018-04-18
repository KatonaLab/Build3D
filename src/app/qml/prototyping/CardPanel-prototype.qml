
import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import "../views/sidebar"
import "CardPanel-sample.js" as SampleData

Window {
    visible: true
    width: 1024
    height: 768
    title: qsTr("CardPanel Prototype")

    Material.theme: Material.Light
    Material.accent: Material.Teal

    ListModel {
        id: model
        Component.onCompleted: {
            model.append(SampleData.demoModules);
        }
    }

    ListModel {
        id: supportedModules
        ListElement {
            displayName: "DataSource"
            type: "source"
        }
        ListElement {
            displayName: "Threshold"
            type: "threshold"
        }
        ListElement {
            displayName: "Magic"
            type: "magic"
        }
    }

    RowLayout {
        anchors.fill: parent
        ColumnLayout {
            Layout.fillHeight: true
            width: 250
            Button {
                text: "add item to type1 model"
                onClicked: {
                    SampleData.type1PortModel.push({"displayName": "new item", "targetUid": 7, "targetOutput": 1});
                    console.log(JSON.stringify(SampleData.type1PortModel));
                }
            }
            Button {
                text: "remove item to type1 model"
                onClicked: {
                    SampleData.type1PortModel.splice(0);
                }
            }
            Button {
                text: "add item to type2 model"
                onClicked: {
                    SampleData.type2PortModel.push({"displayName": "new item", "targetUid": 7, "targetOutput": 1});
                }
            }
            Button {
                text: "remove item to type2 model"
                onClicked: {
                    SampleData.type2PortModel.splice(0);
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            CardPanel {
                model: model
                supportedModules: supportedModules
                width: 300
                height: 500
                clip: true
                y: 100
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
