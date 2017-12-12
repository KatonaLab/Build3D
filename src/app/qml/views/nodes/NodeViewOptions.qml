import QtQuick 2.8
import QtQuick.Controls 1.5

import "../../stores"
import "../../actions"
import "../controls"

GroupBox {
    
    property int uid: -1
    property var nodeViewParams

    function viewParameters() {
        return {visible: visibleCheck.checked,
                lowCut: slider.lowValue,
                highCut: slider.highValue,
                color: colorSelect.color};
    }

    // TODO: maybe put in a ColumnLayout
    CheckBox {
        id: visibleCheck

        text: "Show"
        checked: nodeViewParams.visible

        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width - colorSelect.width
        anchors.margins: 8

        onClicked: {
            AppActions.setNodeViewParameters(uid, viewParameters());
        }
    }

    ColorIndicator {
        id: colorSelect

        color: nodeViewParams.color

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 8

        onColorChanged: {
            AppActions.setNodeViewParameters(uid, viewParameters());
        }
    }

    DualSlider {
        id: slider

        lowValue: nodeViewParams.lowCut
        highValue: nodeViewParams.highCut

        anchors.left: parent.left
        anchors.top: visibleCheck.bottom
        anchors.topMargin: 8
        width: parent.width - 17*2
        anchors.margins: 8

        onLowValueChanged: {
            AppActions.setNodeViewParameters(uid, viewParameters());
        }
        onHighValueChanged: {
            AppActions.setNodeViewParameters(uid, viewParameters());
        }
    }
}