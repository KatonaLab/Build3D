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
        if (actionType == ActionTypes.saveAnalysisCsv) {
            saveDialog.uid = parameters.uid;
            saveDialog.open();
            return;
        }
        next(actionType, parameters);
    }

    FileDialog {
        id: openDialog
        title: "Import"
        folder: middleware.folder
        selectMultiple: false
        nameFilters: [ "Image Cytometry Standard (*.ics)" ]
        onAccepted: {
            console.log(openDialog.fileUrl);
            next(ActionTypes.importIcsFile, {url: openDialog.fileUrl});
        }
    }

    FileDialog {
        property int uid: -1;

        id: saveDialog
        title: "Save"
        folder: middleware.folder
        nameFilters: [ "CSV (*.csv)" ]
        selectExisting: false
        selectMultiple: false
        onAccepted: {
            console.log(saveDialog.fileUrl);
            next(ActionTypes.saveAnalysisCsv, {uid: uid, url: saveDialog.fileUrl});
        }
    }
}
