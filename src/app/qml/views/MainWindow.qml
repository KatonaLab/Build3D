import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0
import koki.katonalab.a3dc 1.0

import "../actions"
import "../stores"
import "sidebar"
import "display"
import "sidebar/controls"

ApplicationWindow {
    id: appWindow

    width: 480
    height: 480
    title: "A3DC - KatonaLab KOKI MTA (" + A3DCVersion.version() + ")"

    Component.onCompleted: visible = true

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem { action: importAction }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            FontelloButton {
                text: "\uE807"
                action: importAction
            }
        }
    }

    statusBar: Rectangle {
        // TODO
        height: 16
    }

    Settings {
        property alias x: appWindow.x
        property alias y: appWindow.y
        property alias width: appWindow.width
        property alias height: appWindow.height
    }

    Action {
        id: importAction
        text: "Import"
        onTriggered: {
            AppActions.importIcsFile({});
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
    
        CardPanel {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            model: MainStore.cardStore.model
            supportedModules: MainStore.moduleStore.supportedModules
        }

        Scene3D {
            Layout.fillWidth: true
            Layout.fillHeight: true
            // prevent errors when shrinked size 0
            Layout.minimumWidth: 128
            Layout.minimumHeight: 128

            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            SceneRootEntity {}
        }
    }

}