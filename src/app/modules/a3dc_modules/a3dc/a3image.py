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
from .imageclass import Image
import numpy as np
from ast import literal_eval
import PythImage.ImageClass as PythImage
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, print_line_by_line, warning
import numpy as np
import os
import copy
import pandas as pd

required_ics_keys=['IcsGetCoordinateSystem','IcsGetSignificantBits']

def a3image_to_image(a3image, database=None):
        
    #get image array
    array=md.MultiDimImageFloat_to_ndarray(a3image)

    #Get image metadata and convert database if the metadata is ICS style
    metadata=metadata_to_dict(a3image)     
    if is_ics(a3image):
           
        metadata=ics_to_metadata(array, metadata)
        

    #Create output image    
    output=Image(array, metadata)

    #Add database if available
    if database!=None and isinstance(database, dict):
        output.database=database
        
    return output
        
def image_to_a3image(image):
    
    a3image=md.MultiDimImageFloat_from_ndarray(image.array.astype(np.float))    
    
    #Clear metadata
    a3image.meta.clear()
    
    #Add metadata key
    for key in image.metadata.keys():
        a3image.meta.add(key, str(image.metadata[key]))        
        
    return a3image 

def metadata_to_dict(a3image):


    metadata={}
    for idx, line in enumerate(str(a3image.meta).split('\n')[1:-1]):
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
    
class VividImage(PythImage):
    
    def __init__(self, image, metadata, database=None):
     
        #Check if compulsory keys are missing
        key_list=['Type']# 'Name', 'SizeC', 'SizeT', 'SizeX', 'SizeY', 'SizeZ', 'DimensionOrder']
        missing_keys=[]
        for key in key_list:
            if key not in metadata:
                missing_keys.append(key)
        if len(missing_keys)>0:
            raise Exception('Invalid Metadata! Missing the following keys: '+str(missing_keys))
        
        
        #Check if metadata 'Type' field matches the type of the image
        if metadata['Type']!=image.dtype:
             #raise Warning('Image array type is '+str(array.dtype)+' while metadata is '+str( metadata['Type'])+' ! Metadata is modified acordingly!')
             image=image.astype(metadata['Type'])
        
        #Call parent __init__
        super(VividImage, self).__init__(image, metadata)
        
        #Set database if supplied
        if database!=None:
            self.database=database

    def get_channel(self, ch):
        
        #Get channel from image. 

        if ch>=self.metadata['SizeC']:
            raise Exception('Image has %s channels! Invalid channel %s' % (str(self.metadata['SizeC']), str(ch)))
        
        #Create metadata
        metadata=copy.deepcopy(self.metadata)
        metadata['SamplesPerPixel']=metadata['SamplesPerPixel'][ch]
        metadata['Name']=metadata['Name'][ch]  
        metadata['SizeC']=1
        
        #Extract channel from image array
        order=copy.deepcopy(self.metadata['DimensionOrder'])
        if order.index('T')!=0 and order.index('C')==1:
            self.reorder('XYZCT')    
        
        array = self.image[:, ch, :, :, :]
   
        self.reorder(order)         

        
        return VividImage(array, metadata)
   
    def save_data(self, img_list, path, file_name='output', to_text=True):
        '''
        :param dict_list: Save dictionaries in inputdict_list
        :param path: path where file is saved
        :param to_text: if True data are saved to text
        :param fileName: fileneme WITHOUT extension
        :return:
        '''
    
        dataframe_list=[]
        key_order_list=[]
        col_width_list=[]
                    
        dict_list=[x.database for x in img_list]
        name_list = [x.metadata['Name'] for x in img_list]
    
    
        for dic in  dict_list:
    
            # Convert to Pandas dataframe
            df=pd.DataFrame(dic)
            
            dataframe_list.append(df)
    
            # Sort dictionary with numerical types first (tag, volume, voxelCount,  first) and all others after (centroid, center of mass, bounding box first)
            numeric_keys = []
            other_keys = []
    
            for key in list(dic.keys()):
    
                if str(df[key].dtype) in ['int', 'float', 'bool', 'complex', 'Bool_', 'int_','intc', 'intp', 'int8' ,'int16' ,'int32' ,'int64',
                    'uint8' ,'uint16' ,'uint32' ,'uint64' ,'float_' ,'float16' ,'float32' ,'float64','loat64' ,'complex_' ,'complex64' ,'complex128' ]:
                    numeric_keys.append(key)
    
                else:
                    other_keys.append(key)
    
            #Rearange keylist
            preset_order=['tag', 'volume', 'voxelCount', 'filter']
            numeric_keys=self.__reorderList(numeric_keys,preset_order)
            preset_order = ['centroid']
            other_keys=self.__reorderList(other_keys,preset_order)
            key_order_list.append(numeric_keys+other_keys)
    
            # Measure the column widths based on header
            col_width=0
            for i in range(len(key_order_list)):
                for j in range(len(key_order_list[i])):
                    w=len(key_order_list[i][j])
                    if w>col_width:
                        col_width=w
            col_width_list.append(col_width)
    
        if to_text==False:
            # Create a Pandas Excel writer using XlsxWriter as the engine.
            writer = pd.ExcelWriter(os.path.join(path, file_name+'.xlsx'), engine='xlsxwriter')
            
            for i in range(len(dataframe_list)):
                
                #If no names are given ore name_list is shorter generate worksheet name
                if name_list==None or i>len(name_list):
                    name='Data_'+str(i)
                else:
                   name =str(name_list[i]) 
    
                # Convert the dataframe to an XlsxWriter Excel object. Crop worksheet name if too long
                if len(name) > 30:
                    name=(str(name)[:30] + '_')
                
                dataframe_list[i].to_excel(writer, sheet_name=name, columns=key_order_list[i], header=True)
    
                #Get workbook, worksheet and format
                workbook = writer.book
                format=workbook.add_format()
                format.set_shrink('auto')
                format.set_align('center')
                format.set_text_wrap()
    
                worksheet=writer.sheets[name]
                worksheet.set_zoom(90)
                worksheet.set_column(j, 1, col_width_list[i]*0.6, format)
    
            # Close the Pandas Excel writer and save Excel file.
            writer.save()
    
        elif to_text==True:
    
            with open(os.path.join(path, file_name + '.txt'), 'w') as outputFile:
    
                for i in range(len(dataframe_list)):
                    #if dataframe_list[i] != None:
                    outputFile.write('name= '+name_list[i]+'\n')
                    outputFile.write(dataframe_list[i].to_csv(sep='\t', columns=key_order_list[i], index=False, header=True))
    
   

    def __reorderList(self, lst, val_list):
    
        for element in reversed(val_list):
            if element in lst:
                lst.remove(element)
                lst.insert(0, element)
    
        return lst

    @classmethod
    def from_multidimimage(cls, multidimimage, database=None):
            
        #get image array
        array=md.MultiDimImageFloat_to_ndarray(multidimimage)
    
        #Get image metadata and convert database if the metadata is ICS style
        metadata=metadata_to_dict(multidimimage)     
        if is_ics(multidimimage):
               
            metadata=ics_to_metadata(array, metadata)
            
        #Create output image    
        output=cls(array, metadata)
    
        #Add database if available
        if database!=None and isinstance(database, dict):
            output.database=database

        return output
            
    def to_multidimimage(self, ch):
        
         
        img=self.get_channel(ch)
        
        img.reorder('ZXYCT')
        
        #Check if image is time series
        if self.metadata['SizeT']>1:
            warning("Image is a time series! Only the first time step will be extracted!")
            self.metadata['SizeT']=1
        
        #Create output MultiDimImageFloat
        output=md.MultiDimImageFloat_from_ndarray(self.image[1,1, ::-1,::-1,::].astype(np.float))    
        
        #Clear metadata
        output.meta.clear()
        
        #Add metadata key
        for key in self.metadata.keys():
            output.meta.add(key, str(self.metadata[key]))        
            
        return output 
    
    def metadata_to_dict(a3image):
    
    
        metadata={}
        for idx, line in enumerate(str(a3image.meta).split('\n')[1:-1]):
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
    
    

   
    










