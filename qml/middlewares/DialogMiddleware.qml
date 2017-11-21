import QtQuick 2.8
import QtQuick.Dialogs 1.2

import "../actions"

Middleware {

    function dispatch(actionType, parameters) {
        if (actionType == ActionTypes.importIcsFile) {
            fileDialog.open();
            return;
        }
        next(actionType, parameters);
    }

    FileDialog {
        id: fileDialog
        title: "Import"
        folder: shortcuts.home
        onAccepted: {
            console.log(fileDialog.fileUrls[0]);
            next(ActionTypes.importIcsFile, {url: fileDialog.fileUrls[0]});
        }
    }
}
