# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""
import traceback, time
import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import tagImage, analyze, apply_filter, colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.core import filter_database, colocalization_connectivity, colocalization_analysis 
from modules.a3dc_modules.a3dc.utils import os_open, quote

import os
import numpy as np



CHFILTERS=['ch1_totalOverlappingRatio', 'ch2_totalOverlappingRatio','ch2_colocalizationCount','ch1_colocalizationCount']
OVLFILTERS=['volume', 'voxelCount','pixelsOnBorder','ch1_overlappingRatio','ch2_overlappingRatio']

FILTERS = OVLFILTERS+CHFILTERS

####################################################Interface to call from C++####################################################
def colocalize(ch1_img, ch2_img, ch1_settings, ch2_settings, ovl_settings, path=None, show=True, to_text=False):
        
        tagged_img_list=[ch1_img, ch2_img]
        
        
        #Set path
        if path==None:
            outputPath="D:\Playground"
        else:
            outputPath=path
        if not os.path.exists(outputPath):
            os.makedirs(outputPath)
        
        #############################################################################################################################
        ###################################################Colocalization############################################################
        #############################################################################################################################
        # Start timingsourceDictionayList
        tstart = time.clock()
    
        try:
    
            # Creatre LogText
            print('\nColocalization analysis started using: ')
            for img in tagged_img_list:
                print('\t ' + str(img.metadata['Name']))
    
            # Add Filter settings
            print('\n\tFilter settings: ' + str(ovl_settings).replace('{', ' ').replace('}', ' '))

            # Determine connectivity data
            ovl_img = colocalization_connectivity(tagged_img_list)
            print(ovl_img.database.keys())
            
            
            # Filter database and image
            #overlappingImage, _ = apply_filter(overlappingImage, ovl_settings)
            ovl_img.database=filter_database(ovl_img.database, ovl_settings, overwrite=False)
         
            # Analyze colocalization
            ovl_img, _ = colocalization_analysis(tagged_img_list, ovl_img)
            print(ch1_img.database.keys())
            ch1_img.database=filter_database(ch1_img.database, ch1_settings, overwrite=False)
            print(ch1_img.database.keys())
            
            ch2_img.database=filter_database(ch2_img.database, ch2_settings, overwrite=False)
            print(ch2_img.database.keys())

    
            # Finish timing and add to logText
            tstop = time.clock()
            
            #Print number of objects to logText
            print('\n\tNumber of Overlapping Objects: '+str(len(ovl_img.database['tag'])))            
            print('\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! ')
            


            #Save databases
            print('\nSaving object dataBases to xlsx or text!')
            name=ch1_img.metadata['Name']+'_'+ch2_img.metadata['Name']
            if to_text==True:
                file_name=name+'.txt'    
            else:
                file_name=name+'.xlsx'
            save_data([ch1_img, ch2_img ,ovl_img], path=outputPath, file_name=file_name, to_text=to_text)
        
            #Save images
            print('\nSaving output images!')
            name = ch1_img.metadata['Name']#+"_tagged"
            save_image(ch1_img, outputPath, name)
            
            name = ch2_img.metadata['Name']#+"_tagged"
            save_image(ch2_img, outputPath, name)
            
            name = ch1_img.metadata['Name']+ "_" +ch2_img.metadata['Name']+ "_overlap"
            save_image(ovl_img, outputPath, name)
                        
            #Show file
            #print(str(outputPath))
            #print(os.path.join(outputPath, file_name))
            #print(os.path.isfile(os.path.join(outputPath, file_name)))
            #show=True
            #if show==True:    
                #os_open(os.path.join(outputPath, file_name))
            
            print('Colocalization analysis was run successfully!')
            print("\n%s\n" % str(quote()))
        

        except Exception as e:
            traceback.print_exc()
            raise Exception("Error occured durig colocalization!",e)
        
        return ovl_img


def read_params(filters=FILTERS):
    
    out_dict = {}

    out_dict['Ch1_Image']=Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Ch1_Image']), a3.inputs['Ch1_MetaData'], a3.inputs['Ch1_DataBase'])
    out_dict['Ch2_Image']=Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Ch2_Image']), a3.inputs['Ch2_MetaData'], a3.inputs['Ch2_DataBase'])
       
    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {}'.format( f, m)]

    
    ch1_settings={}
    ch2_settings={}
    for key in CHFILTERS:
        if key in settings:
            prefix=key.split('_', 1)[0]
            filter_key=key.split('_', 1)[-1]
            
            if prefix=='ch1':
                ch1_settings[filter_key] = settings[key]
            if prefix=='ch2':
                ch2_settings[filter_key] = settings[key]
    
    out_dict['Ch1'] = ch1_settings
    out_dict['Ch2'] = ch2_settings
            
    #Rename filter keys
    ovl_settings={}
    for key in OVLFILTERS:

        if key in settings:
            
            prefix=key.split('_', 1)[0]
            filter_key=key.split('_', 1)[-1]
            
            if prefix=='ch1':
                ovl_settings[filter_key+' in '+str(a3.inputs['Ch1_MetaData']['Name'])] = settings[key]
     
            if prefix=='ch2':
                ovl_settings[filter_key+' in '+str(a3.inputs['Ch2_MetaData']['Name'])] = settings[key]

        else:
            ovl_settings[key] = settings[key]
    out_dict['Ovl'] = ovl_settings    
    
    #out_dict['FileName']=a3.inputs['FileName']
    out_dict['FileName']=None

    return out_dict    
    
def module_main(ctx):
    
    params = read_params()
    
    output=colocalize(params['Ch1_Image'],
               params['Ch2_Image'],
               params['Ch1'],
               params['Ch2'],
               params['Ovl'], 
               params['FileName'])

    a3.outputs['Analyzed_Image'] = a3.MultiDimImageFloat_from_ndarray(output.array)
    a3.outputs['Analyzed_DataBase']=output.database
  

def add_input_fields(config, filters=FILTERS):
    
    #config.append(a3.Parameter('FileName', a3.types.url))
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
        a3.Output('Analyzed_Image', a3.types.GeneralPyType),
        a3.Output('Analyzed_DataBase', a3.types.GeneralPyType)]

config=add_input_fields(config)

a3.def_process_module(config, module_main)
