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
                // case "string": 
                defualt: return unknownControllerDelegate;
            }

            // if (modelListHasItem(details.typeTraits, "string")) {
                // if (details.hints.file === true) {
                    // return filenameDelegate;
                // } else {
                    // return stringDelegate;
                // }
            // }

            // return unknownControllerDelegate;
        }

        Component {
            id: buttonDelegate
            Button {
                text: details.name
                font: root.font
                // TODO: action
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
            id: editDelegate
            ColumnLayout {
                Label {
                    text: details.name
                    font: root.font
                }
                TextField {
                    font: root.font
                    Layout.fillWidth: true
                }
                // TODO: action
            }
        }

        Component {
            id: comboboxDelegate
            ComboBox {
                model: details.options
                font: root.font
            }
            // TODO: action
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
                    var values = {value: value};
                    AppActions.requestModuleParamChange(uid, details.uid, values);
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
                    var values = {value: value};
                    AppActions.requestModuleParamChange(uid, details.uid, values);
                }
            }
        }

        Component {
            id: rangeDelegate
            PreciseRangeSlider {
                font: root.font
                // TODO: action
                onFirstValueChanged: {
                    console.warn("not implemented CardParameters/PreciseRangeSlider")
                }
                onSecondValueChanged: {
                    console.warn("not implemented CardParameters/PreciseRangeSlider")
                }
            }
        }

        Component {
            id: switchDelegate
            Switch {
                text: details.name
                font: root.font
                onCheckedChanged: {
                    var values = {value: checked};
                    AppActions.requestModuleParamChange(uid, details.uid, values);
                }
            }
        }

        Component {
            id: stringDelegate
            ColumnLayout {
                Label {
                    text: details.name
                    font: root.font
                }
                TextField {
                    font: root.font
                    Layout.fillWidth: true
                    onEditingFinished: {
                        var values = {value: text};
                        AppActions.requestModuleParamChange(uid, details.uid, values);
                    }
                }
            }
        }

        Component {
            id: filenameDelegate
            ColumnLayout {
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
                            var values = {value: cleanPath};
                            AppActions.requestModuleParamChange(uid, details.uid, values);
                        }
                    }
                }
            }
        }
    }
}
