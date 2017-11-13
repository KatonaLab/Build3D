import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property int nextUid: 0
    property alias model: model

    function onDispatched(actionType, parameters) {
        // TODO: find a better way for action response than a switch/case
        switch (actionType) {
            case ActionTypes.addSourceNode:
                add(parameters.uid, null); 
                break;
            case ActionTypes.addSegmentNode:
                add(parameters.uid, "../views/nodetypes/SegmentNodeView.qml");
                break;
            case ActionTypes.addAnalysisNode:
                add(parameters.uid, "../views/nodetypes/AnalysisNodeView.qml"); 
                break;
            case ActionTypes.importIcsFile:
                dataManager.source = parameters.url;
                break;
            case ActionTypes.removeNode:
                remove(parameters.uid)
                break;
        }
    }

    function randomColor() {
        return Qt.rgba(Math.random(), Math.random(), Math.random(), 1.);
    }

    function defaultViewAttributes() {
        return {visible: true, lowCut: 0, highCut: 1, color: randomColor()};
    }

    function add(uid, componentSource) {
        var item = {
            uid: uid,
            viewAttributes: defaultViewAttributes(),
            componentSource: componentSource
        };
        model.append(item);
    }

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

    VolumetricDataManager {
        id: dataManager
        onStatusChanged: {
            if (status == Component.Ready) {
                for (var i = 0; i < volumes.length; ++i) {
                    console.log("add volume from VolumetricDataManager", volumes[i]);
                    AppActions.addSourceNode(AppActions.generateUid(), volumes[i]);
                }
            }
        }
    }
}