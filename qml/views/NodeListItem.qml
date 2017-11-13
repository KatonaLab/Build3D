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
    property string name

    // TODO: put these props into class like ViewProperties
    property alias visibleChecked: visibleCheck.checked
    property alias lowCutValue: slider.lowValue
    property alias highCutValue: slider.highValue
    property alias nodeColor: colorSelect.color

    function viewParameters() {
        return {visible: visibleCheck.checked,
                lowCut: slider.lowValue,
                highCut: slider.highValue,
                color: colorSelect.color};
    }

    QtObject {
        id: d
        readonly property bool showSettings: componentSource != null && componentSource != ""
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

                text: name                
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width - colorSelect.width
                anchors.margins: 8

                onClicked: {
                    AppActions.setNodeViewParameters(uid, viewParameters());
                }
            }

            ColorIndicator {
                id: colorSelect
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 8

                onColorChanged: {
                    AppActions.setNodeViewParameters(uid, viewParameters());
                }
            }

            DualSlider {
                id: slider
                anchors.left: parent.left
                anchors.top: visibleCheck.bottom
                anchors.topMargin: 8
                width: parent.width - 17*2
                anchors.margins: 8

                // lowValue
                onLowValueChanged: {
                    AppActions.setNodeViewParameters(uid, viewParameters());
                }
                onHighValueChanged: {
                    AppActions.setNodeViewParameters(uid, viewParameters());
                }
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
            visible: applyButton.visible && removeButton.visible

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
                    visible: removable
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