# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import tagImage, analyze, apply_filter, colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.utils import os_open, quote

import os
import numpy as np



FILTERS = ['volume', 'voxelCount','pixelsOnBorder', 'ch1_colocalizationCount','ch1_overlappingRatio', 
           'ch1_totalOverlappingRatio', 'ch2_colocalizationCount','ch2_overlappingRatio', 'ch2_totalOverlappingRatio']
####################################################Interface to call from C++####################################################
def colocalize(ch1Img, ch2Img, ovlSettings, path, show=True, to_text=False):

        #Set path
        outputPath=path
        if not os.path.exists(outputPath):
            os.makedirs(outputPath)
        
        #############################################################################################################################
        ###################################################Colocalization############################################################
        #############################################################################################################################
        overlappingImage, taggedImageList, logText = colocalization( [ch1Img, ch2Img], overlappingFilter=ovlSettings, removeFiltered=False)
        print(logText)
        
        name = ch1Img.metadata['Name']+"_tagged"
        save_image(ch1Img, outputPath, name)
        
        name = ch2Img.metadata['Name']+"_tagged"
        save_image(ch2Img, outputPath, name)
        
        name = ch1Img.metadata['Name']+ "_" +ch2Img.metadata['Name']+ "_overlap"
        save_image(overlappingImage, outputPath, name)
        
        logText='\nSaving object dataBases to xlsx or text!'
        print(logText)

        #Save File
        name=ch1Img.metadata['Name']+'_'+ch2Img.metadata['Name']
        if to_text==True:
            file_name=name+'.txt'    
        else:
            file_name=name+'.xlsx'
        save_data([ch1Img, ch2Img ,overlappingImage], path=outputPath, file_name=file_name, to_text=to_text)

        
        #Show file
        #if show==True:
            
            #os_open(os.path.join(outputPath, file_name))
        print('Colocalization analysis was run successfully!')
        print("\n%s\n" % str(quote()))
        
        return overlappingImage


def read_params(filters=FILTERS):
    
    out_dict = {}

    out_dict['Ch1_Image']=Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Ch1_Image']), a3.inputs['Ch1_MetaData'], a3.inputs['Ch1_DataBase'])
    out_dict['Ch2_Image']=Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Ch2_Image']), a3.inputs['Ch2_MetaData'], a3.inputs['Ch2_DataBase'])
       
    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {}'.format( f, m)]

    out_dict['Settings'] = settings
    out_dict['FileName']=a3.inputs['FileName']

    return out_dict    
    
def module_main(ctx):
    
    params = read_params()
    
    output=colocalize(params['Ch1_Image'],
               params['Ch2_Image'],
               params['Settings'], params['FileName'])

    a3.outputs['Analyzed_Image'] = a3.MultiDimImageFloat_from_ndarray(output.array.astype(np.float) / np.amax(output.array.astype(np.float)))
    a3.outputs['Analyzed_DataBased']=output.database
  

def add_input_fields(config, filters=FILTERS):
    
    config.append(a3.Parameter('FileName', a3.types.url))
    
    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {}'.format(f, m), a3.types.float)
                .setIntHint('min', 0)
                .setIntHint('max', 10000000)
                .setIntHint('default', 0 if m == 'min' else 10000000))
    
    
    
    return config

config=[a3.Input('Ch1_Image', a3.types.ImageFloat), 
        a3.Input('Ch1_MetaData', a3.types.GeneralPyType),
        a3.Input('Ch1_DataBase', a3.types.GeneralPyType), 
        a3.Input('Ch2_Image', a3.types.ImageFloat),
        a3.Input('Ch2_MetaData', a3.types.GeneralPyType), 
        a3.Input('Ch2_DataBase', a3.types.GeneralPyType),
        a3.Output('Overlapping_Image', a3.types.ImageFloat),
        a3.Output('Overlapping_MetaData', a3.types.GeneralPyType),
        a3.Output('Analyzed_DataBase', a3.types.GeneralPyType)]

config=add_input_fields(config)

a3.def_process_module(config, module_main)
