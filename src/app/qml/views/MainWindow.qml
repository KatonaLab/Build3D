import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0
import koki.katonalab.a3dc 1.0

import "../stores"
import "sidebar"
import "display"
import "components"

ApplicationWindow {
    id: appWindow

    width: 480
    height: 480
    title: "A3-DC - KatonaLab KOKI MTA (" + A3DCVersion.version() + ")"

    Material.theme: settings.darkTheme ? Material.Dark : Material.Light
    Material.accent: Material.Lime

    Component.onCompleted: visible = true

    MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: newWorkflowAction.text
                onTriggered: newWorkflowAction.onTriggered();
            }
            MenuItem {
                text: openWorkflowAction.text
                onTriggered: openWorkflowAction.onTriggered();
            }
            MenuItem {
                text: saveWorkflowAction.text
                onTriggered: saveWorkflowAction.onTriggered();
            }
        }

        Menu {
            title: "View"
            MenuItem {
                text: toggleSmoothTexturesAction.text
                onTriggered: toggleSmoothTexturesAction.onTriggered();
            }
        }

        Menu {
            title: "Workflow"
            MenuItem {
                text: runAction.text
                onTriggered: runAction.onTriggered();
            }
            MenuItem {
                text: runBatchAction.text
                onTriggered: runBatchAction.onTriggered();
            }
            MenuItem {
                text: stopAction.text
                onTriggered: saveWorkflowAction.onTriggered();
            }
        }
    }

    Settings {
        id: settings
        property alias x: appWindow.x
        property alias y: appWindow.y
        property alias width: appWindow.width
        property alias height: appWindow.height
        property int splitterX: 400
        property int consoleHeight: 240
        property bool consoleStatus: false
        property bool darkTheme: false
        property bool smoothTextures: false
    }

    Component.onDestruction: {
        settings.splitterX = splitter.x;
        settings.consoleHeight = consolePanel.height;
    }

    header: ToolBar {
        // Material.foreground: "white"
        // Material.background: "white"
        Material.primary: parent.Material.background

        RowLayout {
            // anchors.fill: parent
            anchors.left: parent.left
            
            ToolButton {
                action: newWorkflowAction
                font.family: "fontello"
                text: "\uE800"
            }

            ToolSeparator {}

            ToolButton {
                action: openWorkflowAction
                font.family: "fontello"
                text: "\uF115"
            }

            ToolButton {
                action: saveWorkflowAction
                font.family: "fontello"
                text: "\uE80B"
            }

            ToolSeparator {}

            ToolButton {
                action: runAction
                font.family: "fontello"
                text: "\uE809"
            }

            ToolButton {
                action: runBatchAction
                font.family: "fontello"
                text: "\uE80C"
            }

            ToolButton {
                action: stopAction
                font.family: "fontello"
                text: "\uE80A"
            }

            // ToolSeparator {}
        }

        RowLayout {
            // anchors.fill: parent
            anchors.right: parent.right

            ToolButton {
                action: toggleSmoothTexturesAction
                font.family: "fontello"
                text: settings.smoothTextures ? "\uF1B3" : "\uE81B"
                onClicked: {
                    settings.smoothTextures = !settings.smoothTextures;
                }
            }

            ToolButton {
                id: themeButton
                font.family: "fontello"
                text: settings.darkTheme ? "\uF10C" : "\uF111"
                onClicked: {
                    settings.darkTheme = !settings.darkTheme;
                }
            }            
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        CardPanel {
            id: cardPanel
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            baseModel: ModuleStore.model
        }

        Text {
            id: splitter
            text: "⋮"
            Layout.alignment: Qt.AlignVCenter

            x: settings.splitterX

            onXChanged: {
                cardPanel.Layout.preferredWidth = splitter.x;
            }

            MouseArea {
                anchors.fill: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 150
                drag.maximumX: appWindow.width - displayItem.Layout.minimumWidth
                drag.target: splitter
            }
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
                    baseModel: ModuleStore.model
                    viewPortSize: Qt.size(displayItem.width, displayItem.height)
                }
            }

            Rectangle {
                id: consolePanel

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: settings.consoleHeight
                color: "#1f000000"

                ColumnLayout {
                    anchors.fill: parent

                    Text {
                        id: consoleShowHide
                        Layout.preferredHeight: height
                        Layout.fillWidth: true
                        // TODO: nice arrows to indicate show/hide direction
                        text: "…"
                        horizontalAlignment: Text.AlignHCenter
                        // TODO: text color match to the theme
                        color: "white"

                        onYChanged: {
                            consolePanel.height -= y;
                        }

                        MouseArea {
                            anchors.fill: parent
                            drag.axis: Drag.YAxis
                            drag.minimumY: consolePanel.height - displayItem.height
                            drag.maximumY: consolePanel.height - consoleShowHide.height
                            drag.target: consoleShowHide
                        }
                    }

                    Flickable {
                        
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        clip: true
                        contentHeight: textArea.height

                        onContentHeightChanged: {
                            contentY = Math.max(contentHeight - height, 0);
                        }

                        TextArea {
                            id: textArea
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
                            width: parent.width - 16
                            x: 8
                        }

                        ScrollIndicator.vertical: ScrollIndicator {}
                    }

                } // row layout

            }

        }
    }

    Action {
        id: newWorkflowAction
        text: "New Workflow"
        onTriggered: {
            // TODO:
            // AppActions.importIcsFile({});
        }
    }

    Action {
        id: openWorkflowAction
        text: "Open Workflow"
        onTriggered: {
            // TODO:
            // AppActions.readJson({});
        }
    }

    Action {
        id: saveWorkflowAction
        text: "Save Workflow"
        onTriggered: {
            // TODO:
            // AppActions.writeJson({});
        }
    }

    Action {
        id: runAction
        text: "Run"
        onTriggered: {
            ModuleStore.model.evaluate(-1);
        }
    }

    Action {
        id: runBatchAction
        text: "Run Batch"
        onTriggered: {
            // TODO:
            // AppActions.writeJson({});
        }
    }

    Action {
        id: stopAction
        text: "Stop"
        onTriggered: {
            // TODO:
            // AppActions.writeJson({});
        }
    }

    Action {
        id: toggleSmoothTexturesAction
        text: "Linear Texture Interpolation"
        onTriggered: {
            // TODO:
            // AppActions.writeJson({});
        }
    }
}