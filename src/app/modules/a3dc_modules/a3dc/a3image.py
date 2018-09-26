# -*- coding: utf-8 -*-
"""
Created on Wed Sep 26 07:11:59 2018

@author: pongor.csaba

This file contains functions needed to work with A3-DC a3dc_module_interface
image types and the ICS reading module. The ics loading module reads ics
metadata and gives it a name based on the function that reads the metadata with 
subelements divided by ':'. ICS files have required keys (see Dean P, Mascio L,
Ow D, Sudar D, Mullikin J.,Proposed standard for image cytometry data files., 
Cytometry. 1990;11(5):561-9.) if 'IcsGetCoordinateSystem' OR 
'IcsGetSignificantBits' is among the metadata keys the metadata is taken as 
ics metadata!!!!!. Currently only one channel and first time point is loaded as 
an image. The channel number is added as the 'channel' key along with data type
as the 'type key', the source file path as the 'path' key, the probe emission 
wavelength as 'wavelength'. The 'normalized' key is True if the image has been 
normalized between 0 and 1  and False otherwise. These later keys are not ics 
compatible metadata keys!!! Dimension order of the reader is XYZ
"""
import a3dc_module_interface as a3
from .imageclass import Image
import numpy as np
from ast import literal_eval

required_ics_keys=['IcsGetCoordinateSystem','IcsGetSignificantBits'] 

def metadata_to_dict(a3image):


    metadata={}
    for idx, line in enumerate(str(a3image.meta).split('\n')[1:-1]):
            line_list=line.split(':')
            
            #for the 'path' key the path is separated as well
            if line_list[0].lstrip()=='path':
                metadata['path']=':'.join(line_list[1:])

            else:
                try:
                    metadata[':'.join(line_list[:-1])]=literal_eval(line_list[-1].lstrip())
                except:
                    metadata[':'.join(line_list[:-1])]=line_list[-1].lstrip()
    
    return metadata


def is_ics(a3_image):
    '''Check if image has been loaded from ics image. ICS files have required keys 
    (see Dean P, Mascio L, Ow D, Sudar D, Mullikin J.,Proposed standard for 
    image cytometry data files., Cytometry. 1990;11(5):561-9.) Function checks 
    if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is among the 
    dictionary keys the dictionary is taken as ics.
    '''
    return (a3_image.meta.has(required_ics_keys[0]) or a3_image.meta.has(required_ics_keys[1]))


  

def ics_to_metadata(array, ics_metadata):
        
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
    
    
def a3image_to_image(a3image):
        
    #get image array
    array=a3.MultiDimImageFloat_to_ndarray(a3image)
    
    #Get image metadata and convert database if the metadata is ICS style
    metadata=metadata_to_dict(a3image)     
    if is_ics(a3image):
        metadata=ics_to_metadata(array, metadata)
        
    return Image(array, metadata)
        
def image_to_a3image(image):
    
    a3image=a3.MultiDimImageFloat_from_ndarray(image.array.astype(np.float))    
    
    #Clear metadata
    a3image.meta.clear()
    
    #Add metadata key
    for key in image.metadata.keys():
        a3image.meta.add(key, str(image.metadata[key]))        
        
    return a3image 
   
    










