import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.utils import error
import os

def module_main(ctx):

    if os.path.isfile(a3.inputs['File'].path):
        a3.outputs['File']=a3.inputs['File']
    else:
        error("Error occured while executing "+str(ctx.name())+" ! Invalid file path!!")


config = [a3.Parameter('File', a3.types.url).setBoolHint('folder', False),
    a3.Output('File',  a3.types.url)]

a3.def_process_module(config, module_main)
