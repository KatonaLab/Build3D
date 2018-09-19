# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 15:21:27 2018

@author: pongor.csaba
"""
import time
import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.core import filter_database
from modules.a3dc_modules.a3dc.utils import quote, SEPARATOR, error, warning
import os
import math
import sys

CHFILTERS=['Ch1 totalOverlappingRatio', 'Ch2 totalOverlappingRatio','Ch1 colocalizationCount','Ch2 colocalizationCount']
OVLFILTERS=[ 'volume','Ch1 overlappingRatio','Ch2 overlappingRatio']

FILTERS = sorted(OVLFILTERS+CHFILTERS, key=str.lower)

def save_database(database, path, file_name, to_text=False):

    #Save databases
    print('Saving object dataBases!')

    
    #Get extension
    if to_text==True:
        extension=".txt"    
    else:
        extension=".xlsx"
    
    #If filename exists generate a neme that is not used
    i=1
    final_name=file_name
    while os.path.exists(os.path.join(path, final_name+extension)):
        final_name=file_name+'_'+str('{:03d}'.format(i))
        i += 1
    if i!=0:
        file_name=final_name
        warning('Warning: Trying to save to file that already exist!! Data will be saved to '+file_name+extension)

    #Save data and give output path
    save_data(database, path=path, file_name=file_name, to_text=to_text)
     



def read_params(filters=FILTERS):
    
    out_dict = {}
    out_dict['Path']=os.path.dirname(a3.inputs['Path'].path)
    out_dict['DataBase']=a3.inputs['Path']
    out_dict['Save to text']= a3.inputs['Save to text']
    out_dict['Filename']=a3.inputs['Filename']

    return out_dict    
    
def module_main(ctx):
    try:   
        #Inizialization
        print(SEPARATOR)
        print('Saving Database!')
        
        #Read Parameters
        params = read_params()
        save_database(params['Database'], params['Path'], params['Filename'], to_text=params['Save to text'])
        
        #Finalization
        print('Object analysis was run successfully!')
        print(SEPARATOR)
    
    except IOError as e:
        print("Warning: Failed to write to file!!", file=sys.stderr)
        print(str(e), file=sys.stderr)
    
    except Exception as e:
        error("Error occured while executing "+str(ctx.name())+" !", exception=e)


#Set Outputs and inputs
config = [a3.Input('DataBase', a3.types.GeneralPyType),
          a3.Parameter('Filename', a3.types.string),
          a3.Parameter('Path',  a3.types.url).setBoolHint('folder', True),
        a3.Parameter('Save to text', a3.types.bool)]

a3.def_process_module(config, module_main)
