import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
// import QtQuick.Controls.Material 2.1
// import QtQml.Models 2.1
import QtQuick.Scene3D 2.0
import QtQuick.Dialogs 1.2
// import QtGraphicalEffects 1.0

import Qt.labs.settings 1.0
import koki.katonalab.a3dc 1.0

ApplicationWindow {
    id: root
    visible: true

    width: 1024
    height: 600
    title: "A3DC - KatonaLab KOKI MTA"

    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    VolumetricDataManager {
        id: dataManager
        onSourceChanged: { console.log("src changed ", source); }
        onStatusChanged: { console.log("src changed ", status); }
    }

    Component.onCompleted: {
        dataManager.source = "/Users/fodorbalint/projects/a3dc/example/K32_bassoon_TH_vGluT1_c01_cmle.ics";
        console.log("len", dataManager.volumes.length);
    }

    FileDialog {
        id: openDialog
        // fileMode: FileDialog.OpenFile
        // selectedNameFilter.index: 1
        nameFilters: ["Image Cytometry Standard (*.ics *.ids)"]
        // folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            console.log("opened");
        }
    }

    header: ToolBar {
        leftPadding: 8

        Flow {
            id: flow
            width: parent.width

            Row {
                id: fileRow
                ToolButton {
                    id: openButton
                    text: "A" // icon-folder-open-empty
                    onClicked: openDialog.open()
                }
                ToolSeparator {
                    contentItem.visible: fileRow.y === editRow.y
                }
            }

            Row {
                id: editRow
                ToolButton {
                    id: copyButton
                    text: "B" // icon-docs
                }
                ToolButton {
                    id: cutButton
                    text: "C" // icon-scissors
                }
                ToolButton {
                    id: pasteButton
                    text: "D" // icon-paste
                }
                // ToolSeparator {
                    // contentItem.visible: editRow.y === formatRow.y
                // }
            }
        } // flow
    } // header toolbar

    RowLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Scene3D {
                id: scene3d
                
                anchors.fill: parent

                focus: true
                aspects: ["input", "logic"]
                cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
                Viewer {
                    id: sceneView
                }
            }

            Rectangle {
                id:sceneLoadingOverlay
                visible: false
                anchors.fill: parent
                color: "Black"
                opacity: 0.75
                
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 20
                    Text {
                        anchors.centerIn: parent
                        text: "\uE839"
                        font.family: "fontello"
                        font.pointSize: 22
                        color: "White"
                        NumberAnimation on rotation {
                            from: 0; to: 360;
                            running: sceneLoadingOverlay.visible == true; 
                            loops: Animation.Infinite
                            duration: 1200;
                        }
                    }
                    ProgressBar {
                        value: 0.3
                    }
                }
            }
        }

        ListModel {
            id: demoListModel
            ListElement {
                name: "Lorem"
            }
            ListElement {
                name: "ipsum"
            }
            ListElement {
                name: "dolorem"
            }
            ListElement {
                name: "sit"
            }
        }

        ColumnLayout {
            ListView  {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.2

                model: demoListModel
                delegate: CheckBox {
                    text: name
                }
            }

            CheckBox {
                text: "thresholding"
                onClicked: {
                    thresholdSlider.enabled = checked;
                    sceneView.thresholding = checked;
                }
            }

            Slider {
                id: thresholdSlider
                enabled: false
                from: 0.
                to: 10.
                onMoved: {
                    sceneView.threshold = value;
                }
            }
        }
    }

    // Rectangle {
    //     // width: 100
    //     // height: 100
    //     color: "steelblue"
    //     anchors.fill: parent

    //     Scene3D {
    //         id: scene3d
    //         anchors.fill: parent
    //         // anchors.centerIn: parent

    //         focus: true
    //         aspects: ["input", "logic"]
    //         cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

    //         Viewer {}
    //     }
    // }
}

// Rectangle {
//     id: root

//     // anchors.fill: parent

//     RowLayout {
//         anchors.fill: parent
//         anchors.margins: spacing
//         // Rectangle {
//             // color: "black"
//         // }

//         // Rectangle {
//             // color: "White"
//         // }
        
//         // Label {
//             // text: "ho"
//         // }

//         // Rectangle {
//         //     color: "steelblue"
//         //     height: 100
//         //     width: 100
//         // }
        
//         Rectangle {
//             // width: 100
//             // height: 100
//             color: "steelblue"
//             anchors.fill: parent

//             Scene3D {
//                 id: scene3d
//                 anchors.fill: parent
//                 // anchors.centerIn: parent

//                 focus: true
//                 aspects: ["input", "logic"]
//                 cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

//                 SceneRoot {}
//             }
//         }

//     }
//     // SideBar {}
// }