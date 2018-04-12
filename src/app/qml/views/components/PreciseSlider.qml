import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    property int decimals: 4
    property alias font: field.font
    property alias from: slider.from
    property alias to: slider.to
    property real value: slider.value
    property alias editWidth: field.implicitWidth

    Component.onCompleted: {
        slider.valueChanged(from);
    }

    Slider {
        Layout.fillWidth: true
        id: slider
        onValueChanged: {
            field.text = Number(value.toFixed(decimals)).toLocaleString();
        }
    }

    TextField {
        id: field
        horizontalAlignment: TextInput.AlignRight
        Layout.alignment: Qt.AlignRight
        validator: DoubleValidator {}
        onEditingFinished: {
            slider.value = Number.fromLocaleString(text);
        }
    }
}

