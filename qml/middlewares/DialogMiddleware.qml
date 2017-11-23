import QtQuick 2.8
import QtQuick.Dialogs 1.2

import "../actions"

Middleware {
    id: middleware

    property url folder: "."

    function dispatch(actionType, parameters) {
        if (actionType == ActionTypes.importIcsFile) {
            openDialog.open();
            return;
        }
        // if (actionType == ActionTypes.saveAnalysisCsv) {
        //     saveDialog.open();
        //     return;
        // }
        next(actionType, parameters);
    }

    FileDialog {
        id: openDialog
        title: "Import"
        folder: middleware.folder
        onAccepted: {
            console.log(openDialog.openUrls[0]);
            next(ActionTypes.importIcsFile, {url: openDialog.fileUrls[0]});
        }
    }

    // FileDialog {
    //     id: saveDialog
    //     title: "Save"
    //     folder: middleware.folder
    //     onAccepted: {
    //         console.log(saveDialog.fileUrls[0]);
    //         next(ActionTypes.saveAnalysisCsv, {url: saveDialog.fileUrls[0]});
    //     }
    // }
}
