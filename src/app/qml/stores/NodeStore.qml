import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    function onDispatched(actionType, args) {
        console.debug("action " + actionType + " reached NodeStore");
        
        var handlers = {};

        handlers[ActionTypes.ics_file_import] = function(args) {
            volumeCollection.source = args.url;
        };

        handlers[ActionTypes.node_add_request] = function(args) {
            var node = null;
            switch (args.type) {
                case "source":
                    node = platform.createSourceNode(args.data);
                    break;
                case "generic":
                    node = platform.createGenericNode(args.script);
                    break;
                default:
                    // TODO: error handling
                    console.error("no node type", args.type)
                    return;
            }
            AppActions.notifyNodeAdded(node.uid, node.params);
        };

        handlers[ActionTypes.node_remove_request] = function(uid) {
            if (platform.hasNode(uid)) {
                AppActions.notifyNodeRemoved(uid);
                platform.removeNode(uid);
            }
        };

        var notHandled = function(args) {
            console.debug(actionType, "is not handled by NodeStore");
        };
        (handlers[actionType] || notHandled)(args);
    }

    VolumeDataCollection {
        id: volumeCollection
        onStatusChanged: {
            console.debug('status change received', status);
            if (status === Component.Ready) {
                for (var i = 0; i < volumes.length; ++i) {
                    AppActions.requestAddNode({type: "source", data: volumes[i]});
                }
            }
        }
    }

    NodePlatform {
        id: platform
    }
}
