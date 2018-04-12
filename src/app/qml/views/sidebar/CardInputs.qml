import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../../actions"
import "../components"

Repeater {
    id: root
    property int uid: -1
    property font font

    delegate: ColumnLayout {
        Layout.fillWidth: true

        Label {
            font: root.font
            text: model.displayName
            Layout.fillWidth: true
        }

        ComboBox {
            font: root.font
            Layout.fillWidth: true
            model: ["DataSource1/output", "threshold.1/output"]
        }
    }
}

