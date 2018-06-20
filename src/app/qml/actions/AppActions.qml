pragma Singleton
import QtQuick 2.8

import "."

QtObject {

    function importIcsFile(url) {
        AppDispatcher.dispatch(ActionTypes.ics_file_import, {url: url});
    }

    function requestAddModule(scriptPath) {
        AppDispatcher.dispatch(ActionTypes.module_add_request, {scriptPath: scriptPath});
    }

    function notifyModuleAdded(uid) {
        AppDispatcher.dispatch(ActionTypes.module_added_notification, {uid: uid});
    }

    function requestRemoveModule(uid) {
        AppDispatcher.dispatch(ActionTypes.module_remove_request, {uid: uid});
    }

    function notifyModuleRemoved(uid) {
        AppDispatcher.dispatch(ActionTypes.module_removed_notification, {uid: uid});
    }

    function requestModulePropertiesChange(uid, values) {
        AppDispatcher.dispatch(ActionTypes.module_properties_change_request, {uid: uid, values: values});
    }

    function notifyModulePropertiesChanged(uid, values) {
        AppDispatcher.dispatch(ActionTypes.module_properties_changed_notification, {uid: uid, values: values});
    }
    
    function requestModuleInputChange(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_input_change_request, {uid: uid, portId: portId, values: values});
    }

    function notifyModuleInputChanged(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_input_changed_notification, {uid: uid, portId: portId, values: values});
    }

    function requestModuleParamChange(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_param_change_request, {uid: uid, portId: portId, values: values});
    }

    function notifyModuleParamChanged(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_param_changed_notification, {uid: uid, portId: portId, values: values});
    }

    function requestModuleOutputChange(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_output_change_request, {uid: uid, portId: portId, values: values});
    }

    function notifyModuleOutputChanged(uid, portId, values) {
        AppDispatcher.dispatch(ActionTypes.module_output_changed_notification, {uid: uid, portId: portId, values: values});
    }

    function evaluatePlatform() {
        AppDispatcher.dispatch(ActionTypes.platform_evaluation, {});
    }

    function refreshAllModuleOutput() {
        AppDispatcher.dispatch(ActionTypes.all_module_output_refresh, {});
    }

    function refreshModuleList() {
        AppDispatcher.dispatch(ActionTypes.module_list_refresh, {});
    }
}
