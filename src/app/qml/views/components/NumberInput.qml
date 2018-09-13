import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    id: numberInput
    property var details
    property bool intType: true
    property bool hasMin: details.hints.min !== undefined
    property bool hasMax: details.hints.max !== undefined
    property bool hasUnusedValue: details.hints.unusedValue !== undefined
    property bool isLocked: hasUnusedValue && !usingField.checked
    property alias validator: textField.validator

    Label {
        Layout.fillWidth: true
        text: details.name
    }

    Slider {
        id: slider
        visible: hasMin && hasMax
        from: hasMin ? details.hints.min : -Infinity
        to: hasMax ? details.hints.max : Infinity
        value: details.value
        stepSize: details.hints.stepSize || 1
        snapMode: numberInput.intType ? Slider.SnapAlways : Slider.NoSnap
        onValueChanged: {
            if (!isLocked && details.value != value) {
                details.value = value;
            }
        }

        Component.onCompleted: {
            console.log(isLocked);
        }

        MouseArea {
            anchors.fill: parent
            enabled: isLocked
        }
    }

    TextField {
        id: textField
        text: {
            return Number(details.value.toFixed(4)).toLocaleString();
        }
        selectByMouse: true
        horizontalAlignment: TextInput.AlignRight
        readOnly: isLocked
        onEditingFinished: {
            if (isLocked) {
                return;
            }

            var v = null;
            try {
                v = Number.fromLocaleString(text);
            } catch(err) {
                v = null;
            }
            
            var lt = v < details.hints.min;
            var gt = v > details.hints.max;
            var notInRange = (details.hints.min && lt) || (details.hints.max && gt);
            if (notInRange || v === null) {
                if (lt) {
                    details.value = details.hints.min;
                } else if (gt) {
                    details.value = details.hints.max;
                } else {
                    // trick to refresh text and slider value
                    slider.value = Qt.binding(function() { return details.value; });
                    textField.text = Qt.binding(function() { return details.value; });
                }
            } else {
                if (details.value != v) {
                    details.value = v;
                }
            }
        }
    }

    CheckBox {
        id: usingField
        property real tmp
        checked: true
        visible: hasUnusedValue
        onCheckedChanged: {
            if (!checked) {
                tmp = details.value;
                details.value = details.hints.unusedValue || 0;
            } else {
                details.value = tmp;
            }
        }
    }
}