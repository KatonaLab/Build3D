pragma Singleton
import QtQuick 2.8

Item {

    property alias nodeStore: nodeStore

    NodeStore {
        id: nodeStore
    }
}