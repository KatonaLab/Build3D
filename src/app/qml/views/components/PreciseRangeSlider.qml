import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    id: root
    property int decimals: 4
    property alias from: rangeSlider.from
    property alias to: rangeSlider.to
    property real firstValue
    property real secondValue
    property int editWidth: 80
    property alias text: label.text

    signal rangeChanged(real x, real y)

    Label {
        id: label
        visible: text != ""
    }

    TextField {
        id: lowField
        text: { return Number(firstValue.toFixed(decimals)).toLocaleString(); }
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
        first.value: firstValue
        second.value: secondValue

        function update() {
            // lowField.text = Number(first.value.toFixed(decimals)).toLocaleString();
            // highField.text = Number(second.value.toFixed(decimals)).toLocaleString();
            if (first.value != firstValue || second.value != secondValue) {
                rangeChanged(first.value, second.value);
            }
        }

        Component.onCompleted: update()
        first.onValueChanged: update()
        second.onValueChanged: update()
    }

    TextField {
        id: highField
        text: { return Number(secondValue.toFixed(decimals)).toLocaleString(); }
        horizontalAlignment: TextInput.AlignRight
        validator: DoubleValidator {}
        implicitWidth: editWidth
        onEditingFinished: {
            rangeSlider.second.value = Number.fromLocaleString(text);
        }
    }
}

