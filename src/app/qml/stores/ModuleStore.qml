pragma Singleton
import QtQuick 2.8
import koki.katonalab.a3dc 1.0

Item {
    property alias model: model

    BackendStore {
        id: model
    }
}
