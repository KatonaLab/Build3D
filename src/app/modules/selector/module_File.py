import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time

def module_main(_):

    a3.outputs['File']=a3.inputs['File']

config = [
    a3.Parameter('File', a3.types.url).setBoolHint('folder', False),
    a3.Output('File',  a3.types.url)]

a3.def_process_module(config, module_main)
