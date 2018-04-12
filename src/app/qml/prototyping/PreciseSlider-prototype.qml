
import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls.Material 2.2

import "../views/components"

Window {
    visible: true
    width: 1024
    height: 768
    title: qsTr("PreciseSlider Prototype")

    Material.theme: Material.Light
    Material.accent: Material.Teal

    PreciseSlider {
        anchors.centerIn: parent
    }
}
