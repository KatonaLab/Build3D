import a3dc_module_interface as a3
from itertools import product

def module_main(_):
    pass

params = [{'name': 'int',
           'type': a3.types.int32,
           'min': -6,
           'max': 19,
           'default': 7,
           'unusedValue': 2,
           'setter': a3.Parameter.setIntHint},
          {'name': 'float',
           'type': a3.types.float,
           'min': -2.34,
           'max': 45.9435,
           'default': 7.32,
           'unusedValue': 2.9483,
           'setter': a3.Parameter.setFloatHint}]

config = []
for a in params:
    for b in product([0, 1], repeat=4):
        min_str = 'min:' + str(a['min']) if b[0] else ''
        max_str = 'max:' + str(a['max']) if b[1] else ''
        def_str = 'default:' + str(a['default']) if b[2] else ''
        unused_str = 'unusedValue:' + str(a['unusedValue']) if b[3] else ''
        name = '{} {} {} {} {}'.format(a['name'], min_str, max_str, def_str, unused_str)
        x = a3.Parameter(name, a['type'])
        if b[0]:
            a['setter'](x, 'min', a['min'])
        if b[1]:
            a['setter'](x, 'max', a['max'])
        if b[2]:
            a['setter'](x, 'default', a['default'])
        if b[3]:
            a['setter'](x, 'unusedValue', a['unusedValue'])
        config.append(x)


a3.def_process_module(config, module_main)
