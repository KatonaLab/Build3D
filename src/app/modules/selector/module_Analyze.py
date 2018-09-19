# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import tagImage, analyze, apply_filter
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error
import time
import math
import sys



import numpy as np


FILTERS = ['volume', 'meanIntensity']
          
#'volume', 

def analyze_image(source, mask, settings, removeFiltered=False):

    print('Processing the following channels: '+ str(source.metadata['Name']))
    print('Filter settings: '+str(settings))
    
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

    #Tagging Image
    print('Running connected components!')
    taggedImage, _ = tagImage(mask)
    
    # Analysis and Filtering of objects
    print('Analyzing tagged image!')
    taggedImage, _ = analyze(taggedImage, imageList=[source], measurementInput=measurementList)
    
    print('Filtering object database!')
    taggedImage, _ = apply_filter(taggedImage, filter_dict=settings, remove_filtered=removeFiltered)#{'tag':{'min': 2, 'max': 40}}
        

    
    return taggedImage


def read_params(filters=FILTERS):
    
    out_dict = {'Source': Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Source image']),a3.inputs['Source metadata'] ),
                    'Mask':Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Mask image']),a3.inputs['Mask metadata'] )}

    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {}'.format( f, m)]
    
    if a3.inputs['Exclude bordering objects']:       
        settings['pixelsOnBorder']={'min': 1, 'max':float(math.inf)}

    if a3.inputs['Use physical dimensions'] and ('volume' in settings.keys()):
        
        #Check if physical size metadata is available  if any is missing raise Exeption
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        missing_size=[s for s in size_list if s not in out_dict['Source'].metadata.keys()]
        if len(missing_size)!=0:
            raise Exception('Missing :'+str(missing_size)+'! Unable to carry out analysis!')

        #Check if unit metadata is available, default Unit is um!!!!!!!!
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in out_dict['Source'].metadata.keys()]
        if len(missing_size)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)        
        
        #Set default Unit values is not in metadata
        #Remember that if unit value is missing an exception is raised
        for un in missing_unit:
            out_dict['Source'].metadata[un]='um'
            out_dict['Mask'].metadata[un]='um'
        
        print('Physical voxel volume is : '
              +str(out_dict['Source'].metadata['PhysicalSizeX']*out_dict['Source'].metadata['PhysicalSizeY']*out_dict['Source'].metadata['PhysicalSizeZ'])
              +' '+out_dict['Source'].metadata['PhysicalSizeXUnit']+'*'+out_dict['Source'].metadata['PhysicalSizeYUnit']+'*'+out_dict['Source'].metadata['PhysicalSizeZUnit'])
        
  
    else:
        settings['voxelCount'] = settings.pop('volume')
    
    out_dict['Settings'] = settings
    
    out_dict['removeFiltered']=a3.inputs['Remove filtered objects']

    return out_dict    
    

def generate_config(filters=FILTERS):
    
    #Set Outputs and inputs
    config = [a3.Output('Analyzed image', a3.types.ImageFloat),  
              a3.Output('Analyzed database', a3.types.GeneralPyType), 
              a3.Output('Analyzed metadata', a3.types.GeneralPyType),
              a3.Input('Source image', a3.types.ImageFloat),
             a3.Input('Source metadata',  a3.types.GeneralPyType),
             a3.Input('Mask image', a3.types.ImageFloat),
             a3.Input('Mask metadata',  a3.types.GeneralPyType)]

    #Set parameters 
    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {}'.format(f, m), a3.types.float)
                .setFloatHint('default', 0 if m == 'min' else float(math.inf))
                .setFloatHint('unusedValue',0 if m == 'min' else float(math.inf)))
    
    switch_list=[a3.Parameter('Remove filtered objects', a3.types.bool).setBoolHint("default", False),
                 a3.Parameter('Exclude bordering objects', a3.types.bool).setBoolHint("default", False),
                 a3.Parameter('Use physical dimensions', a3.types.bool).setBoolHint("default", False)]
    config.extend(switch_list)

    
    return config

def module_main(ctx):
    try:
        #Inizialization
        #print(ctx.name())
        tstart = time.clock()
        print(SEPARATOR)
        print('Object analysis started!')
        
        #Read Parameters
        print('Reading input parameters!')
        params = read_params()
        
        output=analyze_image(params['Source'],
                   params['Mask'],
                   params['Settings'],
                   params['removeFiltered'])
        
        #Change Name in metadata
        #output.metadata['Name']=params['Mask'].metadata['Name']+'_tagged'
        a3.outputs['Analyzed image'] = a3.MultiDimImageFloat_from_ndarray(output.array.astype(np.float))
        
        a3.outputs['Analyzed database']=output.database
        a3.outputs['Analyzed metadata']=output.metadata
        
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Object analysis was run successfully!')
        print(SEPARATOR)

    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name)+" !",exception=e)
    




a3.def_process_module(generate_config(), module_main)
