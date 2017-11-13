import QtQuick 2.8

import "../actions"

Item {

    property int nextUid: 0
    property alias model: model

    function onDispatched(actionType, parameters) {
        // TODO: find a better way for action response than a switch/case
        switch (actionType) {
            case ActionTypes.addSourceNode:
                addSource(parameters.uid, parameters.data, parameters.name);
                break;
            case ActionTypes.addSegmentNode:
            case ActionTypes.addAnalysisNode:
                // add(parameters.uid);
                break;
            case ActionTypes.removeNode:
                remove(parameters.uid)
                break;
            case ActionTypes.setNodeViewParameters:
                setViewParameters(parameters.uid, parameters.parameters)
                break;
        }
    }

    function setViewParameters(uid, parameters) {
        for (var i = 0 ; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                item.viewParameters = parameters;
                break;
            }
        }
    }

    function defaultViewAttributes() {
        return {visible: true, lowCut: 0, highCut: 1, color: Qt.rgba(0,0,0,1)};
    }

    function addSource(uid, data, name) {
        var d = Math.max(data.width, data.height, data.depth);
        var item = {
            uid: uid,
            width: data.width / d,
            height: data.height / d,
            depth: data.depth / d,
            data: data,
            name: name,
            viewParameters: defaultViewAttributes()
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