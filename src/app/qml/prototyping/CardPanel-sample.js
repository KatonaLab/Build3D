var demoModules = [
{
    "uid": 1,
    "displayName": "Data Source 1",
    "inputs": [
        {"displayName": "first input", "index": 0},
        {"displayName": "second input", "index": 1},
        {"displayName": "third input", "index": 2}
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
},
{
    "uid": 2,
    "displayName": "Data Processing",
    "inputs": [
        {"displayName": "Fancy Input", "type": "volume"}
    ],
    "parameters": [
        {"displayName": "Do Other Things", "type": "button"},
        {"displayName": "Edit Other Things", "type": "edit"},
        {"displayName": "Select Something",
            "type": "combobox",
            "options": [
                {"text": "Red"},
                {"text": "Green"},
                {"text": "Blue"},
                {"text": "Violet"}
            ]
        },
        {"displayName": "Range That Value", "type": "range"},
        {"displayName": "Switch That Switch", "type": "switch"},
        {"displayName": "Switch That Too", "type": "switch"}
    ],
    "outputs": []
},
{
    "uid": 3,
    "displayName": "Data Processing 3",
    "inputs": [
        {"displayName": "Fancy Input", "type": "volume"}
    ],
    "parameters": [
        {"displayName": "Do Other Things", "type": "button"},
        {"displayName": "Edit Other Things", "type": "edit"},
        {"displayName": "Select Something",
            "type": "combobox",
            "options": [
                {"text": "Red"},
                {"text": "Green"},
                {"text": "Blue"},
                {"text": "Violet"}
            ]
        },
        {"displayName": "Range That Value", "type": "range"},
        {"displayName": "Switch That Switch", "type": "switch"},
        {"displayName": "Switch That Too", "type": "switch"}
    ],
    "outputs": []
},
{
    "uid": 4,
    "displayName": "Data Source 4",
    "inputs": [],
    "parameters": [
        {"displayName": "Do Something", "type": "button"},
        {"displayName": "Edit Something", "type": "edit"},
        {"displayName": "Slide That Value", "type": "slider"}
    ],
    "outputs": [
        {"displayName": "Fancy Output", "type": "volume"},
        {"displayName": "Fancy Number Output", "type": "number"}
    ]
}
]
