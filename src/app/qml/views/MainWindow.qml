import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0
import koki.katonalab.a3dc 1.0

import "../actions"
import "../stores"
import "sidebar"
import "display"
import "components"

ApplicationWindow {
    id: appWindow

    width: 480
    height: 480
    title: "A3-DC - KatonaLab KOKI MTA (" + A3DCVersion.version() + ")"

    Material.theme: Material.Light
    Material.accent: Material.Teal

    Component.onCompleted: visible = true

    MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: importAction.text
                onTriggered: importAction.onTriggered();
            }
            MenuItem {
                text: readJsonAction.text
                onTriggered: readJsonAction.onTriggered();
            }
            MenuItem {
                text: writeJsonAction.text
                onTriggered: writeJsonAction.onTriggered();
            }
        }
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

    Action {
        id: readJsonAction
        text: "Open Workflow"
        onTriggered: {
            AppActions.readJson({});
        }
    }

    Action {
        id: writeJsonAction
        text: "Save Workflow"
        onTriggered: {
            AppActions.writeJson({});
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        CardPanel {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            model: MainStore.cardStore.model
            supportedModules: MainStore.moduleStore.supportedModules
            configurationUpToDate: MainStore.moduleStore.modelUpToDate
        }

        Item {
            id: displayItem
            Layout.fillWidth: true
            Layout.fillHeight: true
            // prevent errors when shrinked size 0
            Layout.minimumWidth: 128
            Layout.minimumHeight: 128

            Scene3D {
                id: sceneView
                anchors.fill: parent
                aspects: ["input", "logic"]
                cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
                hoverEnabled: false
                SceneRootEntity {
                    viewPortSize: Qt.size(displayItem.width, displayItem.height)
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 160
                color: "#1f000000"

                ScrollView {
                    id: textAreaScroll
                    anchors.fill: parent
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    clip: true
                    Flickable {
                        flickableDirection: Flickable.VerticalFlick
                        contentHeight: textArea.height
                        contentY: contentHeight - height
                        
                        TextArea.flickable: TextArea {
                            id : textArea
                            font.pointSize: 11
                            font.family: "Courier"
                            text: LogCollector.unfilteredLog
                            color: Material.accent
                            textFormat: TextEdit.RichText
                            readOnly: true
                            selectByMouse: true
                            selectByKeyboard: true
                            wrapMode: TextEdit.WrapAnywhere
                            background: Item {}
                        }
                    }
                }
            }

        }
    }
}