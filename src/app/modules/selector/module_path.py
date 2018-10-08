import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.utils import error
import os

def module_main(ctx):

    if os.path.exists(a3.inputs['Path'].path):
        a3.outputs['Path']=a3.inputs['Path']
    else:
        error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"'! Invalid path!!")

config = [a3.Parameter('Path', a3.types.url),
    a3.Output('Path',  a3.types.url)]

a3.def_process_module(config, module_main)
