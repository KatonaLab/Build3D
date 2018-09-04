import a3dc_module_interface as a3


def module_main(ctx):
    print('Hello Consumer Demo! This is {}'.format(ctx.name()))
    print('string input: {}'.format(a3.inputs['string input']))
    print('filename input: {}'.format(a3.inputs['filename input'].path))
    print('int input: {}'.format(a3.inputs['int input']))
    print('float input: {}'.format(a3.inputs['float input']))
    print('bool input: {}'.format(a3.inputs['bool input']))
    print('enum int input: {}'.format(a3.inputs['enum int input']))
    print('pyobject input: {}'.format(a3.inputs['pyobject input']))
    print('done.')


config = [
    a3.Input('string input', a3.types.string),
    a3.Input('filename input', a3.types.url),
    a3.Input('int input', a3.types.int8),
    a3.Input('float input', a3.types.float),
    a3.Input('bool input', a3.types.bool),
    a3.Input('enum int input', a3.types.int32),
    a3.Input('pyobject input', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
