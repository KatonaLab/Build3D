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

    function changeModuleProperties(uid, props) {
        AppDispatcher.dispatch(ActionTypes.module_properties_change, {uid: uid, props: props});
    }

    function requestRemoveModule(uid) {
        AppDispatcher.dispatch(ActionTypes.module_remove_request, {uid: uid});
    }

    function notifyModuleRemoved(uid) {
        AppDispatcher.dispatch(ActionTypes.module_removed_notification, {uid: uid});
    }

    function requestModulePropertyChange(uid, propIndex, values) {
        AppDispatcher.dispatch(ActionTypes.module_property_change_request, {uid: uid, index: propIndex, values: values});
    }

    function notifyModulePropertyChanged(uid, propIndex, values) {
        AppDispatcher.dispatch(ActionTypes.module_property_changed_notification, {uid: uid, index: propIndex, values: values});
    }
}
