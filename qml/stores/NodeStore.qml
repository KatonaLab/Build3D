import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property alias model: model
    // TODO: make VolumetricData size handling dynamic
    // so dont store this information
    // also change VolumetricData to VolumeData, 'cos it's shorter
    property vector3d storedVolumeSize: Qt.vector3d(0, 0, 0)

    function onDispatched(actionType, args) {
        // TODO: find a better way for action response than a switch/case
        switch (actionType) {
            case ActionTypes.addSourceNode:
                var path = "../views/nodes/SourceNodeView.qml";
                addNode(args.uid, args.data, path);
                break;
            case ActionTypes.addSegmentNode:
                var path = "../views/nodes/SegmentNodeView.qml";
                addNode(args.uid, args.data, path);
                break;
            case ActionTypes.addAnalysisNode:
                var path = "../views/nodes/AnalysisNodeView.qml";
                addNode(args.uid, args.data, path);
                break;
            case ActionTypes.importIcsFile:
            case ActionTypes.autoImportIcsFile:
                dataManager.source = args.url;
                break;
            case ActionTypes.removeNode:
                remove(args.uid)
                break;
            case ActionTypes.setNodeViewParameters:
                setViewParameters(args.uid, args.parameters)
                break;
            // TODO: cleanWorkspace
        }
    }

    function addNode(uid, data, nodeViewPath) {
        // FIXME: nasty hack
        if (storedVolumeSize.x == 0) {
            storedVolumeSize = Qt.vector3d(data.width, data.height, data.depth);
        }

        var maxDim = Math.max(data.width, data.height, data.depth);
        var item = {
            uid: uid,
            size: Qt.vector3d(data.width / maxDim, 
                data.height / maxDim, data.depth / maxDim),
            data: data,
            nodeViewPath: nodeViewPath,
            nodeViewParams: defaultViewAttributes()
        };
        model.append(item);
    }

    function randomColor() {
        return Qt.rgba(Math.random(), Math.random(), Math.random(), 1.);
    }

    function defaultViewAttributes() {
        // TODO: debug: if visible is true here then you have to click twice on the checkbox to make it truly visible
        return {visible: false, lowCut: 0, highCut: 1, color: randomColor()};
    }

    function setViewParameters(uid, args) {
        for (var i = 0 ; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                item.nodeViewParams = args;
                break;
            }
        }
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
                    AppActions.addSourceNode(AppActions.generateUid(), volumes[i]);
                }
            }
        }
    }
}