# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""
import traceback, time
import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.core import filter_database
from modules.a3dc_modules.a3dc.utils import quote, SEPARATOR
import os

CHFILTERS=['ch1_totalOverlappingRatio', 'ch2_totalOverlappingRatio','ch2_colocalizationCount','ch1_colocalizationCount']
OVLFILTERS=[ 'voxelCount','pixelsOnBorder','ch1_overlappingRatio','ch2_overlappingRatio']
#'volume',
FILTERS = OVLFILTERS+CHFILTERS

def colocalize(ch1_img, ch2_img, ch1_settings, ch2_settings, ovl_settings, path, show=True, to_text=False):
        
        #Create list of images
        tagged_img_list=[ch1_img, ch2_img]

        
        try:

            print('Processing the following channels: '+ str([img.metadata['Name'] for img in tagged_img_list]))
            
            # Add Filter settings
            print('Filter settings: ' + str(ovl_settings))
            ovl_img, _=colocalization(tagged_img_list, overlappingFilter=ovl_settings)
            
            ch1_img.database=filter_database(ch1_img.database, ch1_settings, overwrite=True)
            ch2_img.database=filter_database(ch2_img.database, ch2_settings, overwrite=True)
        
            #Print number of objects to logText
            print('Number of Overlapping Objects: '+str(len(ovl_img.database['tag'])))            

            
            #Set path and filename
            outputPath=os.path.join(path, 'Output')
            if not os.path.exists(outputPath):
                os.makedirs(outputPath)            
            
         
            if ch1_img.metadata['FileName']!=ch2_img.metadata['FileName']:
                
                basename=os.path.splitext(ch1_img.metadata['FileName'])[0]+'_'+os.path.splitext(ch2_img.metadata['FileName'][0])
            else:
                basename=os.path.splitext(ch1_img.metadata['FileName'])[0]
                
            #Save databases
            print('Saving object dataBases to xlsx or text!')
            name=basename+'_'+ch1_img.metadata['Name']+'_'+ch2_img.metadata['Name']
            if to_text==True:
                file_name=name+'.txt'    
            else:
                file_name=name+'.xlsx'
            save_data([ch1_img, ch2_img ,ovl_img], path=outputPath, file_name=file_name, to_text=to_text)
        
            #Save images
            print('Saving output images!')
            name = basename+'_'+ch1_img.metadata['Name']#+"_tagged"
            save_image(ch1_img, outputPath, name)
            
            name =basename+'_'+ch2_img.metadata['Name']#+"_tagged"
            save_image(ch2_img, outputPath, name)
            
            name =basename+'_'+ch1_img.metadata['Name']+ "_" +ch2_img.metadata['Name']+ "_overlap"
            save_image(ovl_img, outputPath, name)
                        
            #Show file
            #if show==True:    
                #os_open(os.path.join(outputPath, file_name))
        
        except Exception as e:
            traceback.print_exc()
            raise Exception("Error occured durig colocalization!",e)
        
        return ovl_img


def read_params(filters=FILTERS):
    
    out_dict = {}
    out_dict['Path']=os.path.dirname(a3.inputs['Path'].path)

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

    return out_dict    
    
def module_main(ctx):
       
    #Inizialization
    tstart = time.clock()
    print(SEPARATOR)
    print('Colocalization analysis started!')
    
    #Read Parameters
    print('Reading input parameters!')
    params = read_params()
    
    output=colocalize(params['Ch1_Image'],
               params['Ch2_Image'],
               params['Ch1'],
               params['Ch2'],
               params['Ovl'], 
               params['Path'])

    a3.outputs['Overlapping_Image'] = a3.MultiDimImageFloat_from_ndarray((output.array>0).astype(float))
    a3.outputs['Overlapping_MetaData'] =output.metadata
    a3.outputs['Overlapping_DataBase']=output.database
  
    #Finalization
    tstop = time.clock()
    print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
    print('Object analysis was run successfully!')
    print(SEPARATOR)
    quote(verbose=True)  
    print(SEPARATOR)
    
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

config=[a3.Input('Path', a3.types.url),
        a3.Input('Ch1_Image', a3.types.ImageFloat), 
        a3.Input('Ch1_MetaData', a3.types.GeneralPyType),
        a3.Input('Ch1_DataBase', a3.types.GeneralPyType), 
        a3.Input('Ch2_Image', a3.types.ImageFloat),
        a3.Input('Ch2_MetaData', a3.types.GeneralPyType), 
        a3.Input('Ch2_DataBase', a3.types.GeneralPyType),
        
        a3.Output('Overlapping_Image', a3.types.ImageFloat),
        a3.Output('Overlapping_MetaData', a3.types.GeneralPyType),
        a3.Output('Overlapping_DataBase', a3.types.GeneralPyType)]


config=add_input_fields(config)

a3.def_process_module(config, module_main)
