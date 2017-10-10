// import QtQuick 2.7
// import QtQuick.Layouts 1.1
// import QtQuick.Controls 1.2
import QtQuick.Scene3D 2.0
// import Qt3D.Render 2.0

import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.1
import QtQml.Models 2.1
// import Qt.labs.controls.material 1.0

ApplicationWindow {

    // Material.theme: Material.Dark
    // Material.theme: Material.Dark
    // Material.accent: Material.Purple

    id: window
    width: 1024
    height: 600
    visible: true
    title: "A3DC - KOKI MTA"

    // Component.onCompleted: {
    //     x = Screen.width / 2 - width / 2
    //     y = Screen.height / 2 - height / 2
    // }

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
            SceneRoot {}
        }

         ListView  {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.2

            model: ObjectModel {
                Repeater {
                    model: 30
                    CheckBox { text: "A" }
                }
            }
        }
    }

    // Rectangle {
    //     // width: 100
    //     // height: 100
    //     color: "steelblue"
    //     anchors.fill: parent

        // Scene3D {
        //     id: scene3d
        //     anchors.fill: parent
        //     // anchors.centerIn: parent

        //     focus: true
        //     aspects: ["input", "logic"]
        //     cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        //     SceneRoot {}
        // }
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