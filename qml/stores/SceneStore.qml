import QtQuick 2.8

import "../actions"

Item {

    property int nextUid: 0
    property alias model: model

    function onDispatched(actionType, parameters) {
        // TODO: find a better way for action response than a switch/case
        switch (actionType) {
            case ActionTypes.addSourceNode:
                addSource(parameters.uid, parameters.data);
                break;
            case ActionTypes.addSegmentNode:
            case ActionTypes.addAnalysisNode:
                // add(parameters.uid);
                break;
            case ActionTypes.removeNode:
                remove(parameters.uid)
                break;
        }
    }

    function addSource(uid, data) {
        var item = {
            uid: uid,
            width: Math.random(),
            height: Math.random(),
            depth: Math.random(),
            data: data
        };
        model.append(item);
    }

    // function add(uid) {
    //     var item = {
    //         uid: uid,
    //         width: Math.random(),
    //         height: Math.random(),
    //         depth: Math.random()
    //     };
    //     model.append(item);
    // }

    function remove(uid) {
        for (var i = 0 ; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                model.remove(i);
                break;
            }
        }
    }

    ListModel {
        id: model
    }   
}