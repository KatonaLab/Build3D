import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "controls"


Rectangle {
    width: 200
    height: 300
    color: "transparent"

    ListModel {
        id: model
        ListElement {
            title: "hello panel"
        }
    }

    CardPanel {
        model: model
    }
}