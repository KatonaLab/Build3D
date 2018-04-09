import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

import "../views/sidebar"
import "CardPanel-sample.js" as SampleData

Rectangle {
    width: 400
    height: 600
    color: "transparent"

    ListModel {
        id: model
        Component.onCompleted: {
            model.append(SampleData.demoModules);
        }
        
    }

    CardPanel {
        model: model
    }
}
