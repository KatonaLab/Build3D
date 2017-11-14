import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "../controls"
import "../../actions"

GroupBox {
    id: box

    property int uid: -1
    property string nodeName
    property var nodeViewParams

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        Label {text: nodeName}

        NodeViewOptions {
            uid: box.uid
            width: 0 // breaking a binding loop (?), TODO
            nodeViewParams: box.nodeViewParams
            Layout.fillWidth: true
        }
    }
}