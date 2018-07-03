import a3dc_module_interface as a3


def module_main(ctx):
    print('Hello Parameters! This is {}'.format(ctx.name()))
    print('such string: {}'.format(a3.inputs['such string 🐕']))
    print('so filename: {}'.format(a3.inputs['so filename']))
    print('wow integer: {}'.format(a3.inputs['wow integer']))
    print('much float: {}'.format(a3.inputs['much float']))
    print('very bool: {}'.format(a3.inputs['very bool']))
    print('Bye 🐕')


config = [a3.Parameter('such string 🐕', a3.types.string),
          a3.Parameter('so filename', a3.types.string)
            .setBoolHint('file', True),
          a3.Parameter('wow integer', a3.types.int8)
            .setIntHint('min', 2)
            .setIntHint('max', 8)
            .setIntHint('stepSize', 3),
          a3.Parameter('much float', a3.types.float)
            .setFloatHint('min', -0.5)
            .setFloatHint('max', 1.72)
            .setFloatHint('stepSize', 0.1),
          a3.Parameter('very bool', a3.types.bool)]

a3.def_process_module(config, module_main)
