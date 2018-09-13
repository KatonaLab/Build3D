import a3dc_module_interface as a3


def module_main(ctx):
    a3.outputs['string output'] = a3.inputs['such string 🐕']
    a3.outputs['filename output'] = a3.inputs['so filename']
    a3.outputs['int output'] = a3.inputs['wow integer']
    a3.outputs['float output'] = a3.inputs['much float']
    a3.outputs['bool output'] = a3.inputs['very bool']
    a3.outputs['enum int output'] = a3.inputs['so enum'][1]
    a3.outputs['pyobject output'] = {'name': 'demo_dict',
                                     'value': a3.inputs['much float'],
                                     'list': [a3.inputs['wow integer'],
                                              a3.inputs['so enum'][1]]}

    print('Hello Parameters! This is {}'.format(ctx.name()))
    print('such string: {}'.format(a3.inputs['such string 🐕']))
    print('so filename: {}'.format(a3.inputs['so filename'].path))
    print('wow integer: {}'.format(a3.inputs['wow integer']))
    print('much float: {}'.format(a3.inputs['much float']))
    print('very bool: {}'.format(a3.inputs['very bool']))
    print('so enum: {}'.format(a3.inputs['so enum']))
    print('int16 in [-7, 9674]: {}'.format(a3.inputs['int16 in [-7, 9674]']))
    print('uint16 in [42, 72000]: {}'.format(a3.inputs['uint16 in [42, 72000]']))
    print('uint32 in [42, 72000]: {}'.format(a3.inputs['uint32 in [42, 72000]']))
    print('Bye 🐕')


config = [a3.Parameter('such string 🐕', a3.types.string),
          a3.Parameter('so filename', a3.types.url),
          a3.Parameter('wow integer', a3.types.int8)
            .setIntHint('min', 2)
            .setIntHint('unusedValue', 42)
            .setIntHint('max', 64),
          a3.Parameter('much float', a3.types.float)
            .setFloatHint('min', -0.5)
            .setFloatHint('unusedValue', -67.23)
            # .setFloatHint('max', 1.72)
            .setFloatHint('stepSize', 0.1),
          a3.Parameter('very bool', a3.types.bool),
          a3.Parameter('so enum', a3.types.enum)
            .setIntHint("option1", 0)
            .setIntHint("option2", 1)
            .setIntHint("option3", 42),
          a3.Parameter('int16 in [-7, 9674]', a3.types.int16)
            .setIntHint('min', -7)
            .setIntHint('max', 9674),
          a3.Parameter('uint16 in [42, 72000]', a3.types.uint16)
            .setIntHint('min', 42)
            .setIntHint('max', 72000),
          a3.Parameter('uint32 in [42, 72000]', a3.types.uint32)
            .setIntHint('min', 42)
            .setIntHint('max', 72000),
          a3.Output('string output', a3.types.string),
          a3.Output('filename output', a3.types.url),
          a3.Output('int output', a3.types.int8),
          a3.Output('float output', a3.types.float),
          a3.Output('bool output', a3.types.bool),
          a3.Output('enum int output', a3.types.int32),
          a3.Output('pyobject output', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
