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
            var module = backend.getModuleProperties(args.uid);
            var inputs = backend.enumerateInputPorts(args.uid);
            var params = backend.enumerateParamPorts(args.uid);
            var outputs = backend.enumerateOutputPorts(args.uid);
            
            var inputProps = [];
            inputs.forEach(function(x) {
                inputProps.push(backend.getInputPortProperties(args.uid, x));
            });

            var paramProps = [];
            params.forEach(function(x) {
                paramProps.push(backend.getInputPortProperties(args.uid, x));
            });

            var outputProps = [];
            outputs.forEach(function(x) {
                outputProps.push(backend.getOutputPortProperties(args.uid, x));
            });

            console.log(JSON.stringify(module));
            console.log(JSON.stringify(inputProps));
            console.log(JSON.stringify(paramProps));
            console.log(JSON.stringify(outputProps));

            model.append({
                uid: args.uid,
                displayName: module.displayName,
                inputs: inputProps,
                parameters: paramProps,
                outputs: outputProps});
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
