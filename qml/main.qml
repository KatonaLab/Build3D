import QtQuick 2.8
import QtQuick.Dialogs 1.2

import "actions"
import "stores"
import "views"
import "middlewares"

Item {

    Component.onCompleted: {        
        AppDispatcher.addStoreListener(MainStore.nodeStore);
        AppDispatcher.addMiddlewareListener(dialogMiddleware);

        // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Desktop/K32_bassoon_TH_vGluT1_c01_cmle.ics");
        // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Desktop/spheres.ics");
    }

    MainWindow {
        id: mainWindow
        visible: true

        // FileDialog should be the child of the MainWindow
        DialogMiddleware {
            id: dialogMiddleware
        }
    }
}