import QtQuick 2.8

import "../actions"

Item {

    property Item nextMiddleware

    function dispatch(actionType, args) {
        // no-op
        console.warning("not implemented middleware dispatch");
    }

    function next(actionType, args) {
        if (nextMiddleware != null) {
            nextMiddleware.dispatch(actionType, args);
        } else {
            AppDispatcher.dispatchToStores(actionType, args);
        }
    }
}