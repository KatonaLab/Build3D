import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

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
            var typeToDelegate = {
                "int": {
                    "default": intSliderDelegate,
                    "slider": intSliderDelegate
                },
                "float": {
                    "default": floatSliderDelegate,
                    "slider": floatSliderDelegate
                },
                "bool": {
                    "default": switchDelegate
                }
            };

            var type = model.type || "unknown";
            var hint = model.hint || "default";
            if (typeToDelegate[type]) {
                if (typeToDelegate[type][hint]) {
                    return typeToDelegate[type][hint];
                }
            }
            return unknownControllerDelegate;
        }

        Component {
            id: buttonDelegate
            Button {
                text: details.displayName
                font: root.font
                // TODO: action
            }
        }

        Component {
            id: unknownControllerDelegate
            Label {
                text: "invalid type for property '" + details.displayName + "'"
                font: root.font
                color: Material.color(Material.Red)
                // TODO: action
            }
        }

        Component {
            id: editDelegate
            ColumnLayout {
                Label {
                    text: details.displayName
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
                stepSize: 1
                snapMode: Slider.SnapAlways
                from: details.from || 0
                to: details.to || 1000
                text: details.displayName

                onValueChanged: {
                    var values = {value: value};
                    AppActions.requestModuleParamChange(uid, details.portId, values);
                }
            }
        }

        Component {
            id: floatSliderDelegate
            PreciseSlider {
                font: root.font
                from: details.from || 0
                to: details.to || 1
                text: details.displayName

                onValueChanged: {
                    var values = {value: value};
                    AppActions.requestModuleParamChange(uid, details.portId, values);
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
                text: details.displayName
                font: root.font
                onCheckedChanged: {
                    var values = {value: checked};
                    AppActions.requestModuleParamChange(uid, details.portId, values);
                }
            }
        }
    }
}
