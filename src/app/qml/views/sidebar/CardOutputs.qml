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

    // TODO: move to a shared utility js file
    function modelListHasItem(modelList, item) {
        if (!modelList) {
            return false;
        }
        for (var i = 0; i < modelList.count; ++i) {
            if (modelList.get(i)[item] === true) {
                return true;
            }
        }
        return false;
    }

    delegate: Loader {
        property int uid: root.uid
        property var details: model
        Layout.fillWidth: true

        sourceComponent: {
            if (modelListHasItem(model.typeTraits, "int-like")) {
                return intOutputDelegate;
            }

            if (modelListHasItem(model.typeTraits, "float-like")) {
                return floatOutputDelegate;
            }

            if (modelListHasItem(model.typeTraits, "float-image")) {
                return floatImageOutputDelegate;
            }

            if (modelListHasItem(model.typeTraits, "int-image")) {
                return intImageOutputDelegate;
            }

            if (modelListHasItem(model.typeTraits, "py-object")) {
                return pyObjectOutputDelegate;
            }

            return unknownControllerDelegate;
        }

        Component {
            id: pyObjectOutputDelegate
            Label {
                text: details.name + "(py object)"
                font: root.font
            }
        }

        Component {
            id: intOutputDelegate
            Label {
                text: details.name + "(int)"
                font: root.font
            }
        }

        Component {
            id: floatOutputDelegate
            Label {
                text: details.name + "(float)"
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
                        color: colorSelector.color,
                        visible: visibilitySwitch.checked,
                        labeled: false
                    };
                    AppActions.requestModuleOutputChange(uid, details.uid, values);
                }

                RowLayout {
                    Layout.fillWidth: true

                    Switch {
                        id: visibilitySwitch
                        Layout.fillWidth: true
                        text: details.name
                        font: root.font
                        onCheckedChanged: {
                            floatImageOutput.update();
                        }
                    }

                    ColorIndicator {
                        id: colorSelector
                        // TODO:
                        // color: details.color
                        Layout.alignment: Qt.AlignRight
                        onColorChanged: floatImageOutput.update()
                    }
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
            ColumnLayout {
                id: intImageOutput
                Layout.fillWidth: true

                function update() {
                    var values = {
                        firstValue: 0.,
                        secondValue: 1.,
                        color: Qt.rgba(1., 1., 1., 1.),
                        visible: intVisibilitySwitch.checked,
                        labeled: true
                    };
                    AppActions.requestModuleOutputChange(uid, details.uid, values);
                }

                RowLayout {
                    Layout.fillWidth: true

                    Switch {
                        id: intVisibilitySwitch
                        Layout.fillWidth: true
                        text: details.name
                        font: root.font
                        onCheckedChanged: {
                            intImageOutput.update();
                        }
                    }
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
