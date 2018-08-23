import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    id: root
    property int decimals: 4
    property alias from: rangeSlider.from
    property alias to: rangeSlider.to
    property vector2d value: Qt.vector2d(0, 1)
    property int editWidth: 80
    property alias text: label.text

    Component.onCompleted: {
        rangeSlider.first.valueChanged(from);
        rangeSlider.second.valueChanged(to);
    }

    Label {
        id: label
        visible: text != ""
    }

    TextField {
        id: lowField
        horizontalAlignment: TextInput.AlignRight
        validator: DoubleValidator {}
        implicitWidth: editWidth
        onEditingFinished: {
            rangeSlider.first.value = Number.fromLocaleString(text);
        }
    }

    RangeSlider {
        id: rangeSlider
        Layout.fillWidth: true
        first.value: value.x
        second.value: value.y

        first.onValueChanged: {
            lowField.text = Number(first.value.toFixed(decimals)).toLocaleString();
            root.value = Qt.vector2d(rangeSlider.first.value, rangeSlider.second.value);
        }

        second.onValueChanged: {
            highField.text = Number(second.value.toFixed(decimals)).toLocaleString();
            root.value = Qt.vector2d(rangeSlider.first.value, rangeSlider.second.value);
        }
    }

    TextField {
        id: highField
        font: lowField.font
        horizontalAlignment: TextInput.AlignRight
        validator: DoubleValidator {}
        implicitWidth: editWidth
        onEditingFinished: {
            rangeSlider.second.value = Number.fromLocaleString(text);
        }
    }
}

