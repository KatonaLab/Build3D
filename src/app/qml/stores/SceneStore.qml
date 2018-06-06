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
    
    // TODO: move this to some utils.js file
    function findModelIndex(m, criteria) {
        for(var i = 0; i < m.count; ++i) {
            if (criteria(m.get(i))) {
                return i;
            }
        }
        return null;
    }

    VolumeTexture {
        id: defaultTexture
    }

    function updateTexture(idx) {
        var x = model.get(idx);

        var item = {"uid": x.uid,
            "portId": x.portId,
            "color": x.color,
            "lutLow": x.lutLow,
            "lutHigh": x.lutHigh,
            "visible": x.visible};

        var vol = MainStore.moduleStore.backend.getOutputTexture(x.uid, x.portId);
        var m = Math.max(vol.size.x, vol.size.y, vol.size.z);
        var size = Qt.vector3d(vol.size.x / m, vol.size.y / m, vol.size.z / m);

        item['texture'] = vol;
        item['size'] = size;

        // TODO: remove and insert is the workaround to ensure that VolumeTexture object is destroyed
        // it should be done in a neater way
        model.remove(idx);
        model.insert(idx, item)
    }

    function onDispatched(actionType, args) {
        var backend = MainStore.moduleStore.backend;

        var handlers = {};
        handlers[ActionTypes.module_added_notification] = function(args) {
            var outputs = backend.enumerateOutputPorts(args.uid);
            outputs.forEach(function (x) {
                var props = backend.getOutputPortProperties(args.uid, x);
                if (props.type == "float-image") {
                    model.append({
                        "uid": args.uid,
                        "portId": props.portId,
                        "color": Qt.rgba(1, 0, 0, 1),
                        "lutLow": 0,
                        "lutHigh": 1,
                        "visible": true,
                        "texture": defaultTexture,
                        // TODO: Qt.vector3d(1, 1, 1) is crucial, if it is Qt.vector3d(0, 0, 0) then
                        // the program crashes on windows, find a way in the backend for protection
                        "size": Qt.vector3d(1, 1, 1)});
                }
            });
        };

        handlers[ActionTypes.module_output_changed_notification] = function(args) {
            var idx = findModelIndex(model, function (item) {
                return (args.uid == item.uid) && (args.portId == item.portId);
            });

            // // TODO: FIXME: find a neat way to handle this preModle - model mess
            // var preIdx = findModelIndex(preModel, function (item) {
            //     return (args.uid == item.uid) && (args.portId == item.portId);
            // });

            if (idx !== null) {
                model.get(idx).lutLow = args.values.firstValue;
                model.get(idx).lutHigh = args.values.secondValue;
                model.get(idx).color = args.values.color;
                model.get(idx).visible = args.values.visible;
            }

            // if (preIdx !== null) {
            //     preModel.get(preIdx).lutLow = args.values.firstValue;
            //     preModel.get(preIdx).lutHigh = args.values.secondValue;
            //     preModel.get(preIdx).color = args.values.color;
            //     preModel.get(preIdx).visible = args.values.visible;
            // }
        };

        handlers[ActionTypes.all_module_output_refresh] = function(args) {
            for(var i = 0; i < model.count; ++i) {
                updateTexture(i);
            }
        };

        handlers[ActionTypes.module_removed_notification] = function(args) {
            var idx = findModelIndex(model, function (item) {
                return args.uid == item.uid;
            });
            if (idx !== null) {
                model.remove(idx);
            }
        };

        var notHandled = function(args) {};
        (handlers[actionType] || notHandled)(args);
    }

    ListModel {
        id: model
    }
}
