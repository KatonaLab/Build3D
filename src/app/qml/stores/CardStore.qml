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
        var inputs = backend.enumerateInputPorts(newUid);
        var params = backend.enumerateParamPorts(newUid);
        var outputs = backend.enumerateOutputPorts(newUid);

        var inputProps = [];
        inputs.forEach(function(x) {
            var props = backend.getInputPortProperties(newUid, x);
            props.inputOptionForceReset = false;
            inputProps.push(props);
        });

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
            inputs: inputProps,
            parameters: paramProps,
            outputs: outputProps};
    }

    function updateInputs(modelIndex, inputPropCallback) {
        inputPropCallback = inputPropCallback || function (prop) { return prop; };

        if (modelIndex < 0 || modelIndex >= model.count) {
            console.warn("can not perform 'updateInputs' for modelIndex: " + modelIndex);
            return;
        }

        var backend = MainStore.moduleStore.backend;
        var obj = model.get(modelIndex);
        var uid = obj.uid;
        var n = obj.inputs.count;

        for (var i = 0; i < n; ++i) {
            var input = obj.inputs.get(i);
            var props = backend.getInputPortProperties(uid, input.portId);
            input.options = props.options;
        }
    }

    function findModule(uid) {
        for (var i = 0; i < model.count; ++i) {
            if (model.get(i).uid === uid) {
                return model.get(i);
            }
        }
        return null;
    }

    function findInputPort(uid, portId) {
        var m = findModule(uid);
        // TODO: handle bad indices
        for (var i = 0; i < m.inputs.count; ++i) {
            if (m.inputs.get(i).portId === portId) {
                return m.inputs.get(i);
            }
        }
        return null;
    }

    function onDispatched(actionType, args) {
        var handlers = {};
        var backend = MainStore.moduleStore.backend;
        
        handlers[ActionTypes.module_added_notification] = function(args) {
            var newItem = createCardForNewModule(args.uid);
            model.append(newItem);
            for (var i = 0; i < model.count; ++i) {
                updateInputs(i);
            }
        };

        handlers[ActionTypes.module_input_changed_notification] = function(args) {
            var p = findInputPort(args.uid, args.portId);
            // TODO: nasty way to send 'inputOptionForceReset' signal to DynamicComboBox
            // find a better way, this is basicaly a function call
            p.inputOptionForceReset = args.values.inputOptionForceReset;
            p.inputOptionForceReset = false;
        };

        handlers[ActionTypes.module_param_changed_notification] = function(args) {
            var p = findInputPort(args.uid, args.portId);
        };

        var notHandled = function(args) {};
        (handlers[actionType] || notHandled)(args);
    }

    ListModel {
        id: model
    }
}
