import QtQuick 2.8
import QtQuick.Controls 1.5

Label {
    property real progress

    // TODO: text for loading
    anchors.centerIn: parent
    text: "loading " + progress*100 + "%"
    color: "white"
}