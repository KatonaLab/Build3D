import QtQuick 2.8

Item {
    width: 200
    height: 200

    Rectangle {
        anchors.fill: parent
        color: "Gray"
        ColorIndicator {
            anchors.centerIn: parent
        }
    }
}