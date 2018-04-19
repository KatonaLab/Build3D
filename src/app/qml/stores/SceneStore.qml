import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {
    property alias model: model

    function findModelIndex(m, criteria) {
        for(var i = 0; i < m.count; ++i) {
            if (criteria(m.get(i))) {
                return {index: i, item: m.get(i)};
            }
        }
        return null;
    }

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached SceneStore");
        
        var handlers = {};
        handlers[ActionTypes.module_added_notification] = function(args) {
            var vol = MainStore.moduleStore.backend.getModuleTexture(args.uid, 0);
            var m = Math.max(vol.size.x, vol.size.y, vol.size.z);
            var size = Qt.vector3d(vol.size.x / m, vol.size.y / m, vol.size.z / m);
            model.append({
                uid: args.uid,
                portId: 0,
                texture: vol,
                size: size,
                lut: {low:0, high:1}});
        };

        handlers[ActionTypes.module_output_changed_notification] = function(args) {
            var x = findModelIndex(model, function (item) {
                return (args.uid == item.uid) && (args.portId == item.portId);
            });
            // TODO: hangs if JSON.stringify(x) called
            console.log(x);
            // if (x !== null) {
            //     // x.item.lut = {"low": args.firstValue, "high": args.secondValue};
            //     // model.set(x.index, x.item);
            //     console.log(JSON.stringify(x.item));
            // }
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
