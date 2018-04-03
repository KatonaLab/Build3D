pragma Singleton
import QtQuick 2.8

Item {

    property alias nodeStore: nodeStore
    property alias sceneStore: sceneStore
    property alias cardStore: cardStore

    NodeStore {
        id: nodeStore
    }

    SceneStore {
        id: sceneStore
    }

    CardStore {
        id: cardStore
    }
}