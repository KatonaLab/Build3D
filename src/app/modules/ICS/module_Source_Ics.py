# -*- coding: utf-8 -*-
"""
Created on Fri Sep 14 14:45:36 2018

@author: pongor.csaba
"""

import a3dc_module_interface as a3

from modules.a3dc_modules.external.PythImage import Image
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, print_line_by_line, warning
from modules.a3dc_modules.a3dc.imageclass import Image as Im
from modules.a3dc_modules.a3dc.a3image import a3image_to_image, image_to_a3image
import time, os, copy
import numpy as np
import inspect

from ast import literal_eval


def module_main(ctx):
    
    try:
        filename = a3.inputs['FileName'].path
        
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Loading the following image: ', filename)
        
        #Load and reshape image
        #img = Image.load(filename, file_type='ome')
        #img.reorder('XYZCT')
        
        img=a3image_to_image(a3.inputs['Image'])
        
        print(img.metadata)
        print(image_to_a3image(img).meta)
        print(a3.inputs['Image'].meta)
        
        

        '''
        ics_metadata=metadata_string_to_dict(meta_string)
        
      
        print(ics_metadata)#.keys())
        def ics_to_metadata(a3_image,ics_metadata):
            
            
            
            if is_ics_metadata(ics_metadata):
 
                
                #get image
                array=a3.MultiDimImageFloat_to_ndarray(a3_image)
                
                #Get Shape information
                ome_metadata={'SizeT': 1, 'SizeC':1, 'SizeZ':array.shape[-1], 'SizeX':array.shape[0], 'SizeY':array.shape[1]}
                ome_metadata['DimensionOrder']='XYZCT'
                
                channel=int(ics_metadata['channel'])
                
                #Add Type and path
                ome_metadata['Type']=ics_metadata['type']
                ome_metadata['Path']=ics_metadata['path']
                
                #Generate channel name
                if str('IcsGetSensorExcitationWavelength:'+str(channel)) in ics_metadata.keys(): 
                    ome_metadata['Name']='Probe_Ex_{}nm'.format(str(int(float(ics_metadata['IcsGetSensorExcitationWavelength:'+str(channel)]))))
                elif str('IcsGetSensorEmissionWavelength:'+str(channel)) in ics_metadata.keys():
                    ome_metadata['Name']='Probe_Em_{}nm'.format(str(int(float(ics_metadata['IcsGetSensorEmissionWavelength:'+str(channel)]))))
                else:
                    ome_metadata['Name']= 'Ch'+str(channel)
                
                #Get scale information in ome compatible format
                scale_dict={'IcsGetPosition:scale:x':'PhysicalSizeX','IcsGetPosition:scale:y':'PhysicalSizeY', 'IcsGetPosition:scale:z':'PhysicalSizeZ'}
                for key in scale_dict.keys():
                    if key in ics_metadata.keys():
                        ome_metadata[scale_dict[key]]=ics_metadata[key]
                
                #Get scale unit information in ome compatible format
                unit_dict={'IcsGetPosition:units:x':'PhysicalSizeXUnit','IcsGetPosition:units:y':'PhysicalSizeYUnit', 'IcsGetPosition:units:z':'PhysicalSizeZUnit'}
                for key in unit_dict.keys():
                    if key in ics_metadata.keys():
                        ome_metadata[unit_dict[key]]=ics_metadata[key]

        
            return ome_metadata    
        
        print(ics_to_metadata(img2, ics_metadata))
        
        

            #Name:['Ch0', 'Ch1', 'Ch2']
            
            #print('s')
        #channel: 0
        #normalized: false
        #path: C:/Users/pongor.csaba/Desktop/K32_bassoon_TH_vGaT_c02_cmle.ics
        #type: float
        

        a3_image=a3.MultiDimImageFloat_from_ndarray(img.image[0][0].astype(float))
        
       
        #dict to meta
        for key in img.metadata.keys():
            #a3_image.meta(key, str(img.metadata(key)))
            a3_image.meta.add(str(key), str(img.metadata[key]))
            #a3_image.meta.add(str(key), str(img.metadata(key)))
        print('2222222222222222')
        
        #meta to dict
        output_metadata={}
        for key in img.metadata.keys():
            
            if  a3_image.meta.has(str(key)):
         
                #print(key, a3_image.meta.get(str(key)))
            
                try:
                    output_metadata[str(key)]=literal_eval(a3_image.meta.get(str(key)))
                except:
                    output_metadata[str(key)]=a3_image.meta.get(str(key))
        

        
        #print(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Image']).shape)
        #print(str(a3.inputs['Image'].meta))
        #print(str(a3.inputs['Image'].meta.get('IcsGetPosition:units:x')))
        #print(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Image']).shape)
        #print(inspect.getmembers(OptionParser, predicate=inspect.ismethod))
        #print(a3_image.meta)
              
        
        
        
        
        
        
        
        
        
        
        #Create Output
        a3.outputs['Array'] = img.image
        a3.outputs['MetaData']=img.metadata
        
        #Add path and filename to metadata
        a3.outputs['MetaData']['Path']=os.path.dirname(filename)
        a3.outputs['MetaData']['FileName']=os.path.basename(filename)
        '''
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)

    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name())+" !",exception=e)

config = [a3.Input('FileName', a3.types.url),
          a3.Input('Image', a3.types.ImageFloat),
    a3.Output('Array', a3.types.GeneralPyType),
    a3.Output('MetaData', a3.types.GeneralPyType)]
    

a3.def_process_module(config, module_main)


