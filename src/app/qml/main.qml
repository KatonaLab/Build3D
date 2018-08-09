// import QtQuick 2.8
// import QtQuick.Dialogs 1.2
// import koki.katonalab.a3dc 1.0

import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0
import koki.katonalab.a3dc 1.0

import "actions"
import "stores"
import "views"
import "middlewares"

ApplicationWindow {
    ModuleStoreBackend {
        id: bke
    }

    width: 480
    height: 480

    Text {
        text: "hey"
    }

    ListView {
        anchors.fill: parent
        model: bke
        delegate: Text {
            text: model.display.hello
            Component.onCompleted: {
                console.log("notif");
            }
        }
    }

    Button {
        text: "add";
        onClicked: {
            bke.addModule("again");
        }
    }

    Component.onCompleted: {
        bke.addModule("hello");
        bke.addModule("bello");
        bke.addModule("yello");
        console.log("added all");
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
