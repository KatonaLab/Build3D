import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import QtQuick.Scene3D 2.0
import QtQuick.Dialogs 1.2
import koki.katonalab.a3dc 1.0

ApplicationWindow {
    id: root

    visible: true
    width: 1024
    height: 600
    title: "A3DC - KatonaLab KOKI MTA"

    menuBar: MenuBar {
        // TODO
    }

    toolBar: ToolBar {
        // TODO
    }

    statusBar: Rectangle {
        // TODO
        height: 16
    }

    Component.onCompleted: {
        dataManager.source = "file:///Users/fodorbalint/Desktop/K32_bassoon_TH_vGluT1_c01_cmle.ics";
        console.log("len", dataManager.volumes.length);
        console.log("len", dataManager.source);
    }

    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    SystemPalette {
        id: sysColors
        colorGroup: SystemPalette.Active
    }

    VolumetricDataManager {
        id: dataManager
        onStatusChanged: { 
            switch (status) {
                case Component.Ready:
                    loadIndicator.visible = true;
                    break;
                case Component.Loading:
                    loadIndicator.visible = true;
                    break;
                case Component.Error:
                    // TODO: pop up error
                    console.log("error in loading")
                    break;
            }
        }
        onProgressChanged: {
            loadIndicator.progress = progress;
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
    
        Rectangle {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            color: sysColors.dark
            border.color: sysColors.mid
            border.width: 1

            ScrollView {
                anchors.fill: parent
                ColumnLayout {
                    spacing: 8
                    
                }
            }
        }

        Scene3D {
            Layout.fillWidth: true
            Layout.fillHeight: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
            SceneModel {
                id: sceneModel
            }
        }

        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            color: sysColors.dark
            border.color: sysColors.mid
            border.width: 1
        }
    }

    Window {
        width: 100
        height: 200
    }

    Label {
        id: loadIndicator
        
        property real progress

        // TODO: text for loading
        anchors.centerIn: parent
        text: "loading " + progress*100 + "%"
        color: "white"
    }
}