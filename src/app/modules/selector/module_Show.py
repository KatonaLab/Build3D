# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.utils import  os_open
from modules.a3dc_modules.a3dc.utils import error
from pathlib import Path

   
def module_main(ctx):
    try:   
        extensions=['.txt','.xlsx']
        path=a3.inputs['Path'].path
        
        if a3.inputs['Show'] and Path(path).suffix in extensions:
            os_open(path)
        
    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name)+" !",exception=e)




a3.def_process_module([a3.Input('Path', a3.types.url), a3.Parameter('Show', a3.types.bool)], module_main)
