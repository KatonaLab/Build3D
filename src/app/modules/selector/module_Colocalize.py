# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""
import time
import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import VividImage
from modules.a3dc_modules.a3dc.interface import colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.core import filter_database
from modules.a3dc_modules.a3dc.utils import quote, SEPARATOR, error, warning
import os
import math
import sys

from modules.a3dc_modules.a3dc.multidimimage import from_multidimimage, to_multidimimage


CHFILTERS=['Ch1 totalOverlappingRatio', 'Ch2 totalOverlappingRatio','Ch1 colocalizationCount','Ch2 colocalizationCount']
OVLFILTERS=[ 'volume','Ch1 overlappingRatio','Ch2 overlappingRatio']

FILTERS = sorted(OVLFILTERS+CHFILTERS, key=str.lower)

def colocalize(ch1_img, ch2_img, ch1_settings, ch2_settings, ovl_settings, path, show=True, to_text=False):
    
    
    tagged_img_list=[ch1_img, ch2_img]
    print('Processing the following channels: '+ str([img.metadata['Name'] for img in tagged_img_list]))
    print('Filter settings: ' + str(ovl_settings))
    
    #Run colocaliyation analysis
    ovl_img, _=colocalization(tagged_img_list, overlappingFilter=ovl_settings)
    
    #Run filtering steps
    ch1_img.database=filter_database(ch1_img.database, ch1_settings, overwrite=True)
    ch2_img.database=filter_database(ch2_img.database, ch2_settings, overwrite=True)
    
    #Print number of objects to logText
    print('Number of Overlapping Objects: '+str(len(ovl_img.database['tag'])))            
    
    
    #Set path and filename
    outputPath=os.path.join(path, 'Output')
    if not os.path.exists(outputPath):
        os.makedirs(outputPath)            
    
    #Generate output filename
    filename_1=os.path.basename(ch1_img.metadata['Path'])
    filename_2=os.path.basename(ch1_img.metadata['Path'])
    if filename_1!=filename_2:
        
        basename=os.path.splitext(filename_1)[0]+'_'+os.path.splitext(filename_2)[0]
    else:
        basename=os.path.splitext(filename_1)[0]
        
    #Save databases
    print('Saving object dataBases!')
    file_name=basename+'_'+ch1_img.metadata['Name']+'_'+ch2_img.metadata['Name']
    
    #Get extension
    if to_text==True:
        extension=".txt"    
    else:
        extension=".xlsx"
    
    #If filename exists generate a neme that is not used
    i=1
    final_name=file_name
    while os.path.exists(os.path.join(outputPath, final_name+extension)):
        final_name=file_name+'_'+str('{:03d}'.format(i))
        i += 1
    if i!=1:
        file_name=final_name
        warning('Warning: Trying to save to file that already exist!! Data will be saved to '+file_name+extension)

    #Save data and give output path
    save_data([ch1_img, ch2_img ,ovl_img], path=outputPath, file_name=file_name, to_text=to_text)
    output_path=os.path.join(outputPath, file_name+extension)
    
    #Save images
    print('Saving output images!')
    name_img = basename+'_'+ch1_img.metadata['Name']#+"_tagged"
    save_image(ch1_img, outputPath, name_img)
    
    name_img =basename+'_'+ch2_img.metadata['Name']#+"_tagged"
    save_image(ch2_img, outputPath, name_img)
    
    name_img =basename+'_'+ch1_img.metadata['Name']+ "_" +ch2_img.metadata['Name']+ "_overlap"
    save_image(ovl_img, outputPath, name_img)            
    
    return ovl_img,  output_path  



def read_params(filters=FILTERS):
    
    out_dict = {}
    out_dict['Path']=os.path.dirname(a3.inputs['Path'].path)

    out_dict['Ch1 Image']=from_multidimimage(a3.inputs['Ch1 Image'],a3.inputs['Ch1 DataBase'])
    out_dict['Ch2 Image']=from_multidimimage(a3.inputs['Ch2 Image'],a3.inputs['Ch2 DataBase'])
    
    out_dict['to_text']=a3.inputs['Save to text']
    
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
            
            if prefix=='Ch1':
                ch1_settings[filter_key] = settings[key]
            if prefix=='Ch2':
                ch2_settings[filter_key] = settings[key]
    
    out_dict['Ch1'] = ch1_settings
    out_dict['Ch2'] = ch2_settings
            
    #Rename filter keys
    ovl_settings={}
    for key in OVLFILTERS:

        if key in settings:
            
            prefix=key.split('_', 1)[0]
            filter_key=key.split('_', 1)[-1]
            
            if prefix=='Ch1':
                ovl_settings[filter_key+' in '+str(a3.inputs['Ch1 MetaData']['Name'])] = settings[key]
     
            if prefix=='Ch2':
                ovl_settings[filter_key+' in '+str(a3.inputs['Ch2 MetaData']['Name'])] = settings[key]

        else:
            ovl_settings[key] = settings[key]
    
    out_dict['Ovl'] = ovl_settings    
    
    #out_dict['FileName']=a3.inputs['FileName']

    return out_dict    
    
def module_main(ctx):
    try:   
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Colocalization analysis started!')
        
        #Read Parameters
        print('Reading input parameters!')
        params = read_params()
        
        output=colocalize(params['Ch1 Image'],
                   params['Ch2 Image'],
                   params['Ch1'],
                   params['Ch2'],
                   params['Ovl'], 
                   params['Path'],
                   to_text=params['to_text'])
        
        a3.outputs['Overlapping Image'] = to_multidimimage(output[0])
        a3.outputs['Overlapping Binary'] = to_multidimimage(VividImage(output[0].array>0,output[0].metadata))
        a3.outputs['Overlapping DataBase'] =output[0].database
        
        path=a3.Url()
        path.path=output[1]
        a3.outputs['Overlapping Path'] = path

        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Object analysis was run successfully!')
        print(SEPARATOR)
        quote(verbose=True)  
        print(SEPARATOR)
    
    except IOError as e:
        print("Warning: Failed to write to file!!", file=sys.stderr)
        print(str(e), file=sys.stderr)
    
    except Exception as e:
        error("Error occured while executing "+str(ctx.name())+" !", exception=e)

 
def generate_config(filters=FILTERS):

    #Set Outputs and inputs
    config=[a3.Input('Path', a3.types.url),
        a3.Input('Ch1 Image', a3.types.ImageFloat), 
        a3.Input('Ch1 DataBase', a3.types.GeneralPyType), 
        a3.Input('Ch2 Image', a3.types.ImageFloat),
        a3.Input('Ch2 DataBase', a3.types.GeneralPyType),              
        a3.Output('Overlapping Image', a3.types.ImageFloat),
        a3.Output('Overlapping Binary', a3.types.ImageFloat),
        a3.Output('Overlapping DataBase', a3.types.GeneralPyType),
        a3.Output('Overlapping Path', a3.types.url)]    
    
    #Set parameters
    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {}'.format(f, m), a3.types.float)
                .setIntHint('default', 0 if m == 'min' else 10000000)
                .setFloatHint('default', 0 if m == 'min' else float(math.inf))
                .setFloatHint('unusedValue',0 if m == 'min' else float(math.inf)))
    
    config.append(a3.Parameter('Save to text', a3.types.bool)) 
    
    return config





a3.def_process_module(generate_config(), module_main)
