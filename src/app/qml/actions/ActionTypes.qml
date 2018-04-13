import QtQuick 2.8
pragma Singleton

QtObject {

    readonly property string ics_file_import: "ICS_FILE_IMPORT"
    readonly property string module_add_request: "MODULE_ADD_REQUEST"
    readonly property string module_added_notification: "MODULE_ADDED_NOTIFICATION"
    readonly property string module_remove_request: "MODULE_REMOVE_REQUEST"
    readonly property string module_removed_notification: "MODULE_REMOVED_NOTIFICATION"
    readonly property string module_property_change_request: "MODULE_PROPERTY_CHANGE_REQUEST"
    readonly property string module_property_changed_notification: "MODULE_PROPERTY_CHANGED_NOTIFICATION"
}
