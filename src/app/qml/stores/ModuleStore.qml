import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property alias backend: backend

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
            
        };

        handlers[ActionTypes.module_remove_request] = function(uid) {
            
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by ModuleStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    ModulePlatformBackend {
        id: backend
    }
}
