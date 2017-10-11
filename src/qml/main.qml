import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.1
import QtQml.Models 2.1
import QtQuick.Scene3D 2.0
import QtQuick.Dialogs 1.2

ApplicationWindow {

    id: root
    width: 1024
    height: 600
    visible: true
    title: "A3DC - KOKI MTA"

    Component.onCompleted: {
        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
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
                ToolSeparator {
                    contentItem.visible: editRow.y === formatRow.y
                }
            }
        } // flow
    } // header toolbar

    RowLayout {
        anchors.fill: parent

        Scene3D {
            id: scene3d
            
            Layout.fillHeight: true
            Layout.fillWidth: true

            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
            Viewer {
                id: sceneView
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