pragma Singleton
import QtQuick 2.8

import "."

Item {

    property int nextUid: 0
    
    function generateUid() {
        return nextUid++;
    }

    function resetWorkspace() {
        AppDispatcher.dispatch(ActionTypes.resetWorkspace, {});
    }

    function importIcsFile(url) {
        AppDispatcher.dispatch(ActionTypes.importIcsFile, {url: url});
    }

    function addSourceNode(uid, data) {
        AppDispatcher.dispatch(ActionTypes.addSourceNode, {uid: uid, data: data});
    }

    function addSegmentNode(uid) {
        AppDispatcher.dispatch(ActionTypes.addSegmentNode, {uid: uid});
    }

    function addAnalysisNode(uid) {
        AppDispatcher.dispatch(ActionTypes.addAnalysisNode, {uid: uid});
    }

    function setNodeViewParameters(uid, viewParameters) {
        AppDispatcher.dispatch(ActionTypes.setNodeViewParameters,
            {uid: uid, parameters: viewParameters});
    }

    function setSegmentNodeParameters(uid, segmentParameters) {
        AppDispatcher.dispatch(ActionTypes.setSegmentNodeParameters,
            {uid: uid, parameters: segmentParameters});
    }

    function setAnalysisNodeParameters(uid, analysisParameters) {
        AppDispatcher.dispatch(ActionTypes.setAnalysisNodeParameters,
            {uid: uid, parameters: analysisParameters});
    }

    function removeNode(uid) {
        AppDispatcher.dispatch(ActionTypes.removeNode, {uid: uid});
    }

    function applyNodeParameters(uid) {
        AppDispatcher.dispatch(ActionTypes.applyNodeParameters, {uid: uid});
    }
}