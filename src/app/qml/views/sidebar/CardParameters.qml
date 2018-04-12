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
            switch(model.type) {
                case "button": return buttonDelegate;
                case "edit": return editDelegate;
                case "combobox": return comboboxDelegate;
                case "slider": return sliderDelegate;
                case "range": return rangeDelegate;
                case "switch": return switchDelegate;
            }
        }

        Component {
            id: buttonDelegate
            Button {
                text: details.displayName
                font: root.font
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
            }
        }

        Component {
            id: comboboxDelegate
            ComboBox {
                model: details.options
                font: root.font
            }
        }

        Component {
            id: sliderDelegate
            PreciseSlider {
                font: root.font
            }
        }

        Component {
            id: rangeDelegate
            PreciseRangeSlider {
                font: root.font
            }
        }

        Component {
            id: switchDelegate
            Switch {
                text: details.displayName
                font: root.font
            }
        }
    }
}
