pragma Singleton
import QtQuick 2.8

import "."

QtObject {

    function importIcsFile(url) {
        AppDispatcher.dispatch(ActionTypes.ics_file_import, {url: url});
    }

    function requestAddNode() {
        AppDispatcher.dispatch(ActionTypes.node_add_request, {});
    }

    function notifyNodeAdded(uid, params) {
        AppDispatcher.dispatch(ActionTypes.node_added_notification, {uid: uid, params: params});
    }

    function requestRemoveNode(uid) {
        AppDispatcher.dispatch(ActionTypes.node_remove_request, {uid: uid});
    }

    function notifyNodeRemoved(uid) {
        AppDispatcher.dispatch(ActionTypes.node_removed_notification, {uid: uid});
    }
}