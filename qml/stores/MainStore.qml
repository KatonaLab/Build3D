pragma Singleton
import QtQuick 2.8

Item {

    property alias nodeStore: nodeStore
    property alias sceneStore: sceneStore

    NodeStore {
        id: nodeStore
    }

    SceneStore {
        id: sceneStore
    }
}