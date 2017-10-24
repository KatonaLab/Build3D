import QtQuick 2.8

Item {
    width: 400
    height: 400

    SegmentOptions {
        from: 0
        to: 10
        title: "hello world"
        channelColor: "#aa77cc"
        onVisibilityChanged: {
            console.log("demo: onVisibilityChanged");
        }
        onValueChanged: function(first, second) {
            console.log("demo: onValueChanged ", first, second)
        }
        onChannelColorChanged: {
            console.log("demo: color changed", channelColor)
        }
    }
}