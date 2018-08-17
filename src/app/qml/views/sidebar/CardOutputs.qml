import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0

import "../../actions"
import "../components"

Repeater {
    id: root
    property font font
    // TODO: not sure this is neccessary
    property int uid: -1

    delegate: Loader {
        property int uid: root.uid
        property var details: model
        Layout.fillWidth: true

        sourceComponent: {
            switch (model.type) {
                case "int-image": return floatImageOutputDelegate;
                case "float-image": return floatImageOutputDelegate;
                case "int":
                case "float":
                case "bool":
                case "py-object": return recognizedOutputDelegate;
                defualt: return unknownControllerDelegate;
            }
        }

        Component {
            id: recognizedOutputDelegate
            Label {
                text: details.name + "(" + details.type + ")"
                font: root.font
            }
        }

        Component {
            id: floatImageOutputDelegate
            ColumnLayout {
                id: floatImageOutput
                Layout.fillWidth: true

                RowLayout {
                    Layout.fillWidth: true

                    Switch {
                        id: visibilitySwitch
                        Layout.fillWidth: true
                        text: details.name
                        font: root.font
                        checked: details.value.visible
                        Binding {
                            target: details.value
                            property: "visible"
                            value: visibilitySwitch.checked
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignRight
                        width: colorSelector.width + 9
                        height: colorSelector.height + 9
                        
                        ColorIndicator {
                            id: colorSelector
                            color: details.value.color
                            Binding {
                                target: details.value
                                property: "color"
                                value: colorSelector.color
                            }
                        }

                        DropShadow {
                            anchors.fill: parent
                            horizontalOffset: 0
                            verticalOffset: 0
                            radius: 9
                            samples: 19
                            color: "#40000000"
                            source: colorSelector
                        }
                    }
                }

                PreciseRangeSlider {
                    id: rangeSlider
                    Layout.fillWidth: true
                    font: root.font
                    // onFirstValueChanged: floatImageOutput.update()
                    // onSecondValueChanged: floatImageOutput.update()
                }
            }
        }

        Component {
            id: unknownControllerDelegate
            Label {
                text: "invalid type for output '" + details.name + "'"
                font: root.font
                color: Material.color(Material.Red)
            }
        }
    }
}
