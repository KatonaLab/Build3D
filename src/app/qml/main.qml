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

    ListView {
        anchors.fill: parent
        model: BackendStoreProxy {
            source: bs
        }
        // model: bs
        delegate: Text {
            // text: name + " " + type
            text: name
        }
    }

    // TreeView {
    //     anchors.fill: parent
    //     model: BackendStoreProxy{
    //         source: bs
    //     }
    //     TableViewColumn {
    //         role: "uid"
    //         width: 100
    //     }
    //     TableViewColumn {
    //         role: "name"
    //         width: 100
    //     }
    //     TableViewColumn {
    //         role: "type"
    //         width: 100
    //     }
    // }

    Component.onCompleted: {
        bs.addModule("hello", "type1");
        bs.addModule("bello", "type1");
        bs.addModule("yello", "type2");
        bs.addModule("yellobellow", "type2");
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
