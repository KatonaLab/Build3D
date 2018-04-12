import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../../actions"
import "../components"

Pane {
    id: card
    property int uid
    property string displayName
    property ListModel inputs
    property ListModel parameters
    property ListModel outputs
    property int fontPointSize: 12

    Material.elevation: 8

    contentWidth: layout.implicitWidth
    contentHeight: header.implicitHeight

    states: State {
        name: "opened"
        when: header.checked
        PropertyChanges {
            target: card
            contentHeight: layout.implicitHeight
        }
    }

    transitions: Transition {
        from: ""; to: "opened"; reversible: true
        NumberAnimation {
            properties: "contentHeight"
            easing.type: Easing.InOutQuad
            duration: 250
        }
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        clip: true

        ArrowCheckBox {
            id: header
            text: "module name"
            Layout.fillWidth: true
            font.pointSize: fontPointSize + 2
        }

//        Rectangle {
//            Layout.fillWidth: true
//            height: 1
//            color: Material.color(Material.Grey)
//            anchors.topMargin: 8
//            anchors.bottomMargin: 8
//        }

        Repeater {
            id: inputsRepeater
            model: card.inputs
            delegate: ColumnLayout {
                Layout.fillWidth: true

                Label {
                    font.pointSize: fontPointSize - 2
                    text: model.displayName
                    Layout.fillWidth: true
                }

                ComboBox {
                    font.pointSize: fontPointSize
                    Layout.fillWidth: true
                    model: ["DataSource1/output", "threshold.1/output"]
                }
            }
        }

//        Rectangle {
//            Layout.fillWidth: true
//            height: 1
//            color: Material.color(Material.Grey)
//            anchors.topMargin: 16
//            anchors.bottomMargin: 16
//        }

        Repeater {
            id: parametersRepeater
            visible: false
            model: card.parameters
            delegate: Loader {
                property int uid: card.uid
                property var details: model
                Layout.fillWidth: true

                sourceComponent: {
                    switch(model.type) {
                        case "button": return buttonDelegate;
                        case "edit": return editDelegate;
                        case "combobox": return comboboxDelegate;
                        case "slider": return sliderDelegate;
                        case "range": return rangeDelegate;
                        case "switch": return switchDelegate;
                    }
                }
            }
        }

//        Rectangle {
//            Layout.fillWidth: true
//            height: 1
//            color: Material.color(Material.Grey)
//            anchors.topMargin: 16
//            anchors.bottomMargin: 16
//        }

        Repeater {
            id: outputsRepeater
            model: card.outputs
            delegate: Loader {
                property int uid: card.uid
                property var details: model

                sourceComponent: {
                    switch(model.type) {
                        case "volume": return volumeOutputDelegate;
                        case "?": return volumeOutputDelegate;
                    }
                }
            }
        }

        Component {
            id: volumeInputDelegate
            Label {
                text: details.displayName
                font.pointSize: fontPointSize
            }
        }

        Component {
            id: volumeOutputDelegate
            Label {
                text: details.displayName
                font.pointSize: fontPointSize
            }
        }

        Component {
            id: buttonDelegate
            Button {
                text: details.displayName
                font.pointSize: fontPointSize
            }
        }

        Component {
            id: editDelegate
            ColumnLayout {
                Label {
                    text: details.displayName
                    font.pointSize: fontPointSize
                }
                TextField {
                    font.pointSize: fontPointSize
                    Layout.fillWidth: true
                }
            }
        }

        Component {
            id: comboboxDelegate
            ComboBox {
                model: details.options
                font.pointSize: fontPointSize
            }
        }

        Component {
            id: sliderDelegate
            PreciseSlider {
//                font.pointSize: fontPointSize
            }
        }

        Component {
            id: rangeDelegate
            PreciseRangeSlider {}
        }

        Component {
            id: switchDelegate
            RowLayout {
                Label {
                    text: details.displayName
                    font.pointSize: fontPointSize
                }
                Switch {
                    font.pointSize: fontPointSize
                }
            }
        }
    }
//    contentWidth: layout.implicitWidth

//    ColumnLayout {
//        id: layout
//        anchors.fill: parent

//        Rectangle {
//            Layout.fillWidth: true
//            height: 10
//            color: "red"
//            Label {
//                text: displayName
//                anchors.left: parent.leftMargin
//                font.pointSize: fontPointSize
//            }

//            RoundButton {
//                text: "x"
//                font.pointSize: fontPointSize
//                Layout.alignment: Qt.AlignRight
//                onClicked: {
//                    AppActions.requestRemoveModule(card.uid);
//                }
//            }
//        }

//        Repeater {
//            model: card.inputs
//            delegate: RowLayout {
//                Label {
//                    font.pointSize: 12
////                    Layout.fillWidth: true
//                    text: model.displayName
//                }

//                ComboBox {
//                    font.pointSize: 12
//                    anchors.right: card.right
//                    flat: true
//                    model: ["DataSource1/output", "threshold.1/output"]
//                }
//            }
//        }

//        Repeater {
//            model: card.parameters
//            delegate: Loader {
//                property int uid: card.uid
//                property var details: model
////                Layout.fillWidth: true

//                sourceComponent: {
//                    switch(model.type) {
//                        case "button": return buttonDelegate;
//                        case "edit": return editDelegate;
//                        case "combobox": return comboboxDelegate;
//                        case "slider": return sliderDelegate;
//                        case "range": return rangeDelegate;
//                        case "switch": return switchDelegate;
//                    }
//                }
//            }
//        }


//        Repeater {
//            model: card.outputs
//            delegate: Loader {
//                property int uid: card.uid
//                property var details: model
////                Layout.fillWidth: true

//                sourceComponent: {
//                    switch(model.type) {
//                        case "volume": return volumeOutputDelegate;
//                        case "?": return volumeOutputDelegate;
//                    }
//                }
//            }
//        }

//    }
}
