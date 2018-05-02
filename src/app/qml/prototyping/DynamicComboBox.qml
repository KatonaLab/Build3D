import QtQuick 2.0
import QtQuick.Controls 2.2

ComboBox {
    property var options: []
    property var activeItem: null
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

    onOptionsChanged: {
        var newModel = [];
        if (hasDefaultOption) {
            newModel.push({"name": defaultOptionName, "details": null});
        }

        options.forEach(function (item) {
            newModel.push({"name": optionNameGenerator(item), "details": item});
        });

        var idx = newModel.findIndex(function (item) {
            return (item.details === null && activeItem === null)
                    || (item.details !== null && activeItem !== null
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
}

