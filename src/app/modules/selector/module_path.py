import a3dc_module_interface as a3

def module_main(_):

    a3.outputs['Path']=a3.inputs['Path']

config = [a3.Parameter('Path', a3.types.url),
    a3.Output('Path',  a3.types.url)]

a3.def_process_module(config, module_main)
