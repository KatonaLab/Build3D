import QtQuick 2.8
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import QtQuick.Window 2.0

import "../controls"
import "../../actions"
import "../../stores"

GroupBox {
    id: box

    property int uid: -1
    property string nodeName
    property var nodeViewParams
    property bool nodeApplied

    Action {
        id: removeAction
        text: "Remove"
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: nodeName
                Layout.fillWidth: true
            }
            FontelloButton {
                text: "\uE822"
                action: removeAction
            }
        }

        NodeViewOptions {
            uid: box.uid
            enabled: nodeApplied
            width: 0 // breaking a binding loop (?), TODO
            nodeViewParams: box.nodeViewParams
            Layout.fillWidth: true
        }

        GroupBox {
            enabled: !nodeApplied
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 2
                anchors.fill: parent
                
                Label {
                    text: "Channel 1"
                }

                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    model: ListModel {
                        ListElement {
                            text: "vGlut"
                        }
                        ListElement {
                            text: "CB1"
                        }
                    }
                }

                RowLayout {
                    Label {
                        text: "filters"
                        Layout.fillWidth: true
                    }
                    Label {
                        text: "min"
                        Layout.preferredWidth: 60
                    }
                    Label {
                        text: "max"
                        Layout.preferredWidth: 60
                    }
                }

                RowLayout {
                    Label {
                        text: "volume"
                        Layout.fillWidth: true
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    FontelloButton {
                        text: "\uE822"
                    }
                }

                RowLayout {
                    Label {
                        text: "intensity"
                        Layout.fillWidth: true
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    FontelloButton {
                        text: "\uE822"
                    }
                }

                Button {
                    text: "Add Filter"
                    Layout.fillWidth: true
                }

            }
        }

        GroupBox {
            enabled: !nodeApplied
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 2
                anchors.fill: parent
                
                Label {
                    text: "Channel 2"
                }

                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    model: ListModel {
                        ListElement {
                            text: "vGlut"
                        }
                        ListElement {
                            text: "CB1"
                        }
                    }
                }

                RowLayout {
                    Label {
                        text: "filters"
                        Layout.fillWidth: true
                    }
                    Label {
                        text: "min"
                        Layout.preferredWidth: 60
                    }
                    Label {
                        text: "max"
                        Layout.preferredWidth: 60
                    }
                }

                RowLayout {
                    Label {
                        text: "volume"
                        Layout.fillWidth: true
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    TextField {
                        Layout.preferredWidth: 60
                    }
                    FontelloButton {
                        text: "\uE822"
                    }
                }


                Button {
                    text: "Add Filter"
                    Layout.fillWidth: true
                }

            }
        }

        Button {
            text: "Add Channel"
            Layout.fillWidth: true
        }

        Button {
            text: "Run"
            Layout.fillWidth: true
        }

    }
}