import QtQuick 2.9
import QtQuick.Window 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0
import koki.katonalab.a3dc 1.0
import QtQuick.Dialogs 1.3

import "../stores"
import "sidebar"
import "display"
import "components"

ApplicationWindow {
    id: appWindow
    font.pointSize: 11

    width: 1024
    height: 768
    title: "A3-DC - KatonaLab KOKI MTA (" + A3DCVersion.version() + ")"

    Material.theme: settings.darkTheme ? Material.Dark : Material.Light
    Material.accent: Material.Lime

    MenuBar {
        Menu {
            title: "File"
            Menu {
                id: barMenuNew
                title: newWorkflowAction.text
                onAboutToShow: ModuleStore.model.refreshAvailableWorkflows()

                Instantiator {
                    model: ModuleStore.model.availableWorkflows
                    MenuItem {
                        property url pathUrl: modelData.path
                        text: modelData.name
                        onTriggered: {
                            newWorkflowAction.startNewWorkflowProcess(modelData.path);
                        }
                    }
                    onObjectAdded: barMenuNew.insertItem(index, object)
                    onObjectRemoved: barMenuNew.removeItem(object)
                }
            }
            MenuItem {
                text: openWorkflowAction.text
                onTriggered: openWorkflowAction.startOpenWorkflowProcess()
            }
            MenuItem {
                text: saveWorkflowAction.text
                onTriggered: saveWorkflowAction.triggered()
            }
        }

        Menu {
            title: "View"
            MenuItem {
                text: toggleSmoothTexturesAction.text
                onTriggered: toggleSmoothTexturesAction.triggered()
            }
        }

        Menu {
            title: "Workflow"
            MenuItem {
                text: runAction.text
                onTriggered: runAction.triggered()
            }
            // MenuItem {
            //     text: runBatchAction.text
            //     onTriggered: runBatchAction.triggered()
            // }
            // MenuItem {
            //     text: stopAction.text
            //     onTriggered: saveWorkflowAction.triggered()
            // }
        }

        Menu {
            title: "Help"
            MenuItem {
                text: "Help"
                onTriggered: Qt.openUrlExternally("https://github.com/vivien-miczan/A3DC/wiki/Help")
            }
            MenuItem {
                text: "About"
                onTriggered: aboutWindow.show()
            }
        }
    }

    Window {
        id: aboutWindow
        title: "About"
        visible: false
        width: layout.width + 32
        height: layout.height + 32
        
        ColumnLayout {
            id: layout
            anchors.centerIn: parent
            spacing: 16
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: 16
                text: "A3-DC"
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "made by KatonaLab @ MTA KOKI"
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "<a href=\"https://github.com/vivien-miczan/A3DC\">https://github.com/vivien-miczan/A3DC</a>"
                onLinkActivated: Qt.openUrlExternally(link)
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: 11
                text: A3DCVersion.version()
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
        property url dialogFolder: ""
    }

    Component.onCompleted: {
        visible = true;
        ModuleStore.dialogFolder = settings.dialogFolder;
    }

    Component.onDestruction: {
        settings.splitterX = splitter.x;
        settings.consoleHeight = consolePanel.height;
        settings.dialogFolder = ModuleStore.dialogFolder;
    }

    Binding {
        target: ModuleStore.model
        property: "smoothTextures"
        value: settings.smoothTextures
    }

    header: ToolBar {
        font.pixelSize: 14
        Material.primary: parent.Material.background

        RowLayout {
            // anchors.fill: parent
            anchors.left: parent.left
            
            ToolButton {
                font.family: "fontello"
                text: "\uE800"
                onClicked: toolMenuNew.open()

                Menu {
                    id: toolMenuNew
                    title: newWorkflowAction.text
                    onAboutToShow: ModuleStore.model.refreshAvailableWorkflows()

                    Instantiator {
                        model: ModuleStore.model.availableWorkflows
                        MenuItem {
                            text: modelData.name
                            onTriggered: {
                                newWorkflowAction.startNewWorkflowProcess(modelData.path);
                            }
                        }
                        onObjectAdded: toolMenuNew.insertItem(index, object)
                        onObjectRemoved: toolMenuNew.removeItem(object)
                    }
                }
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

            // ToolButton {
            //     action: runBatchAction
            //     font.family: "fontello"
            //     text: "\uE80C"
            // }

            // ToolButton {
            //     action: stopAction
            //     font.family: "fontello"
            //     text: "\uE80A"
            // }
        }

        RowLayout {
            anchors.right: parent.right

            ToolButton {
                action: toggleSmoothTexturesAction
                font.family: "fontello"
                text: settings.smoothTextures ? "\uF1B3" : "\uE81B"
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

        Label {
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
                            // TODO: set proper size
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

    MessageDialog {
        id: unsavedWorkflowMessageBox
        title: "Unsaved Workflow"
        text: "All unsaved modifications will be lost. Are you sure to proceed?"
        standardButtons: StandardButton.Yes | StandardButton.No
    }

    Connections {
        property var nextAction
        id: messageBoxConnection
        target: unsavedWorkflowMessageBox
        onYes: function () {
            if (nextAction) {
                nextAction();
            }
        }
    }

    Action {
        id: newWorkflowAction
        text: "New Workflow"
        function startNewWorkflowProcess(path) {
            if (ModuleStore.model.unsaved) {
                messageBoxConnection.nextAction = function () {
                    ModuleStore.model.readWorkflow(path);
                }
                unsavedWorkflowMessageBox.open();
            } else {
                ModuleStore.model.readWorkflow(path);
            }
        }
    }

    Action {
        id: openWorkflowAction
        text: "Open Workflow"
        function startOpenWorkflowProcess() {
            if (ModuleStore.model.unsaved) {
                messageBoxConnection.nextAction = openDialog.open;
                unsavedWorkflowMessageBox.open();
            } else {
                openDialog.open();
            }
        }
        onTriggered: startOpenWorkflowProcess()
    }

    FileDialog {
        id: openDialog
        title: "Open Workflow"
        folder: ModuleStore.dialogFolder
        nameFilters: [ "A3-DC Workflow File (*.json)", "All files (*)" ]
        onAccepted: {
            ModuleStore.model.readWorkflow(openDialog.fileUrl);
            ModuleStore.dialogFolder = folder;
        }
        onRejected: {
            ModuleStore.dialogFolder = folder;
        }
    }

    Action {
        id: saveWorkflowAction
        text: "Save Workflow"
        onTriggered: saveDialog.open()
    }

    FileDialog {
        id: saveDialog
        title: "Save Workflow"
        folder: ModuleStore.dialogFolder
        defaultSuffix: "json"
        selectExisting: false
        selectFolder: false
        sidebarVisible: true
        onAccepted: {
            ModuleStore.model.writeWorkflow(saveDialog.fileUrl);
            ModuleStore.dialogFolder = folder;
        }
        onRejected: {
            ModuleStore.dialogFolder = folder;
        }
    }

    Action {
        id: runAction
        text: "Run"
        onTriggered: ModuleStore.model.evaluate(-1);
    }

    Action {
        id: runBatchAction
        text: "Run Batch"
        onTriggered: ModuleStore.model.evaluateBatch();
    }

    Action {
        id: stopAction
        text: "Stop"
        onTriggered: {
            // TODO:
        }
    }

    Action {
        id: toggleSmoothTexturesAction
        text: "Linear Texture Interpolation"
        onTriggered: {
            settings.smoothTextures = !settings.smoothTextures;
        }
    }
}