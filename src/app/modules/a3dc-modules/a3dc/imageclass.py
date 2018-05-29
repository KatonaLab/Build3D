# -*- coding: utf-8 -*-
"""
Created on Sun Mar 25 22:55:01 2018

@author: Nerberus
"""

import pandas as pd
import os

from skimage.external.tifffile import imread, imsave



class Image(object):
    
    '''TOBE FIXED return objects cpy or deepcopy etc.
    
    '''

    def __init__(self, image, metadata=None, database=None):
        
    
        #set array
        self.array=image
        
        #Set metadata
        if metadata==None:
            self.metadata={}
        else:
            self.metadata=metadata
        self.metadata["Type"]=image.dtype
        
        #Set database if supplied
        if database!=None:
            self.database=database
    
    @classmethod
    def load_image(cls, filePath, metadata=None):
        #Load image using scikit/image im read
        return Image(imread(str(filePath)), metadata)

    @staticmethod
    def save_image(inputImageList, dir_path, suffix):
                
        for img in inputImageList:
            #Save image using tifffile save
            imsave(os.path.join(dir_path, img.metadata['Name']+"_"+suffix+".tiff" ), img.array) 
    
    @staticmethod
    def save_data(inputImageList, path, fileName='output', toText=True):
        '''
        :param dictionaryList: Save dictionaries in inputDictionaryList
        :param path: path where file is saved
        :param toText: if True data are saved to text
        :param fileName: fileneme WITHOUT extension
        :return:
        '''
    
        dataFrameList=[]
        keyOrderList=[]
        columnWidthsList=[]
                    
        dictionaryList=[x.database for x in inputImageList]
        namelist = [x.metadata['Name'] for x in inputImageList]
    
    
        for dic in  dictionaryList:
    
           
            print('')
            # Convert to Pandas dataframe
            dataFrame=pd.DataFrame(dic)
            
            dataFrameList.append(dataFrame)
    
            # Sort dictionary with numerical types first (tag, volume, voxelCount,  first) and all others after (centroid, center of mass, bounding box first)
            numericalKeys = []
            otherKeys = []
    
            for key in list(dic.keys()):
    
                if str(dataFrame[key].dtype) in ['int', 'float', 'bool', 'complex', 'Bool_', 'int_','intc', 'intp', 'int8' ,'int16' ,'int32' ,'int64',
                    'uint8' ,'uint16' ,'uint32' ,'uint64' ,'float_' ,'float16' ,'float32' ,'float64','loat64' ,'complex_' ,'complex64' ,'complex128' ]:
                    numericalKeys.append(key)
    
                else:
                    otherKeys.append(key)
    
            #Rearange keylist
            presetOrder=['tag', 'volume', 'voxelCount', 'filter']
            numericalKeys=reorderList(numericalKeys,presetOrder)
            presetOrder = ['centroid']
            otherKeys=reorderList(otherKeys,presetOrder)
            keyOrderList.append(numericalKeys+otherKeys)
    
            # Measure the column widths based on header
            columnWidth=0
            for i in range(len(keyOrderList)):
                for j in range(len(keyOrderList[i])):
                    w=len(keyOrderList[i][j])
                    if w>columnWidth:
                        columnWidth=w
            columnWidthsList.append(columnWidth)
    
        if toText==False:
            # Create a Pandas Excel writer using XlsxWriter as the engine.
            writer = pd.ExcelWriter(os.path.join(path, fileName+'.xlsx'), engine='xlsxwriter')
            
            for i in range(len(dataFrameList)):
                
           
       
                #If no names are given ore namelist is shorter generate worksheet name
                if namelist==None or i>len(namelist):
                    name='Data_'+str(i)
                else:
                   name =str(namelist[i]) 
    
                # Convert the dataframe to an XlsxWriter Excel object. Crop worksheet name if too long
                if len(name) > 30:
                    name=(str(name)[:30] + '_')
                
                dataFrameList[i].to_excel(writer, sheet_name=name, columns=keyOrderList[i], header=True)
    
                #Get workbook, worksheet and format
                workbook = writer.book
                format=workbook.add_format()
                format.set_shrink('auto')
                format.set_align('center')
                format.set_text_wrap()
    
                worksheet=writer.sheets[name]
                worksheet.set_zoom(90)
                worksheet.set_column(j, 1, columnWidthsList[i]*0.6, format)
    
            # Close the Pandas Excel writer and save Excel file.
            writer.save()
    
        elif toText==True:
    
            with open(os.path.join(path, fileName + '.txt'), 'w') as outputFile:
    
                for i in range(len(dataFrameList)):
                    #if dataFrameList[i] != None:
                    outputFile.write('name= '+namelist[i]+'\n')
                    outputFile.write(dataFrameList[i].to_csv(sep='\t', columns=keyOrderList[i], index=False, header=True))
    
        return 0
   

  
'''
def save_image(imageList, path, fileName='output.tif', append=False, bigtiff=False, byteorder=None, software='A3DC', imagej=False, photometric=None, planarconfig=None, tile=None, contiguous=True, compress=0, colormap=None, description=None, datetime=None, resolution=None, metadata={}, extratags=()):

    if byteorder is None:
        byteorder = '<' if sys.byteorder == 'little' else '>'

    with TiffWriter(os.path.join(path, fileName), append, bigtiff, byteorder, software, imagej) as tif:
        tif.save( np.array(imageList),  photometric, planarconfig, tile, contiguous, compress, colormap, description, datetime, resolution, metadata, extratags)
'''

def reorderList(list, valueList):

    for element in reversed(valueList):
        if element in list:
            list.remove(element)
            list.insert(0, element)

    return list