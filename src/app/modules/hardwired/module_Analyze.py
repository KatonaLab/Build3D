# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import tagImage, analyze, apply_filter
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time


import numpy as np


FILTERS = ['voxelCount',
           'pixelsOnBorder', 'meanIntensity']
#'volume', 

def analyze_image(source, mask, settings, show=True, to_text=False):

        #Parameters to measure
        measurementList = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder']
        
        #TEMP###########TEMP##############TEMP#################TEMP
        multi_img_keys = ['meanIntensity','medianIntensity', 'skewness', 'kurtosis', 'variance','maximumPixel',
                               'maximumValue', 'minimumValue','minimumPixel','centerOfMass','standardDeviation',
                               'cumulativeIntensity','getWeightedElongation','getWeightedFlatness','getWeightedPrincipalAxes',
                               'getWeightedPrincipalMoments']
        
        for key in settings:
            if key in multi_img_keys:
                settings[str(key)+' in '+str(source.metadata['Name'])] = settings[key]
                del settings[key]

        #Channel 1:Tagging Image
        taggedImage, logText = tagImage(mask)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage, logText = analyze(taggedImage, imageList=[source], measurementInput=measurementList)
        print(logText)
        
        taggedImage, logText = apply_filter(taggedImage, filterDict=settings, removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        
        taggedImage.as_type(np.int64)#(taggedImage.metadata['Type'])
        
        return taggedImage


def read_params(filters=FILTERS):
    
    out_dict = {}
    out_dict = {'Source': Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Source_Image']),a3.inputs['Source_MetaData'] ),
                    'Mask':Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Mask_Image']),a3.inputs['Mask_MetaData'] )}

    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {}'.format( f, m)]

    out_dict['Settings'] = settings
     
    return out_dict    
    

def add_input_fields( filters=FILTERS):
    
    config = [a3.Input('Source_Image', a3.types.ImageFloat),
             a3.Input('Source_MetaData',  a3.types.GeneralPyType),
             a3.Input('Mask_Image', a3.types.ImageFloat),
             a3.Input('Mask_MetaData',  a3.types.GeneralPyType)]

    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {}'.format(f, m), a3.types.float)
                .setIntHint('min', 0)
                .setIntHint('max', 10000000)
                .setIntHint('default', 0 if m == 'min' else 10000000))

    return config

def module_main(ctx):
    
    #Inizialization
    #print(ctx.name())
    tstart = time.clock()
    print(SEPARATOR)
    print('Object analysis started!')
    
    #Read Parameters
    print('Reading input parameters!')
    params = read_params()
    
    params = read_params()
    
    output=analyze_image(params['Source'],
               params['Mask'],
               params['Settings'])

    #Change Name in metadata
    #output.metadata['Name']=params['Mask'].metadata['Name']+'_tagged'
    a3.outputs['Analyzed_Image'] = a3.MultiDimImageFloat_from_ndarray(output.array.astype(np.float))
    a3.outputs['Analyzed_DataBase']=output.database
    a3.outputs['Analyzed_MetaData']=output.metadata

    #Finalization
    tstop = time.clock()
    print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
    print('Object analysis was run successfully!')
    print(SEPARATOR)

    
config = [a3.Output('Analyzed_Image', a3.types.ImageFloat),  a3.Output('Analyzed_DataBase', a3.types.GeneralPyType), a3.Output('Analyzed_MetaData', a3.types.GeneralPyType)]
config.extend(add_input_fields())

a3.def_process_module(config, module_main)
