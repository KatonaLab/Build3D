
from modules.a3dc_modules.external.PythImage.ImageClass import ImageClass as PythImage
from skimage.external.tifffile import TiffWriter

import SimpleITK as sitk
import sys, os, copy
from skimage.external.tifffile import imsave
from itertools import product

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

    def is_3d(self):
        
        if self.metadata['SizeT']>1:
            flag=False
        elif self.metadata['SizeC']>1:
            flag=False
        else:
            flag=True
        
        return flag
    
    def get_3d_array(self, T=None, C=None):
        
        output=self

        if T!=None:
            if self.metadata['SizeT']>T+1 or self.metadata['SizeT']<0:
                raise Exception('Invalid time index {}. Image has {} time points!'.format( T, self.metadata['SizeT']))
            else:
                output=output.get_dimension( T, dimension='T')
        
        if C!=None:
            if self.metadata['SizeC']>C+1 or self.metadata['SizeC']<0:
                raise Exception('Invalid channel index {}. Image has {} time points! '.format( C, self.metadata['SizeC']))
            else:
                output=output.get_dimension( C, dimension='C')
        
        if output.metadata['SizeT']==1 and output.metadata['SizeC']==1 :
            self.reorder('XYZCT')
            array=output.image[0][0]
            
        else:
            raise Exception('Image has to be 3D only (must contain one time point or one channel)!')


        return array

    def to_itk(self):

        
        itk_img = sitk.GetImageFromArray(self.get_3d_array())
        
        #Check if physical size metadata is available if any is missing raise Exeption
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        missing_size=[s for s in size_list if s not in self.metadata.keys()]
        if len(missing_size)!=0:
            print('Missing :'+str(missing_size)+'! Unable to carry out analysis!', file=sys.stderr)
        else:
            itk_img.SetSpacing((self.metadata['PhysicalSizeX'],self.metadata['PhysicalSizeY'],self.metadata['PhysicalSizeZ']))
        
        #Check if unit metadata is available
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in self.metadata.keys()]
        if len(missing_size)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)
    
        return itk_img
    
