pragma Singleton
import QtQuick 2.8
import koki.katonalab.a3dc 1.0

Item {
    property alias model: model

    BackendStore {
        id: model
        
        Component.onCompleted: {
            // demo data
            addModule("modules/examples/module_volume_generator.py");
            addModule("modules/examples/module_volume_generator.py");
            addModule("modules/examples/module_volume_generator.py");
            addModule("modules/experimental/module_binarize.py");
        }
    }
}
