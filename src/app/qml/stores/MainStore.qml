pragma Singleton
import QtQuick 2.8

Item {
    property alias moduleStore: moduleStore
    ModuleStore {
        id: moduleStore
    }
}
