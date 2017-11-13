import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0

import "controls"
import "../actions"

GroupBox {

    property int uid: -1
    property url componentSource: null
    property bool removable: false;
    property bool editable: true;

    QtObject {
        id: d
        readonly property bool showSettings: componentSource != null
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        GroupBox {
            id: viewSettings

            width: 0 // breaking a binding loop
            Layout.fillWidth: true

            // TODO: maybe put in a ColumnLayout
            CheckBox {
                id: visibleCheck
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width - colorSelect.width
                anchors.margins: 8
            }

            ColorIndicator {
                id: colorSelect
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 8
            }

            DualSlider {
                id: slider
                anchors.left: parent.left
                anchors.top: visibleCheck.bottom
                anchors.topMargin: 8
                width: parent.width - 17*2
                anchors.margins: 8
            }
        }

        GroupBox {
            id: nodeSettings
            Layout.fillWidth: true
            visible: d.showSettings
            clip: true
            Loader {
                source: componentSource
            }
        }

        GroupBox {
            id: nodeControls

            width: 0 // breaking a binding loop
            height: 0 // breaking a binding loop
            Layout.fillWidth: true
            // visible: applyButton.visible && removeButton.visible
            visible: true

            ColumnLayout {

                anchors.fill: parent

                Button {
                    id: applyButton
                    visible: d.showSettings
                    Layout.fillWidth: true
                    text: editable ? "Apply" : "Edit"
                    onClicked: {
                        editable = !editable;
                    }
                }

                Button {
                    id: removeButton
                    // visible: removable
                    visible: true
                    Layout.fillWidth: true
                    text: "Remove"
                    onClicked: {
                        AppActions.removeNode(uid);
                    }
                }
            }
        }
    }

    // TODO: make nice transitions

    states: [
        State {
            name: "Expanded"
            when: editable
            PropertyChanges {
                target: nodeSettings
                // TODO: it might not be the nicest way to do
                Layout.maximumHeight: nodeSettings.implicitHeight
                enabled: true
                opacity: 1.
            }
        },
        State {
            name: "Collapsed"
            when: !editable
            PropertyChanges {
                target: nodeSettings
                Layout.maximumHeight: 0
                enabled: false
                opacity: 0.
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation {
            target: nodeSettings
            properties: "Layout.maximumHeight, opacity"
            easing.type: Easing.InOutQuad
            duration: 200
        }
    }
}