
import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

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
