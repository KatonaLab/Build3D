var type1PortModel = [
    {"displayName": "foo port type1", "targetUid": 7, "targetOutput": 1},
    {"displayName": "bar port type1", "targetUid": 5, "targetOutput": 2},
    {"displayName": "abc port type1", "targetUid": 5, "targetOutput": 4}
]

var type2PortModel = [
    {"displayName": "foo port type2", "targetUid": 17, "targetOutput": 11},
    {"displayName": "bar port type2", "targetUid": 15, "targetOutput": 12},
    {"displayName": "abc port type2", "targetUid": 15, "targetOutput": 14}
]

var demoModules = [
{
    "uid": 1,
    "displayName": "Data Source 1",
    "inputs": [
        {"displayName": "first input", "portId": 0, "optionsList": 0},
        {"displayName": "second input", "portId": 1, "optionsList": 0},
        {"displayName": "third input", "portId": 2, "optionsList": 1}
    ],
    "parameters": [
        {"displayName": "amount of foo", "index": 3, "type": "int", "hint": "slider", "from": 2, "to": 8},
        {"displayName": "velocity of bar", "index": 4, "type": "float"},
        {"displayName": "density of baz", "index": 5, "type": "bool"},
        {"displayName": "kaboom", "index": 6, "type": "mysterious-type"}
    ],
    "outputs": [
        {"displayName": "first output", "index": 7, "type": "float-image"},
        {"displayName": "second output", "index": 8, "type": "float"},
        {"displayName": "third output", "index": 9, "type": "int"}
    ]
}]
