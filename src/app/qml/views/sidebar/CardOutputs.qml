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
    property font font
    property int uid: -1

    delegate: Loader {
        property int uid: root.uid
        property var details: model

        sourceComponent: {
            switch(model.type) {
                case "volume": return volumeOutputDelegate;
                case "?": return volumeOutputDelegate;
            }
        }

        Component {
            id: volumeOutputDelegate
            Label {
                text: details.displayName
                font: root.font
            }
        }
    }
}
