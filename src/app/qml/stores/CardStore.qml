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
        
        handlers[ActionTypes.module_added_notification] = function(args) {
            var props = MainStore.moduleStore.backend.getModuleProperties(args.uid);
            console.log(JSON.stringify(props));
            model.append({
                uid: args.uid,
                displayName: props.displayName,
                inputs: props.inputs,
                parameters: props.parameters,
                outputs: props.outputs});
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by CardStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ListModel {
        id: model
    }
}
