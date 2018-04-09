import QtQuick 2.8
import QtQuick.Window 2.0
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2

ScrollView {
    id: root
    property ListModel model
    anchors.fill: parent

    ListView {
        anchors.fill: parent
        model: root.model
        spacing: 4
        delegate: Card {
            uid: model.uid
            moduleName: model.moduleName
            controls: model.properties
        }
    }

}
