pragma Singleton
import QtQuick 2.8

Item {

    property alias moduleStore: moduleStore
    property alias sceneStore: sceneStore
    property alias cardStore: cardStore

    ModuleStore {
        id: moduleStore
    }

    SceneStore {
        id: sceneStore
    }

    CardStore {
        id: cardStore
    }
}
