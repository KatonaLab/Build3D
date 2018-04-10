import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

import "../../actions"

GroupBox {
    id: root
    property int uid
    property string displayName
    property var inputs
    property var parameters
    property var outputs
    
    width: parent.width

    Label {
        id: nameLabel
        text: displayName
    }

    ColumnLayout {
        id: columnLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: nameLabel.bottom
        anchors.topMargin: 8
        spacing: 4

        Rectangle {
            color: "lightgray"
            height: 1
            Layout.fillWidth: true
        }

        Repeater {
            model: root.inputs
            delegate: Loader {
                property int uid: root.uid
                property var details: model
                Layout.fillWidth: true

                sourceComponent: {
                    switch(model.type) {
                        case "volume": return volumeInputDelegate;
                        case "?": return volumeInputDelegate;
                    }
                }
            }
        }

        Rectangle {
            color: "lightgray"
            height: 1
            Layout.fillWidth: true
        }

        Repeater {
            model: root.parameters
            delegate: Loader {
                property int uid: root.uid
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
    
        Rectangle {
            color: "lightgray"
            height: 1
            Layout.fillWidth: true
        }

        Repeater {
            model: root.outputs
            delegate: Loader {
                property int uid: root.uid
                property var details: model
                Layout.fillWidth: true

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
            }
        }

        Component {
            id: volumeOutputDelegate
            Label {
                text: details.displayName
            }
        }

        Component {
            id: buttonDelegate
            Button {
                text: details.displayName
            }
        }

        Component {
            id: editDelegate
            RowLayout {
                Label {
                    text: details.displayName
                }
                TextField {
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: 80
                }
            }
        }

        Component {
            id: comboboxDelegate
            ComboBox {
                model: details.options
            }
        }

        Component {
            id: sliderDelegate
            ColumnLayout {
                Slider {
                    tickmarksEnabled: true
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    Rectangle {
                        Layout.fillWidth: true
                    }
                    SpinBox {
                        Layout.preferredWidth: 80
                        Layout.alignment: Qt.AlignRight
                    }
                }
            }
        }

        Component {
            id: rangeDelegate
            ColumnLayout {
                Slider {
                    tickmarksEnabled: true
                    Layout.fillWidth: true
                }
                Slider {
                    tickmarksEnabled: true
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    SpinBox {
                        Layout.preferredWidth: 80
                    }
                    Rectangle {
                        Layout.fillWidth: true
                    }
                    SpinBox {
                        Layout.preferredWidth: 80
                        Layout.alignment: Qt.AlignRight
                    }
                }
            }
        }

        Component {
            id: switchDelegate
            RowLayout {
                Label {
                    text: details.displayName
                }
                Switch {
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }
}
