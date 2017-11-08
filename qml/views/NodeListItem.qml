import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0
import "ui-components"

GroupBox {
    id: root

    property Component nodeSettingsComponent
    property QtObject processNode
    property bool removable: false;
    property bool editable: true;

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
            visible: nodeSettingsComponent != undefined
            Loader {
                sourceComponent: nodeSettingsComponent
            }
        }

        GroupBox {
            width: 0 // breaking a binding loop
            Layout.fillWidth: true
            ColumnLayout {

                anchors.fill: parent
                
                Button {
                    visible: nodeSettingsComponent != undefined
                    Layout.fillWidth: true
                    text: root.editable ? "Apply" : "Edit"
                    onClicked: {
                        root.editable = !(root.editable);
                    }
                }

                Button {
                    visible: root.removable
                    Layout.fillWidth: true
                    text: "Remove"
                    onClicked: {
                        root.destroy();
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
                visible: true
            }
        },
        State {
            name: "Collapsed"
            when: !editable
            PropertyChanges {
                target: nodeSettings
                visible: false
            }
        }
    ]
}