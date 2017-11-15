pragma Singleton
import QtQuick 2.8

Item {

    property var middlewares: []

    signal dispatchToStores(string actionType, var parameters)

    function dispatch(actionType, parameters) {
        if (middlewares.length > 0) {
            middlewares[0].dispatch(actionType, parameters);
        } else {
            dispatchToStores(actionType, parameters);
        }
    }

    function addStoreListener(store) {
        // add another subscriber to the dispatchToStores signal
        dispatchToStores.connect(store.onDispatched);
    }

    function addMiddlewareListener(middleware) {
        if (middlewares.length > 0) {
            middlewares[middlewares.length - 1].nextMiddleware = middleware;
        }
        middlewares.push(middleware);
        middlewares[middlewares.length - 1].nextMiddleware = null;
    }
}