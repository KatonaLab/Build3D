import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {
    property alias model: model

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached SceneStore");
        
        var handlers = {};
        handlers[ActionTypes.module_added_notification] = function(args) {
            var vol = MainStore.moduleStore.backend.getModuleTexture(args.uid, 0);
            var m = Math.max(vol.size.x, vol.size.y, vol.size.z);
            var size = Qt.vector3d(vol.size.x / m, vol.size.y / m, vol.size.z / m);
            model.append({uid: args.uid, texture: vol, size: size});
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by SceneStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ListModel {
        id: model
    }
}
