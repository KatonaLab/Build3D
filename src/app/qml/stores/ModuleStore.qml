import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property alias backend: backend
    property alias supportedModules: supportedModules

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached ModuleStore");
        
        var handlers = {};

        handlers[ActionTypes.ics_file_import] = function(args) {
            var uids = backend.createSourceModulesFromIcsFile(args.url);
            uids.forEach(function(uid) {
                AppActions.notifyModuleAdded(uid);
            });
        };

        handlers[ActionTypes.module_add_request] = function(args) {
            var uid = backend.createGenericModule(args.scriptPath);
            AppActions.notifyModuleAdded(uid);
        };

        handlers[ActionTypes.module_remove_request] = function(uid) {
            // TODO:
        };

        handlers[ActionTypes.module_input_change_request] = function(args) {
            backend.disconnectInput(args.uid, args.portId);
            var success = backend.connectInputOutput(
                args.values.targetUid, args.values.targetPortId,
                args.uid, args.portId);
            if (!success) {
                // TODO: proper error messaging
                console.warn("can not connect ports");
            }
            args.values.inputOptionForceReset = !success;
            AppActions.notifyModuleInputChanged(args.uid, args.portId, args.values);
        };

        handlers[ActionTypes.module_output_change_request] = function(args) {
            
            AppActions.notifyModuleOutputChanged(args.uid, args.portId, args.values);
        };

        handlers[ActionTypes.module_param_change_request] = function(args) {
            backend.setParamPortProperty(args.uid, args.portId, args.values.value);
            AppActions.notifyModuleParamChanged(args.uid, args.portId, args.values);
        };

        handlers[ActionTypes.platform_evaluation] = function(args) {
            backend.evaluatePlatform();
            AppActions.refreshAllModuleOutput();
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by ModuleStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ModulePlatformBackend {
        id: backend
    }

    ListModel {
        id: supportedModules
        ListElement {
            displayName: "create test module"
            scriptPath: "scripts/test_module.py"
        }
        ListElement {
            displayName: "create sine generator module"
            scriptPath: "scripts/test_sine_module.py"
        }
        ListElement {
            displayName: "create test threshold module"
            scriptPath: "scripts/test_threshold_module.py"
        }
    }
}
