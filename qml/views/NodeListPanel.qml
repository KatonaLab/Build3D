import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    property QtObject nodeManager;

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

    function createViewControl(volumeParam)
    {
        var component = Qt.createComponent("NodeListItem.qml");
        console.debug(component.errorString());
        // TODO: set the ranges properly
        var object = component.createObject(layout);

        object.onChannelColorChanged.connect(function () {
            volumeParam.color = object.channelColor;
        });

        object.onChannelVisibleChanged.connect(function () {
            var c = volumeParam.color;
            volumeParam.color = Qt.rgba(c.r, c.g, c.b, object.channelVisible ? 1.0 : 0.0);
        });

        object.onFromChanged.connect(function () {
            volumeParam.cutParams.z = object.from
            // console.debug(volumeParam.cutParams);
        });

        object.onToChanged.connect(function () {
            volumeParam.cutParams.w = object.to;
            // console.debug(volumeParam.cutParams);
        });

        object.channelColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.);
        object.channelVisible = true;
        var limits = volumeParam.texture.data.dataLimits;
        volumeParam.cutParams.x = limits.x;
        volumeParam.cutParams.y = limits.y;
        object.text = volumeParam.texture.data.dataName;

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
        anchors.margins: 8
        ColumnLayout {
            id: layout
            anchors.fill: parent

            Button {
                text: "add segmentation"
                onClicked: {
                    nodeManager.createSegmentNode();
                }
            }

            Button {
                text: "add analysis"
            }
        }
    }
}