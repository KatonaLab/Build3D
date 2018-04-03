import QtQuick 2.8
import QtQuick.Dialogs 1.2

import "../actions"

Middleware {
    id: middleware

    property url folder: "."

    function dispatch(actionType, args) {
        console.debug("action " + actionType + " reached DialogMiddleware");
        
        var handlers = {};
        handlers[ActionTypes.ics_file_import] = function(args) {
            openDialog.open();
        };

        var notHandled = function(args) {
            next(actionType, args);
        };
        (handlers[actionType] || notHandled)(args);
    }

    FileDialog {
        id: openDialog
        title: "Import"
        folder: middleware.folder
        selectMultiple: false
        nameFilters: [ "Image Cytometry Standard (*.ics)" ]
        onAccepted: {
            console.debug(openDialog.fileUrl);
            next(ActionTypes.ics_file_import, {url: openDialog.fileUrl});
        }
    }
}
