import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time

def module_main(_):

    a3.outputs['Path']=a3.inputs['Path']

config = [
    a3.Parameter('Path', a3.types.url),
    a3.Output('Path',  a3.types.url)]

a3.def_process_module(config, module_main)
