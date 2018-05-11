import QtQuick 2.8
pragma Singleton

QtObject {

    readonly property string ics_file_import: "ICS_FILE_IMPORT"
    readonly property string module_add_request: "MODULE_ADD_REQUEST"
    readonly property string module_added_notification: "MODULE_ADDED_NOTIFICATION"
    readonly property string module_remove_request: "MODULE_REMOVE_REQUEST"
    readonly property string module_removed_notification: "MODULE_REMOVED_NOTIFICATION"
    readonly property string module_input_change_request: "MODULE_INPUT_CHANGE_REQUEST"
    readonly property string module_input_changed_notification: "MODULE_INPUT_CHANGED_NOTIFICATION"
    readonly property string module_param_change_request: "MODULE_PARAM_CHANGE_REQUEST"
    readonly property string module_param_changed_notification: "MODULE_PARAM_CHANGED_NOTIFICATION"
    readonly property string module_output_change_request: "MODULE_OUTPUT_CHANGE_REQUEST"
    readonly property string module_output_changed_notification: "MODULE_OUTPUT_CHANGED_NOTIFICATION"
    readonly property string platform_evaluation: "PLATFORM_EVALUATION"
    readonly property string all_module_output_refresh: "ALL_MODULE_OUTPUT_REFRESH"
    readonly property string module_list_refresh: "MODULE_LIST_REFRESH"
}
