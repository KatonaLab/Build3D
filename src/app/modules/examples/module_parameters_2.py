# -*- coding: utf-8 -*-
"""
Created on Fri Sep 14 09:02:31 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3


def module_main(ctx):

    a3.outputs['float output'] = a3.inputs['much float']



    print('much float: {}'.format(a3.inputs['much float']))


config = [a3.Parameter('much float', a3.types.float)
            .setIntHint('min', -5)
            .setIntHint('unusedValue', 1000),
          a3.Parameter('very bool', a3.types.bool).setBoolHint('s', False),
            # .setFloatHint('max', 1.72)
            #.setFloatHint('stepSize', 0.1),
          a3.Output('float output', a3.types.float)]

a3.def_process_module(config, module_main)