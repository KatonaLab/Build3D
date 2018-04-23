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
            // var outputs = backend.getOutputs(args.uid);
            // console.log("____________________________________________");
            // console.log(outputs);
            // console.log(outputs.length);
            // outputs.forEach(function (x) {
            //     console.log("MODULE ADDINF ", x.type);
            //     if (x.type == "float-image") {
            //         model.append({
            //             uid: args.uid,
            //             portId: x.portId,
            //             texture: null,
            //             size: Qt.vector2d(0, 0),
            //             color: starterColors[model.count % starterColors.length],
            //             lutLow: 0,
            //             lutHigh: 1});
            //         console.log("MODULE ADDED ", model.count);
            //     }
            // });
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
            console.log("-- updating output!!!", model.count);
            for(var i = 0; i < model.count; ++i) {
                console.log("-- updating output!!!");
                var x = model.get(i);
                var vol = backend.getModuleTexture(x.uid, x.portId);
                console.log("-- updating output!!!", vol);
                var m = Math.max(vol.size.x, vol.size.y, vol.size.z);
                var size = Qt.vector3d(vol.size.x / m, vol.size.y / m, vol.size.z / m);
                console.log("-- updating output!!!", size);
                model.get(i).texture = vol;
                model.get(i).size = size;

                        model.append({
                        uid: args.uid,
                        portId: x.portId,
                        texture: null,
                        size: Qt.vector2d(0, 0),
                        color: starterColors[model.count % starterColors.length],
                        lutLow: 0,
                        lutHigh: 1});
            }
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
