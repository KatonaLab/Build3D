import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

GroupBox {
    id: root
    property int uid
    property string moduleName
    property var controls
    
    width: parent.width

    Label {
        id: nameLabel
        text: root.moduleName
    }

    ColumnLayout {
        id: columnLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: nameLabel.bottom
        anchors.topMargin: 8
        spacing: 4

        Repeater {
            model: controls
            delegate: Loader {
                property var properties: model
                property int uid: root.uid
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

        Component {
            id: buttonDelegate
            Button {
                text: properties.name
            }
        }

        Component {
            id: editDelegate
            RowLayout {
                Text {
                    text: properties.name
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
                model: properties.options
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
                Text {
                    text: properties.name
                }
                Switch {
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }
}
