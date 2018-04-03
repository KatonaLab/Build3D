import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached SceneStore");
        
        var handlers = {};
        handlers[ActionTypes.node_added_notification] = function(args) {

        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by SceneStore");
        };
        (handlers[actionType] || notHandled)(args);
    }
}
