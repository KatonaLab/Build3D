import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2

import "../../actions"
import "../components"

Repeater {
    id: root
    property int uid: -1
    property font font

    delegate: Loader {
        property int uid: root.uid
        property var details: model
        Layout.fillWidth: true

        sourceComponent: {
            switch (model.type) {
                case "int": return intSliderDelegate;
                case "float": return floatSliderDelegate;
                case "bool": return switchDelegate;
                case "string": return details.hints.file === true ? filenameDelegate : stringDelegate;
                defualt: return unknownControllerDelegate;
            }
        }

        Component {
            id: unknownControllerDelegate
            Label {
                text: "invalid type for property '" + details.name + "'"
                font: root.font
                color: Material.color(Material.Red)
                // TODO: action
            }
        }

        Component {
            id: intSliderDelegate
            PreciseSlider {
                font: root.font
                stepSize: details.hints.stepSize || 1
                snapMode: Slider.SnapAlways
                from: details.hints.min || 0
                to: details.hints.max || 1000
                defaultValue: details.hints.default || from
                text: details.name
                onValueChanged: {
                    details.value = value;
                }
            }
        }

        Component {
            id: floatSliderDelegate
            PreciseSlider {
                font: root.font
                from: details.hints.min || 0.0
                to: details.hints.max || 1.0
                defaultValue: details.hints.default || from
                text: details.name
                onValueChanged: {
                    details.value = value;
                }
            }
        }

        Component {
            id: switchDelegate
            Switch {
                text: details.name
                font: root.font
                onCheckedChanged: {
                    details.value = checked;
                }
            }
        }

        Component {
            id: stringDelegate
            RowLayout {
                Label {
                    text: details.name
                    font: root.font
                }
                TextField {
                    font: root.font
                    Layout.fillWidth: true
                    onEditingFinished: {
                        details.value = text;
                    }
                }
            }
        }

        Component {
            id: filenameDelegate
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: details.name
                    font: root.font
                }
                Text {
                    id: filenameText
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                    font: root.font
                }
                Button {
                    Layout.fillWidth: true
                    text: "select file"
                    onClicked: {
                        dialog.open();
                    }

                    FileDialog {
                        id: dialog
                        title: "Select File"
                        selectMultiple: details.hints.multipleFiles || false
                        onAccepted: {
                            // https://stackoverflow.com/questions/24927850/get-the-path-from-a-qml-url
                            var path = dialog.fileUrl.toString();
                            console.log(path);
                            // remove prefixed "file:///"
                            path = path.replace(/^(file:\/{3})/,"");
                            console.log(path);
                            // unescape html codes like '%23' for '#'
                            var cleanPath = decodeURIComponent(path);
                            console.log(cleanPath);

                            console.debug("select file", cleanPath);
                            filenameText.text = cleanPath;
                            details.value = cleanPath
                        }
                    }
                }
            }
        }
    }
}
