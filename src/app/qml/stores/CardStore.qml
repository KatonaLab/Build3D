import QtQuick 2.8
import QtQml.Models 2.1
import koki.katonalab.a3dc 1.0

import "../actions"

Item {
    
    id: root
    property alias model: model

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached CardStore");
        
        var handlers = {};
        
        handlers[ActionTypes.node_added_notification] = function(args) {
            var object = Qt.createQmlObject('import QtQuick 2.8; Rectangle {width:100; height:100; color:"blue"}', model);
            model.append(object);
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by CardStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ObjectModel {
        id: model
        // Rectangle { height: 30; width: 80; color: "red" }
        // Rectangle { height: 30; width: 80; color: "green" }
        // Rectangle { height: 30; width: 80; color: "blue" }
    }
}
