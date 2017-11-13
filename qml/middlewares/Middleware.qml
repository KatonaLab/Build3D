import QtQuick 2.8

import "../actions"

Item {

    property Item nextMiddleware

    function dispatch(actionType, parameters) {
        // no-op
        console.warning("not implemented middleware dispatch");
    }

    function next(actionType, parameters) {
        if (nextMiddleware != null) {
            return nextMiddleware.dispatch(actionType, parameters);
        } else {
            AppDispatcher.dispatchToStores(actionType, parameters);
        }
    }
}