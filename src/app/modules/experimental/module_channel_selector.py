import a3dc_module_interface as a3


def module_main(ctx):
    input_channels = (
        a3.inputs['channel 1'], a3.inputs['channel 2'],
        a3.inputs['channel 3'], a3.inputs['channel 4'])

    sel_1 = a3.inputs['first selected channel'][1]
    sel_2 = a3.inputs['second selected channel'][1]
    print(sel_1, sel_2)
    if 0 <= sel_1 < 4:
        a3.outputs['first selected channel'] = input_channels[sel_1]
    else:
        raise RuntimeError('invalid value for \'first selected channel\': {}'
                           .format(sel_1))

    if 0 <= sel_2 < 4:
        a3.outputs['second selected channel'] = input_channels[sel_2]
    else:
        raise RuntimeError('invalid value for \'second selected channel\': {}'
                           .format(sel_1))


config = [a3.Input('channel 1', a3.types.ImageFloat),
          a3.Input('channel 2', a3.types.ImageFloat),
          a3.Input('channel 3', a3.types.ImageFloat),
          a3.Input('channel 4', a3.types.ImageFloat),
          a3.Parameter('first selected channel', a3.types.enum)
            .setIntHint("channel 1", 0)
            .setIntHint("channel 2", 1)
            .setIntHint("channel 3", 2)
            .setIntHint("channel 4", 3),
          a3.Parameter('second selected channel', a3.types.enum)
            .setIntHint("channel 1", 0)
            .setIntHint("channel 2", 1)
            .setIntHint("channel 3", 2)
            .setIntHint("channel 4", 3),
          a3.Output('first channel', a3.types.ImageFloat),
          a3.Output('second channel', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)
