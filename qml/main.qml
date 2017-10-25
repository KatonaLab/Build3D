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

    function buildGuiForVolumeData(manager)
    {
        channelPanel.clearControls();
        sceneEntity.clearCubes();
        for (var i = 0; i < manager.volumes.length; ++i) {
            var cube = sceneEntity.createCube(manager.volumes[i]);
            channelPanel.createViewControl(manager.volumes[i], cube);
        }
    }

    Component.onCompleted: {
        // NOTE: for test purposes
        dataManager.source = "file:///Users/fodorbalint/Desktop/K32_bassoon_TH_vGluT1_c01_cmle.ics";
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

    VolumetricDataManager {
        id: dataManager
        onStatusChanged: { 
            switch (status) {
                case Component.Ready:
                    loadIndicator.visible = false;
                    buildGuiForVolumeData(dataManager);
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
    
        ChannelViewPanel {
            id: channelPanel
            Layout.preferredWidth: 250
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