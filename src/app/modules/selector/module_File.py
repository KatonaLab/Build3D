import a3dc_module_interface as a3

def module_main(_):

    a3.outputs['File']=a3.inputs['File']

config = [a3.Parameter('File', a3.types.url).setBoolHint('folder', False),
    a3.Output('File',  a3.types.url)]

a3.def_process_module(config, module_main)
