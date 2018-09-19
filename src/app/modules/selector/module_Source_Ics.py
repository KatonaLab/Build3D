# -*- coding: utf-8 -*-
"""
Created on Fri Sep 14 14:45:36 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.utils import error

def module_main(ctx):
    
    try:
        print(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Image']).shape)
        print(str(a3.inputs['Image'].meta))
        print(str(a3.inputs['Image'].meta.get('IcsGetPosition:units:x')))
    
    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name)+" !",exception=e)

config = [a3.Input('Image', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)