import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    width: parent.width
    height: childrenRect.height
    ColumnLayout {
        spacing: 2

        ComboBox {
            Layout.fillWidth: true
            model: ["method #1", "method #2", "method #3", "method #4"]
        }

        CheckBox {
            Layout.fillWidth: true
            text: "checkable parameter"
        }
    }
}
