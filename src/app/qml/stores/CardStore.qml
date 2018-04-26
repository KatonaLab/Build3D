import QtQuick 2.8
import QtQml.Models 2.1
import koki.katonalab.a3dc 1.0

import "../actions"

Item {
    
    id: root
    property alias model: model

    function createCardForNewModule(newUid) {
        var backend = MainStore.moduleStore.backend;
        var module = backend.getModuleProperties(newUid);
        // var inputs = backend.enumerateInputPorts(newUid);
        var params = backend.enumerateParamPorts(newUid);
        var outputs = backend.enumerateOutputPorts(newUid);
        
        // var inputProps = [];
        // inputs.forEach(function(x) {
        //     var props = backend.getInputPortProperties(newUid, x);
        //     var opts = [];
        //     props.options.forEach(function(y) {
        //         opts.push({
        //             "displayName": y.targetModuleDisplayName + ":" + y.targetPortDisplayName,
        //             "targetUid": y.targetUid,
        //             "targetPortId": y.targetPortId});
        //     });
        //     props.options = opts;
        //     inputProps.push(props);
        // });

        var paramProps = [];
        params.forEach(function(x) {
            paramProps.push(backend.getInputPortProperties(newUid, x));
        });

        var outputProps = [];
        outputs.forEach(function(x) {
            outputProps.push(backend.getOutputPortProperties(newUid, x));
        });

        return {
            uid: newUid,
            displayName: module.displayName,
            inputs: [],
            parameters: paramProps,
            outputs: outputProps};
    }

    function updateInputs(modelIndex) {
        if (modelIndex < 0 || modelIndex >= model.count) {
            console.warn("can not perform 'updateInputs' for modelIndex: " + modelIndex);
            return;
        }

        var backend = MainStore.moduleStore.backend;
        var uid = model.get(modelIndex).uid;
        console.debug("update input uid " + uid);
        var inputs = backend.enumerateInputPorts(uid);

        var inputProps = [];
        inputs.forEach(function(x) {
            var props = backend.getInputPortProperties(uid, x);
            var opts = [];
            props.options.forEach(function(y) {
                opts.push({
                    "displayName": y.targetModuleDisplayName + ":" + y.targetPortDisplayName,
                    "targetUid": y.targetUid,
                    "targetPortId": y.targetPortId});
            });
            props.options = opts;
            inputProps.push(props);
        });

        model.get(modelIndex).inputs = inputProps;
    }

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached CardStore");
        
        var handlers = {};
        var backend = MainStore.moduleStore.backend;
        
        handlers[ActionTypes.module_added_notification] = function(args) {
            var newItem = createCardForNewModule(args.uid);
            model.append(newItem);
            for (var i = 0; i < model.count; ++i) {
                updateInputs(i);
            }
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
