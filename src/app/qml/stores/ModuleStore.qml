import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property alias backend: backend
    property alias supportedModules: supportedModules
    property bool modelUpToDate: true

    function onDispatched(actionType, args) {
        var handlers = {};

        handlers[ActionTypes.ics_file_import] = function(args) {
            var uids = backend.createSourceModulesFromIcsFile(args.url);
            uids.forEach(function(uid) {
                AppActions.notifyModuleAdded(uid);
            });
        };

        handlers[ActionTypes.json_file_read] = function(args) {
            backend.readJSON("workflow.json");
        }

        handlers[ActionTypes.json_file_write] = function(args) {
            backend.writeJSON("workflow.json");
        }

        handlers[ActionTypes.module_add_request] = function(args) {
            var uid = backend.createGenericModule(args.scriptPath);
            if (uid >= 0) {
                modelUpToDate = false;
                AppActions.notifyModuleAdded(uid);
            }
        };

        handlers[ActionTypes.module_remove_request] = function(args) {
            if (backend.hasModule(args.uid)) {
                modelUpToDate = false;
                backend.destroyModule(args.uid);
                AppActions.notifyModuleRemoved(args.uid);
                AppActions.refreshAllModuleOutput();
            }
        };

        handlers[ActionTypes.module_input_change_request] = function(args) {
            backend.disconnectInput(args.uid, args.portId);
            var success = backend.connectInputOutput(
                args.values.targetUid, args.values.targetPortId,
                args.uid, args.portId);
            if (!success) {
                // TODO: proper error messaging
                console.warn("can not connect ports");
            } else {
                modelUpToDate = false;
                args.values.inputOptionForceReset = !success;
                AppActions.notifyModuleInputChanged(args.uid, args.portId, args.values);
            }
        };

        handlers[ActionTypes.module_properties_change_request] = function(args) {
            backend.setModuleProperties(args.uid, args.values);
            var newValues = backend.getModuleProperties(args.uid);
            AppActions.notifyModulePropertiesChanged(args.uid, newValues);
        };

        handlers[ActionTypes.module_output_change_request] = function(args) {
            AppActions.notifyModuleOutputChanged(args.uid, args.portId, args.values);
        };

        handlers[ActionTypes.module_param_change_request] = function(args) {
            if (backend.hasModule(args.uid)) {
                modelUpToDate = false;
                backend.setParamPortProperty(args.uid, args.portId, args.values.value);
                AppActions.notifyModuleParamChanged(args.uid, args.portId, args.values);
            }
        };

        handlers[ActionTypes.platform_evaluation] = function(args) {
            backend.evaluatePlatform();
            AppActions.refreshAllModuleOutput();
            modelUpToDate = true;
        };

        handlers[ActionTypes.module_list_refresh] = function(args) {
            var moduleFiles = backend.getModuleScriptsList();
            supportedModules.clear();
            moduleFiles.forEach(function(x) {
                supportedModules.append(x);
            });
        };

        var notHandled = function(args) {};
        (handlers[actionType] || notHandled)(args);
    }

    ModulePlatformBackend {
        id: backend
    }

    ListModel {
        id: supportedModules
    }
}
