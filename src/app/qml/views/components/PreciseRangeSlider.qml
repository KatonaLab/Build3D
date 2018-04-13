import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ColumnLayout {
    property int decimals: 4
    property alias font: lowField.font
    property alias from: slider.from
    property alias to: slider.to
    property real firstValue: slider.first.value
    property real secondValue: slider.second.value
    property int editWidth: 80
    property alias text: label.text

    Component.onCompleted: {
        slider.first.valueChanged(from);
        slider.second.valueChanged(to);
    }

    Label {
        id: label
        visible: text != ""
    }

    RangeSlider {
        id: slider
        Layout.fillWidth: true
        first.onValueChanged: {
            lowField.text = Number(first.value.toFixed(decimals)).toLocaleString();
        }
        second.onValueChanged: {
            highField.text = Number(second.value.toFixed(decimals)).toLocaleString();
        }
    }

    RowLayout {
        TextField {
            id: lowField
            horizontalAlignment: TextInput.AlignRight
            validator: DoubleValidator {}
            implicitWidth: editWidth
            onEditingFinished: {
                slider.first.value = Number.fromLocaleString(text);
            }
        }

        Rectangle {
            Layout.fillWidth: true
        }

        TextField {
            id: highField
            font: lowField.font
            horizontalAlignment: TextInput.AlignRight
            validator: DoubleValidator {}
            implicitWidth: editWidth
            onEditingFinished: {
                slider.second.value = Number.fromLocaleString(text);
            }
        }
    }
}

