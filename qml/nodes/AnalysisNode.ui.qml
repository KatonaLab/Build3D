import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    width: parent.width
    height: childrenRect.height
    ColumnLayout {
        spacing: 2

        RowLayout {
            Layout.fillWidth: true
            Text {
                width: 50
                text: "param #1"
            }
            TextField {
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                width: 50
                text: "param #2"
            }
            TextField {
                Layout.fillWidth: true
            }
        }

        CheckBox {
            Layout.fillWidth: true
            text: "checkable parameter"
        }
    }
}
