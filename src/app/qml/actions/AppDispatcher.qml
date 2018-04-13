pragma Singleton
import QtQuick 2.8

Item {

    property var middlewares: []

    signal dispatchToStores(string actionType, var parameters)

    function dispatch(actionType, parameters) {
        console.log("AppDispatcher received action: ", actionType, " parameters: ", JSON.stringify(parameters));
        if (middlewares.length > 0) {
            console.log("  dispatching to middleware");
            middlewares[0].dispatch(actionType, parameters);
        } else {
            console.log("  dispatching to stores");
            dispatchToStores(actionType, parameters);
        }
    }

    function addStoreListener(store) {
        // add another subscriber to the dispatchToStores signal
        console.debug("added store", store);
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
