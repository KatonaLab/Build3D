var demoModules = [
{
    "uid": 1,
    "displayName": "Data Source 1",
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
