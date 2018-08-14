// import QtQuick 2.8
// import QtQuick.Dialogs 1.2
// import koki.katonalab.a3dc 1.0

// import QtQuick 2.9
// import QtQuick.Window 2.3
// import QtQuick.Controls 2.3
// import QtQuick.Controls.Material 2.2
// import QtQuick.Layouts 1.3
// import QtQuick.Scene3D 2.0
// import Qt.labs.settings 1.0
// import Qt.labs.platform 1.0
// import koki.katonalab.a3dc 1.0

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import koki.katonalab.a3dc 1.0

// import "actions"
// import "stores"
// import "views"
// import "middlewares"

ApplicationWindow {
    BackendStore {
        id: bs
    }

    width: 480
    height: 480

    // ListView {
    //     anchors.fill: parent
    //     // model: BackendStoreProxy {
    //         // source: bs
    //     // }
    //     model: bs
    //     delegate: Text {
    //         // text: name + " " + type
    //         text: name
    //     }
    // }

    TableView {
        anchors.fill: parent
        model: BackendStoreFilter{
            source: bs
            includeType: ["int"]
            excludeParentUid: [0] 
        }
        TableViewColumn {
            role: "uid"
            width: 100
        }
        TableViewColumn {
            role: "parentUid"
            width: 100
        }
        TableViewColumn {
            role: "category"
            width: 100
        }
        TableViewColumn {
            role: "name"
            width: 100
        }
        TableViewColumn {
            role: "type"
            width: 100
        }
    }

    Component.onCompleted: {
        bs.addModule(0, -1, "module", "x", "dummyType", 0);
        bs.addModule(0, 0, "input", "x.i0", "int", 0);
        bs.addModule(1, 0, "input", "x.i1", "image", 0);
        bs.addModule(0, 0, "output", "x.o1", "image", 0);

        bs.addModule(1, -1, "module", "y", "dummyType", 0);
        bs.addModule(0, 1, "output", "y.o0", "int", 0);
        bs.addModule(1, 1, "output", "y.o1", "image", 0);
        bs.addModule(0, 1, "output", "y.i0", "image", 0);
        
        visible = true;

        // AppDispatcher.addStoreListener(MainStore.moduleStore);
        // AppDispatcher.addStoreListener(MainStore.sceneStore);
        // AppDispatcher.addStoreListener(MainStore.cardStore);
        // AppDispatcher.addMiddlewareListener(dialogMiddleware);

        // AppActions.refreshModuleList();

        // // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Sandbox/testset/K32_bassoon_TH_vGluT1_c01_cmle.ics");
        // // AppActions.autoImportIcsFile("file:///Users/fodorbalint/Desktop/spheres.ics");        
    }

    // MainWindow {
    //     id: mainWindow

    //     // FileDialog should be the child of the MainWindow
    //     DialogMiddleware {
    //         id: dialogMiddleware
    //     }
    // }
}
