import QtQuick 2.8
import QtQuick.Dialogs 1.2
import koki.katonalab.a3dc 1.0

import "actions"
import "stores"
import "views"
import "middlewares"

Item {

    Component.onCompleted: {
        // bs.addModule(0, -1, "module", "x", "dummyType", 0);
        // bs.addModule(0, 0, "input", "x.i0", "int", 0);
        // bs.addModule(1, 0, "input", "x.i1", "image", 0);
        // bs.addModule(0, 0, "output", "x.o1", "image", 0);

        // bs.addModule(1, -1, "module", "y", "dummyType", 0);
        // bs.addModule(0, 1, "output", "y.o0", "int", 0);
        // bs.addModule(1, 1, "output", "y.o1", "image", 0);
        // bs.addModule(0, 1, "output", "y.i0", "image", 0);

        // visible = true;

        // AppDispatcher.addStoreListener(MainStore.moduleStore);
        // AppDispatcher.addMiddlewareListener(dialogMiddleware);

        // AppActions.refreshModuleList();

        // // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Sandbox/testset/K32_bassoon_TH_vGluT1_c01_cmle.ics");
        // // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Desktop/spheres.ics");        
    }

    MainWindow {
        id: mainWindow

        // FileDialog should be the child of the MainWindow
        DialogMiddleware {
            id: dialogMiddleware
        }
    }
}
