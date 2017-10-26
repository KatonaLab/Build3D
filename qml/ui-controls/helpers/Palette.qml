import QtQuick 2.0

SystemPalette {
    property bool enabled: true
    colorGroup: enabled ? SystemPalette.Active : SystemPalette.Disabled
}