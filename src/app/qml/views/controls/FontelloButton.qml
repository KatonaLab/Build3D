import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.2

ToolButton {
    id: root

    property Action action

    onClicked: action == null ? undefined : action.trigger()
    implicitWidth: 28
    implicitHeight: 28

    style: ButtonStyle {
            label: Component {
                Text {
                    font.family: "fontello"
                    text: root.text
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
}