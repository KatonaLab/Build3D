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
    property font font
    property int uid: -1

    delegate: Loader {
        property int uid: root.uid
        property var details: model
        Layout.fillWidth: true

        sourceComponent: {
            var typeToDelegate = {
                "int": {
                    "default": intOutputDelegate,
                },
                "float": {
                    "default": floatOutputDelegate,
                },
                "float-image": {
                    "default": floatImageOutputDelegate
                },
                "int-image": {
                    "default": intImageOutputDelegate
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
            id: intOutputDelegate
            Label {
                text: details.displayName + "(int)"
                font: root.font
            }
        }

        Component {
            id: floatOutputDelegate
            Label {
                text: details.displayName + "(float)"
                font: root.font
            }
        }

        Component {
            id: floatImageOutputDelegate
            ColumnLayout {
                id: floatImageOutput
                Layout.fillWidth: true

                function update() {
                    var values = {
                        firstValue: rangeSlider.firstValue,
                        secondValue: rangeSlider.secondValue,
                        color: colorSelector.color
                    };
                    AppActions.requestModulePropertyChange(uid, details.portId, values);
                }

                Label {
                    text: details.displayName
                }

                Rectangle {
                    id: colorSelector
                    width: 24
                    height: 24
                    color: "purple"
                    radius: 12
                    Layout.alignment: Qt.AlignRight
                    Material.elevation: 3
                    onColorChanged: floatImageOutput.update()
                    // TODO: change it to a color selector
                }

                PreciseRangeSlider {
                    id: rangeSlider
                    Layout.fillWidth: true
                    font: root.font
                    onFirstValueChanged: floatImageOutput.update()
                    onSecondValueChanged: floatImageOutput.update()
                }
            }
        }

        Component {
            id: intImageOutputDelegate
            Label {
                text: details.displayName
                font: root.font
            }
        }

        Component {
            id: unknownControllerDelegate
            Label {
                text: "invalid type for output '" + details.displayName + "'"
                font: root.font
                color: Material.color(Material.Red)
            }
        }
    }
}
