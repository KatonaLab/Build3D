import QtQuick 2.8
import koki.katonalab.a3dc 1.0

import "../actions"

Item {

    property alias model: model
    property alias sceneModel: sceneModel
    // TODO: make VolumetricData size handling dynamic
    // so dont store this information
    // also change VolumetricData to VolumeData, 'cos it's shorter
    property vector3d storedVolumeSize: Qt.vector3d(0, 0, 0)

    function onDispatched(actionType, args) {
        // TODO: find a better way for action response than a switch/case
        switch (actionType) {
            case ActionTypes.addSourceNode:
                var path = "../views/nodes/SourceNodeView.qml";
                addSourceNode(args.uid, args.data, path);
                break;
            case ActionTypes.addSegmentNode:
                var path = "../views/nodes/SegmentNodeView.qml";
                addProcessNode(args.uid, path, "Segmentation");
                break;
            case ActionTypes.addAnalysisNode:
                var path = "../views/nodes/AnalysisNodeView.qml";
                addProcessNode(args.uid, path, "Analysis");
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
            case ActionTypes.applySegmentNode:
                applySegmentNode(args.uid, args.parameters);
                break;
            case ActionTypes.applyAnalysisNode:
                applyAnalysisNode(args.uid, args.parameters);
                break;
            case ActionTypes.saveAnalysisCsv:
                saveAnalysisCsv(args.uid, args.url);
                break;
            // TODO: cleanWorkspace
        }
    }

    function addSourceNode(uid, data, nodeViewPath) {
        // FIXME: nasty hack
        if (storedVolumeSize.x == 0) {
            storedVolumeSize = Qt.vector3d(data.width, data.height, data.depth);
        }

        var viewParams = defaultViewAttributes();

        var item = {
            uid: uid,
            nodeName: data.dataName,
            nodeViewPath: nodeViewPath,
            nodeViewParams: viewParams,
            nodeApplied: true,
            nodeParams: []
        };
        model.append(item);

        var maxDim = Math.max(data.width, data.height, data.depth);
        var sceneItem = {
            uid: uid,
            size: Qt.vector3d(data.width / maxDim, 
                data.height / maxDim, data.depth / maxDim),
            data: data,
            nodeViewParams: viewParams
        };
        sceneModel.append(sceneItem);
    }

    function addProcessNode(uid, nodeViewPath, nodeNameBase) {
        var item = {
            uid: uid,
            nodeName: nodeNameBase + " [node" + uid + "]",
            nodeViewPath: nodeViewPath,            
            nodeViewParams: defaultViewAttributes(),
            nodeApplied: false,
            nodeParams: []
        };
        model.append(item);
    }

    function getNode(uid) {
        for (var i = 0; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                return item;
            }
        }
        return null;
    }

    function getSceneNode(uid) {
        for (var i = 0; i < sceneModel.count; i++) {
            var item  = sceneModel.get(i);
            if (item.uid === uid) {
                return item;
            }
        }
        return null;
    }

    function applySegmentNode(uid, args) {
        var node = getNode(uid);
        if (node == null) {
            consol.log("no uid", uid);
            return;
        }

        var sceneNode = getSceneNode(uid);
        var outputData;
        if (sceneNode == null) {
            outputData = dataManager.newDataLike(args.data, node.nodeName);
        }

        dataManager.runSegmentation(args.data, outputData, 
            args.method, args.param0, args.param1);
    
        if (sceneNode == null) {
            var maxDim = Math.max(outputData.width, outputData.height, outputData.depth);
            var sceneItem = {
                uid: uid,
                size: Qt.vector3d(outputData.width / maxDim, 
                    outputData.height / maxDim, outputData.depth / maxDim),
                data: outputData,
                nodeViewParams: node.nodeViewParams
            };
            sceneModel.append(sceneItem);
        } else {
            sceneNode.lutDataMax = sceneNode.volumeData.dataLimits.y;
        }

        node.nodeApplied = true;
    }

    function applyAnalysisNode(uid, args) {
        var node = getNode(uid);
        if (node == null) {
            consol.log("no uid, analysis", uid);
            return;
        }

        var sceneNode = getSceneNode(uid);
        var outputData;
        if (sceneNode == null) {
            outputData = dataManager.newDataLike(args.segData0, node.nodeName);
        }
        node.nodeParams = dataManager.runAnalysis(args.data0, args.data1, args.segData0, args.segData1, outputData);
        if (sceneNode == null) {
            var maxDim = Math.max(outputData.width, outputData.height, outputData.depth);
            var sceneItem = {
                uid: uid,
                size: Qt.vector3d(outputData.width / maxDim, 
                    outputData.height / maxDim, outputData.depth / maxDim),
                data: outputData,
                nodeViewParams: node.nodeViewParams
            };
            sceneModel.append(sceneItem);
        } else {
            sceneNode.lutDataMax = sceneNode.volumeData.dataLimits.y;
        }

        node.nodeApplied = true;
    }

    function saveAnalysisCsv(uid, url) {
        var node = getNode(uid);
        if (node == null) {
            consol.log("saveAnalysisCsv: no uid, analysis", uid);
            return;
        }
        var table = [];
        for (var i = 0; i < node.nodeParams.count; ++i) {
            table.push({
                channelName: node.nodeParams.get(i).channelName,
                objectId: node.nodeParams.get(i).objectId,
                volume: node.nodeParams.get(i).volume,
                sumIntensity: node.nodeParams.get(i).sumIntensity,
                meanIntensity: node.nodeParams.get(i).meanIntensity,
                overlapRatio: node.nodeParams.get(i).overlapRatio,
                intersectingVolume: node.nodeParams.get(i).intersectingVolume,
                centerX: node.nodeParams.get(i).centerX,
                centerY: node.nodeParams.get(i).centerY,
                centerZ: node.nodeParams.get(i).centerZ,
                intensityWeightCenterX: node.nodeParams.get(i).intensityWeightCenterX,
                intensityWeightCenterY: node.nodeParams.get(i).intensityWeightCenterY,
                intensityWeightCenterZ: node.nodeParams.get(i).intensityWeightCenterZ
            });
        }
        console.log(url);
        dataManager.saveCsv(table, [
            "channelName",
            "objectId",
            "volume",
            "sumIntensity",
            "meanIntensity",
            "overlapRatio",
            "intersectingVolume",
            "centerX",
            "centerY",
            "centerZ",
            "intensityWeightCenterX",
            "intensityWeightCenterY",
            "intensityWeightCenterZ"], url);
    }

    function randomColor() {
        return Qt.rgba(Math.random(), Math.random(), Math.random(), 1.);
    }

    function defaultViewAttributes() {
        // TODO: debug: if visible is true here then you have to click twice on the checkbox to make it truly visible
        return {visible: false, lowCut: 0, highCut: 1, color: randomColor()};
    }

    function setViewParameters(uid, args) {
        for (var i = 0; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                item.nodeViewParams = args;
                break;
            }
        }

        for (var i = 0; i < sceneModel.count; i++) {
            var item  = sceneModel.get(i);
            if (item.uid === uid) {
                item.nodeViewParams = args;
                break;
            }
        }
    }

    function remove(uid) {
        for (var i = 0; i < model.count; i++) {
            var item  = model.get(i);
            if (item.uid === uid) {
                model.remove(i);
                break;
            }
        }

        for (var i = 0; i < sceneModel.count; i++) {
            var item  = sceneModel.get(i);
            if (item.uid === uid) {
                sceneModel.remove(i);
                break;
            }
        }
    }

    ListModel {
        id: model
    }

    ListModel {
        id: sceneModel
    }

    VolumetricDataManager {
        id: dataManager
        onStatusChanged: {
            console.log ('status change received', status);
            if (status === Component.Ready) {
                console.log(volumes.length);
                for (var i = 0; i < volumes.length; ++i) {
                    AppActions.addSourceNode(AppActions.generateUid(), volumes[i]);
                    console.log('addSourceNode', volumes[i]);
                }
            }
        }
    }
}
