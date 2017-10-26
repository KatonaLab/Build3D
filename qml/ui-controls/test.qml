import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: root
    width: 300
    height: 300
    DualSlider {
        id: slider
        anchors.fill: parent
    }
}