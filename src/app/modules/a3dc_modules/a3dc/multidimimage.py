# -*- coding: utf-8 -*-
"""
Created on Wed Sep 26 07:11:59 2018

@author: pongor.csaba

This file contains functions needed to work with A3-DC MultiDimImageFloat 
(through the a3dc_module_interface) image types and the ICS reading module. 

If the MultiDimImageFloat is from an ics module the image metadata are 
converted: The ics loading module reads ics
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
compatible metadata keys!!! Dimension order of the reader is XYZ. Samples per 
pixel daa not read from ics header so it is set to 1 default.
"""
import a3dc_module_interface as md
from .imageclass import VividImage
import numpy as np
from ast import literal_eval
from modules.a3dc_modules.a3dc.utils import warning




required_ics_keys=['IcsGetCoordinateSystem','IcsGetSignificantBits']


def from_multidimimage(multidimimage, database=None):
        
    #get image array
    array=md.MultiDimImageFloat_to_ndarray(multidimimage)

    #Get image metadata and convert database if the metadata is ICS style
    metadata=metadata_to_dict(multidimimage)

    if is_ics(multidimimage):
        metadata=ics_to_metadata(array, metadata)
    #else:
       #array=array[::,::-1,::] 
    
    #Create output image    
    output=VividImage(array, metadata)

    #Add database if available
    if database!=None and isinstance(database, dict):
        output.database=database

    return output
        
def to_multidimimage(image):
    
    #Check if image is time series
    if image.metadata['SizeT']>1:
        warning("Image is a time series! Only the first time step will be extracted!")
        image.metadata['SizeT']=1
        
    #Check if image has multiple channels
    if image.metadata['SizeC']>1:
        warning("Image is a multichannel image! Only the first channel will be extracted!")
        image.metadata['SizeC']=1
    
    #Create output MultiDimImageFloat
    print('1', image.metadata['DimensionOrder'])
    image.reorder('ZYXCT')
    output=md.MultiDimImageFloat_from_ndarray(image.image[0,0].astype(np.float))
    print('2', image.metadata['DimensionOrder'])
    
    #Clear metadata
    output.meta.clear()
    
    for key in image.metadata.keys():
        output.meta.add(key, str(image.metadata[key]))        
        
    return output 

def metadata_to_dict2(multidimimage):


    metadata={}
    for idx, line in enumerate(str(multidimimage.meta).split('\n')[1:-1]):
            line_list=line.split(':')
            
            #for the 'path' key the path is separated as well.
            if line_list[0].lstrip().lower()=='path':
                metadata[line_list[0].lstrip()]=':'.join(line_list[1:])         

            else:
                try:
                    metadata[':'.join(line_list[:-1])]=literal_eval(line_list[-1].lstrip())

                except:
                    metadata[':'.join(line_list[:-1])]=line_list[-1].lstrip()
    
    return metadata


def metadata_to_dict(multidimimage):

    metadata={}
    for idx, line in enumerate(str(multidimimage.meta).split('\n')[1:-1]):
            
            line_list=line.split(':')
            
            #Get key and value. Ics metadata keys have : as separator
            #for the 'path' key the path is separated as well.
            if line_list[0].lstrip().lower()=='path':
                key=line_list[0].lstrip()
                value=':'.join(line_list[1:])
            else:
                key=':'.join(line_list[:-1])
                value=multidimimage.meta.get(key)
            
            #ad metadata key value to outpit dictionary
            try:
                metadata[key]=literal_eval(value)

            except:
                metadata[key]=value
    
    return metadata

def is_ics_dict(dictionary): 
    
    flag=False
    for key in required_ics_keys:
        if key in dictionary:
            flag=True
    return flag


def is_ics(multidimimage):
    '''Check if image has been loaded from ics image. ICS files have required keys 
    (see Dean P, Mascio L, Ow D, Sudar D, Mullikin J.,Proposed standard for 
    image cytometry data files., Cytometry. 1990;11(5):561-9.) Function checks 
    if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is among the 
    dictionary keys the dictionary is taken as ics.
    '''
    return (multidimimage.meta.has(required_ics_keys[0]) or multidimimage.meta.has(required_ics_keys[1]))



def ics_to_metadata(array, ics_metadata):
        
        #Get Shape information
        ome_metadata={'SizeT': 1, 'SizeC':1, 'SizeZ':array.shape[-1], 'SizeX':array.shape[0], 'SizeY':array.shape[1]}
        ome_metadata['DimensionOrder']='ZYXCT'
        
        channel=int(ics_metadata['channel'])
        
        #Add Type and path
        ome_metadata['Type']=ics_metadata['type']
        ome_metadata['Path']=ics_metadata['path']
        #!!!!!!!!!Not read from header by ics loder module!!!!!!!
        ome_metadata['SamplesPerPixel']=1
        
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
    

