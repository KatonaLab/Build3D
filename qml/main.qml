import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import QtQuick.Scene3D 2.0
import QtQuick.Dialogs 1.2
import koki.katonalab.a3dc 1.0

import "scene"
import "views"
import "nodes"
import "views/ui-components"

ApplicationWindow {
    id: root

    visible: true
    width: 1024
    height: 600
    title: "A3DC - KatonaLab KOKI MTA"

    function buildGuiForVolumeData(manager)
    {
        sideBar.clearControls();
        for (var i = 0; i < manager.volumes.length; ++i) {
            var v = manager.volumes[i];
            var d = Math.max(v.width, v.height, v.depth);
            sceneEntity.volumeCube.size = Qt.vector3d(v.width / d, v.height / d, v.depth / d);

            var cp;
            switch (i) {
                case 0:
                    cp = sceneEntity.volumeCube.volumeParameter0;
                    break;
                case 1:
                    cp = sceneEntity.volumeCube.volumeParameter1;
                    break;
                case 2:
                    cp = sceneEntity.volumeCube.volumeParameter2;
                    break;
                default:
                    cp = sceneEntity.volumeCube.volumeParameter3;
                    break;
            }
            cp.texture.data = manager.volumes[i];
            sideBar.createViewControl(cp);
        }
    }

    Component.onCompleted: {
        // NOTE: for test purposes
        // K32_bassoon_TH_vGluT1_c01_cmle
        dataManager.source = "file:///Users/fodorbalint/Desktop/K32_bassoon_TH_vGluT1_c01_cmle.ics";
        // dataManager.source = "file:///Users/fodorbalint/Desktop/spheres.ics";
    }

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

    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    NodeManager {
        id: nodeManager
    }

    VolumetricDataManager {
        id: dataManager
        onStatusChanged: { 
            switch (status) {
                case Component.Ready:
                    loadIndicator.visible = false;
                    buildGuiForVolumeData(dataManager);
                    console.log(sceneEntity);
                    break;
                case Component.Loading:
                    loadIndicator.visible = true;
                    break;
                case Component.Error:
                    // TODO: pop up error
                    // and reset the changes
                    console.log("error in loading")
                    break;
            }
        }
        onProgressChanged: {
            // TODO: progress to bg thread in the cpp impl.
            loadIndicator.progress = progress;
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
    
        NodeListPanel {
            id: sideBar
            nodeManager: nodeManager
            Layout.preferredWidth: 240
            Layout.fillHeight: true
        }

        Scene3D {
            Layout.fillWidth: true
            Layout.fillHeight: true
            // prevent errors when shrinked size 0
            Layout.minimumWidth: 128
            Layout.minimumHeight: 128

            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
            SceneEntity {
                id: sceneEntity
                nodeManager: nodeManager
            }
        }

        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            // TODO: activate
            visible: false
        }
    }

    LoadIndicator {
        id: loadIndicator
    }
}