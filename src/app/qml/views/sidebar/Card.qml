import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "controls"

GroupBox {
    property string title

    ColumnLayout {
        spacing: 2
        anchors.fill: parent
        Label {text: title}
    }
}