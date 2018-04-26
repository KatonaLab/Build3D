import QtQuick 2.8
import QtQml.Models 2.2
import koki.katonalab.a3dc 1.0

import "../actions"

Item {
    property alias model: model
    property var starterColors;

    Component.onCompleted: {
        starterColors = [
            Qt.rgba(255, 231, 76, 1),
            Qt.rgba(255, 89, 100, 1),
            Qt.rgba(255, 255, 255, 1),
            Qt.rgba(107, 241, 120, 1),
            Qt.rgba(53, 167, 255, 1)];

        starterColors = starterColors.map(function(c) {
            return Qt.rgba(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a);
        });
    }
    

    function findModelIndex(m, criteria) {
        for(var i = 0; i < m.count; ++i) {
            if (criteria(m.get(i))) {
                return i;
            }
        }
        return null;
    }

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached SceneStore");

        var backend = MainStore.moduleStore.backend;

        var handlers = {};
        handlers[ActionTypes.module_added_notification] = function(args) {
            var outputs = backend.enumerateOutputPorts(args.uid);
            outputs.forEach(function (x) {
                var props = backend.getOutputPortProperties(args.uid, x);
                if (props.type == "float-image") {
                    preModel.append({"uid": args.uid, "portId": props.portId});
                }
            });
        };

        handlers[ActionTypes.module_output_changed_notification] = function(args) {
            var idx = findModelIndex(model, function (item) {
                return (args.uid == item.uid) && (args.portId == item.portId);
            });
            if (idx !== null) {
                model.get(idx).lutLow = args.values.firstValue;
                model.get(idx).lutHigh = args.values.secondValue;
                model.get(idx).color = args.values.color;
            }
        };

        handlers[ActionTypes.all_module_output_refresh] = function(args) {
            model.clear();
            for(var i = 0; i < preModel.count; ++i) {
                var x = preModel.get(i);

                var vol = backend.getOutputTexture(x.uid, x.portId);
                var m = Math.max(vol.size.x, vol.size.y, vol.size.z);
                var size = Qt.vector3d(vol.size.x / m, vol.size.y / m, vol.size.z / m);

                var newItem = {
                    uid: x.uid,
                    portId: x.portId,
                    texture: vol,
                    size: size,
                    color: Qt.rgba(1, 0, 0, 1),
                    lutLow: 0,
                    lutHigh: 1};
                model.append(newItem);
            }
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by SceneStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ListModel {
        id: preModel
    }

    ListModel {
        id: model
    }
}
