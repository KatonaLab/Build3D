import QtQuick 2.0
import QtQuick.Controls 2.2

ComboBox {
    property ListModel options: ListModel{}
    property var activeItem
    property var optionNameGenerator: function(item) { return item.name; }
    property var itemEqualsFunction: function(a, b) { return a === b; }
    property bool hasDefaultOption: false
    property string defaultOptionName: "<not selected>"

    signal optionSelected(var selected, var prev)
    signal optionRemoved(var prev)

    textRole: "name"

    onActivated: function (index) {
        var prev = activeItem;
        activeItem = model[index].details;
        optionSelected(activeItem, prev);
    }

    property real rnd: 0

    // TODO: make it a util function and import it
    // credits to: https://stackoverflow.com/questions/11353311/qml-items-children-list-deep-copy?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
    function deepCopy(p) {
        var c = {};
        for (var i in p) {
            if (typeof p[i] === 'object') {
                c[i] = (p[i].constructor === Array) ? [] : {};
                deepCopy(p[i], c[i]);
            } else {
                c[i] = p[i];
            }
        }
        return c;
    }

    function update() {

        if (options === null) {
            return;
        }

        var newModel = [];
        if (hasDefaultOption) {
            newModel.push({"name": defaultOptionName, "details": null});
        }

        for (var i = 0; i < options.count; ++i) {
            var item = options.get(i);
            // TODO: really important to deep copy, find out why and make the implementation cleaner
            newModel.push({"name": optionNameGenerator(item), "details": deepCopy(item)});
        }

        var idx = newModel.findIndex(function (item) {
            return (item.details === null && (activeItem === null || activeItem === undefined))
                    || (item.details !== null && activeItem !== null && activeItem !== undefined
                    && itemEqualsFunction(item.details, activeItem));
        });

        if (idx === -1 && hasDefaultOption) {
            idx = 0;
            optionRemoved(activeItem);
            activeItem = model[idx].details;
        }

        model = newModel;
        currentIndex = idx;
    }

    Connections {
        target: options
        onRowsInserted: update()
    }

    Connections {
        target: options
        onRowsRemoved: update()
    }

    Connections {
        target: options
        onDataChanged: update()
    }

    onOptionsChanged: {
        if (options !== null && options != undefined) {
            update();
        }
    }
}

