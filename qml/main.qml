import QtQuick 2.8
import QtQuick.Dialogs 1.2

import "actions"
import "stores"
import "views"
import "middlewares"

Item {

    Component.onCompleted: {        
        AppDispatcher.addStoreListener(MainStore.nodeStore);
        AppDispatcher.addStoreListener(MainStore.sceneStore);
        AppDispatcher.addMiddlewareListener(dialogMiddleware);
    }

    MainWindow {
        id: mainWindow

        // FileDialog should be the child of the MainWindow
        DialogMiddleware {
            id: dialogMiddleware
        }
    }
}