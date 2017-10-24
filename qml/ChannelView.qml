import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0

Item {
    id: root
    height: layout.height

    property string text
    property real from
    property real to
    property alias channelColor: colorIndicator.color

    signal visibilityChanged(bool visible)
    signal valueChanged(real first, real second)

    Component.onCompleted: syncSliders(true)

    function syncSliders(lowChanged)
    {
        if (lowChanged) {
            highSlider.value = Math.max(lowSlider.value, highSlider.value);
        } else {
            lowSlider.value = Math.min(lowSlider.value, highSlider.value);
        }
        minLimitLabel.text = (from).toFixed(2);
        maxLimitLabel.text = (to).toFixed(2);
        minLabel.text = (lowSlider.value).toFixed(2);
        maxLabel.text = (highSlider.value).toFixed(2);
        valueChanged(lowSlider.value, highSlider.value);
    }

    ColumnLayout {
        id: layout
        // anchors.fill: parent
        RowLayout {
            CheckBox {
                id: visibilityCheckBox
                text: root.text
                onClicked: root.visibilityChanged(checked)
                Layout.fillWidth: true
            }
            ColorIndicator {
                id: colorIndicator
            }
        }
        RowLayout {
            Label {
                id: minLimitLabel
                Layout.fillWidth: true
            }
            Label {id: maxLimitLabel}
        }
        Slider {
            id: lowSlider
            minimumValue: from
            maximumValue: to
            onValueChanged: root.syncSliders(true)
        }
        Slider {
            id: highSlider
            minimumValue: from
            maximumValue: to
            onValueChanged: root.syncSliders(false)
        }
        RowLayout {
            Label {
                id: minLabel
                Layout.fillWidth: true
            }
            Label {id: maxLabel}
        }
    }
}