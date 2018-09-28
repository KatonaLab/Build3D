# -*- coding: utf-8 -*-
"""
Created on Sun Mar 25 22:55:01 2018

@author: Nerberus
"""

import pandas as pd
import os
import numpy as np
from skimage.external.tifffile import imread, imsave



class Image(object):
    
    '''TOBE FIXED return objects cpy or deepcopy etc.
    
    '''

    def __init__(self, image, metadata, database=None):
        
        #Set metadata
        self.metadata=metadata        
        
        #set array
        self.array=self.__validate(image, metadata)

        #Set database if supplied
        if database!=None:
            self.database=database
    
    def __validate(self, array, metadata):
        
        #Check if compulsory keys are missing
        key_list=['Type']# 'Name', 'SizeC', 'SizeT', 'SizeX', 'SizeY', 'SizeZ', 'DimensionOrder']
        missing_keys=[]
        for key in key_list:
            if key not in metadata:
                missing_keys.append(key)
        if len(missing_keys)>0:
            raise Exception('Invalid Metadata! Missing the following keys: '+str(missing_keys))
        
        
        #Check if metadata 'Type' field matches the type of the image
        if metadata['Type']!=array.dtype:
             #raise Warning('Image array type is '+str(array.dtype)+' while metadata is '+str( metadata['Type'])+' ! Metadata is modified acordingly!')
             array=array.astype(metadata['Type'])
    
        return array
    
    @classmethod
    def load_image(cls, file_path, metadata=None):
        #Load image using scikit/image im read
        return Image(imread(str(file_path)), metadata)
    
    @classmethod
    def image_from_a3(cls, file_path, metadata=None):
        #Load image using scikit/image im read
        #return Image(imread(str(file_path)), metadata)
        pass
    
    @staticmethod
    def save_image(img, path, file_name='output'):
        
        name, ext = os.path.splitext(file_name)
        if ext.lower() not in ['tiff', 'tif']:
              file_name=file_name+".tiff"
        
        #Save image using tifffile save
        imsave(os.path.join(path, file_name), img.array)


    def as_type(self, dtype):
        
        if dtype!=self.metadata['Type']:
            self.array=self.array.astype(dtype)
            self.metadata['Type']=dtype

    
    @staticmethod
    def save_data(img_list, path, file_name='output', to_text=True):
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
            numeric_keys=Image.reorderList(numeric_keys,preset_order)
            preset_order = ['centroid']
            other_keys=Image.reorderList(other_keys,preset_order)
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
    
   

    @staticmethod
    def reorderList(list, val_list):
    
        for element in reversed(val_list):
            if element in list:
                list.remove(element)
                list.insert(0, element)
    
        return list