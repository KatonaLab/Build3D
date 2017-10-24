import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    // private property
    QtObject {
        id: d
        property var dynamicViews : []
    }

    function clearControls()
    {
        for (var i = 0; i < d.dynamicViews.length; ++i) {
            d.dynamicViews[i].destroy();
        }
        d.dynamicViews = [];
    }

    function createViewControl(volumeData, volumeCube)
    {
        var component = Qt.createComponent("ChannelView.qml");
        // TODO: set the ranges properly
        var object = component.createObject(layout, {"from": 0, "to": 10});
        d.dynamicViews.push(object);
    }

    color: sysColors.dark
    border.color: sysColors.mid
    border.width: 1

    SystemPalette {
        id: sysColors
        colorGroup: SystemPalette.Active
    }

    ScrollView {
        anchors.fill: parent
        ColumnLayout {
            id: layout
            anchors.fill: parent
        }
    }
}