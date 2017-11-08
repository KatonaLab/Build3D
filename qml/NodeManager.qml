import QtQuick 2.8

QtObject {
    id: root

    signal sourceNodeAdded(string str)
    signal segmentNodeAdded(string str)
    signal analysisNodeAdded(string str)

    function createSourceNode()
    {
        sourceNodeAdded("source node");
    }

    function createSegmentNode()
    {
        segmentNodeAdded("segment node");
    }

    function createAnalysisNode()
    {
        analysisNodeAdded("analysis node");
    }

    function getSegmentNodes()
    {

    }

    function getSourceNodes()
    {

    }
}