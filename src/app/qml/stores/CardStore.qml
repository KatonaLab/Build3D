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
        var backend = MainStore.moduleStore.backend;
        
        handlers[ActionTypes.module_added_notification] = function(args) {
            var inputs = backend.getInputs(args.uid);
            var params = backend.getParameters(args.uid);
            var outputs = backend.getOutputs(args.uid);
            console.debug(JSON.stringify(inputs));
            console.debug(JSON.stringify(outputs));
            
            inputs.forEach(function(part, index, theArray) {
                var opts = backend.getInputOptions(args.uid, part.portId);
                opts = opts.filter(function(item) {
                    return args.uid != item.uid;
                });
                theArray[index].options = opts;
            });

            model.append({
                uid: args.uid,
                displayName: "TODO: query name",
                inputs: inputs,
                parameters: params,
                outputs: outputs});
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
