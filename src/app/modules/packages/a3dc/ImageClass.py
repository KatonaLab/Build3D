import SimpleITK as sitk
import numpy as np
import sys
import copy

from utils import  warning
from external.PythImage import ImageClass as PythImage
from ast import literal_eval
from external.PythImage import ImageClass

#try:
 #   from modules.packages.PythImage.ImageClass import ImageClass as PythImage
#except:
 #   sys.path.append("..")
  #  from PythImage.ImageClass import ImageClass as PythImage


class VividImage(PythImage):
    '''   
    ITK image: Is an image type used by the SimpleITK package.
    
    MultiDimImageFloat: Is an image type used by the Visualization framwork of A3DC
    .This image type can contain only 3D data for a single channel and time steps! 
    The image type has a meta attribute that contains the metadata in string form.
    If the image is from an ics module the image metadata are converted to Ome-Tiff
    compatibble key-value pairs. The ics loading module reads ics metadata and 
    gives it a name based on the function that reads the metadata with subelements 
    divided by ':'. ICS files have required keys (see Dean P, Mascio L, Ow D, Sudar
     D, Mullikin J.,Proposed standard for image cytometry data files., Cytometry. 
     1990;11(5):561-9.) if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is 
    among the metadata keys the metadata is taken asics metadata!!!!!. Currently 
    only one channel and first time point is loaded as an image. The channel number
     is added as the 'channel' key along with data type as the 'type key', the 
     source file path as the 'path' key, the probe emission wavelength as 
     'wavelength'. The 'normalized' key is True if the image has been normalized 
    between 0 and 1  and False otherwise. These later keys are not ics 
    compatible metadata keys!!! Dimension order of the reader is XYZ. Samples per 
    pixel daa not read from ics header so it is set to 1 default.
    '''
    
    __REQUIRED_ICS_KEYS=['IcsGetCoordinateSystem','IcsGetSignificantBits']
    __DIM_TRANSLATE=PythImage._ImageClass__DIM_TRANSLATE   
    
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


        #Check if physical size information available, if not set default values
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        #Add defaul unit if not available
        for key in size_list:
            if key not in metadata.keys():
                metadata[key]=1.0
                
        #Check if physical unit information available, if not set default values
        unit_list=['PhysicalSizeXUnit', 'PhysicalSizeYUnit', 'PhysicalSizeZUnit']
        #Add defaul unit if not available
        for key in unit_list:
            if key not in metadata.keys():
                metadata[key]='um'

        #Call parent __init__
        super(VividImage, self).__init__(image, copy.deepcopy(metadata))
        
        #Add additional keys
        for key in metadata.keys():
            if key not in self.metadata.keys():
                self.metadata[key]=copy.deepcopy(metadata[key])
        
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

    def get_dimension(self, index, dimension='C'):

        #Get channel from image. 
        if dimension not in self.__DIM_TRANSLATE.keys():
            raise Exception('Invalid dimension %s! Value must be in %s' % (str(dimension), str(self.__DIM_TRANSLATE.keys())))
            
        if index>=self.metadata[self.__DIM_TRANSLATE[dimension]] or 0>index:
            raise Exception('Image dimension %s has a length of %s ! Index %s is invalid!' % (str(dimension) ,str(self.metadata[self.__DIM_TRANSLATE[dimension]]),str(index)))
        
        #Create metadata
        metadata=copy.deepcopy(self.metadata)
        metadata[self.__DIM_TRANSLATE[dimension]]=1
        if dimension=='C':
            
            if isinstance(metadata['SamplesPerPixel'], list):
                metadata['SamplesPerPixel']=metadata['SamplesPerPixel'][index]
                metadata['Name']=metadata['Name'][index] 
            elif index==0:
                metadata['SamplesPerPixel']=metadata['SamplesPerPixel']
                metadata['Name']=metadata['Name']
            else:
                raise IndexError('Invalid Index {} ! the "Name" and "SamplesPerPixel" metadata keys are lists for multichannel images.'.format(str(index)))
                
             
        #Extract axis from image array from image array
        order=self.metadata['DimensionOrder']

        array=np.take(self.image, index, len(order)-order.index(dimension)-1)

        return VividImage(array, metadata)
    
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
            #self.reorder('ZYXCT')
            array=output.image[0][0]
            
        else:
            raise Exception('Image has to be 3D image (onlz x, y, z data)!')

        return array
    
    @staticmethod
    def check_compatibility(image_list,  metadata_list=['SizeX','SizeY','SizeZ', 'SizeT']):
            
        
        def check(dict_list, key_list):
            buffer=[]
            for key in key_list:
                value_list=[dic[key] for dic in dict_list]
                
                if not all(x == value_list[0] for x in value_list):
                    buffer.append(key)
            
            if buffer==[]:
                output=None
            else:
                output=buffer
                
            return output
        
        
        error_list=check([a.metadata for a in image_list],  metadata_list)         
  

        if error_list!=None:
            raise(Exception, 'The following image metadata do not match: '+ str(error_list))                   
        

    def to_itk(self):

        itk_img = sitk.GetImageFromArray(self.get_3d_array())
        
        #Get calibration values 
        #!!!Important that size_list/unit_list order has to be Z,Y,X!!!!
        size_list=['PhysicalSizeZ','PhysicalSizeY', 'PhysicalSizeX']
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeYUnit', 'PhysicalSizeXUnit']
        calibration=[]

        for i in range(len(size_list)):
            if size_list[i] in self.metadata.keys():
                calibration.append(float(self.metadata[size_list[i]]))
            else:
                calibration[i].append(1)
        itk_img.SetSpacing(tuple(calibration))
        
        #Check if unit metadata is available
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in self.metadata.keys()]
        if len(missing_unit)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)
    
        return itk_img
    
    @classmethod
    def from_multidimimage(cls, multidimimage, database=None):
        
        import a3dc_module_interface as md
        
        def is_ics(multidimimage):
            '''Check if image has been loaded from ics image. ICS files have required keys 
            (see Dean P, Mascio L, Ow D, Sudar D, Mullikin J.,Proposed standard for 
            image cytometry data files., Cytometry. 1990;11(5):561-9.) Function checks 
            if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is among the 
            dictionary keys the dictionary is taken as ics.
            '''
            return (multidimimage.meta.has(VividImage.__REQUIRED_ICS_KEYS[0]) or multidimimage.meta.has(VividImage.__REQUIRED_ICS_KEYS[1]))   
        
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
    
        
        def metadata_to_dict( multidimimage):
        
            metadata={}
            for idx, line in enumerate(str(multidimimage.meta).split('\n')[1:-1]):
                    
                    line_list=line.split(':')
                    
                    #Get key and value. Ics metadata keys have ':' as separator
                    #For the 'path' key the path also contains ':'  separated as well.
                    if line_list[0].lstrip().lower()=='path':
                        key=line_list[0].lstrip()
                        value=':'.join(line_list[1:])
                        
                    else:    
                        try:
                            key=str(line_list[0])
                            value=multidimimage.meta.get(key)
                        except:
                            key=':'.join(line_list[:-1])
                            value=multidimimage.meta.get(key)
                    
                    #ad metadata key value to outpit dictionary
                    try:
                        metadata[key]=literal_eval(value)
        
                    except:
                        metadata[key]=value
            
            return metadata
    
    
        #get image array
        array=md.MultiDimImageFloat_to_ndarray(multidimimage)
        
        
        #!!!!!!!!!!!!!!!!!!!!!!!!!!!Temporarily solve LUT min max issue
        array[0][0][0]=0
        #maximum=np.amax(array)
        try:
           maximum=np.iinfo(array.dtype).max
        except:
           maximum=np.finfo(array.dtype).max
        array[0][0][1]=maximum
        #array.flat[0]=0

        
        #Get image metadata and convert database if the metadata is ICS style
        metadata=metadata_to_dict(multidimimage)
    
        if is_ics(multidimimage):
            metadata=ics_to_metadata(array, metadata)
        #else:
           #array=array[::,::-1,::] 
        
        #Create output image    
        output=cls(array, metadata)
    
        #Add database if available
        if database!=None and isinstance(database, dict):
            output.database=database
    
        return output

    def to_multidimimage(self):
        
        import a3dc_module_interface as md
        

        
        #Check if image is time series
        if self.metadata['SizeT']>1:
            warning("Image is a time series! Only the first time step will be extracted!")
            self.metadata['SizeT']=1
            
        #Check if image has multiple channels
        if self.metadata['SizeC']>1:
            warning("Image is a multichannel image! Only the first channel will be extracted!")
            self.metadata['SizeC']=1
        
        #Create output MultiDimImageFloat
        self.reorder('ZYXCT')
        multidimimage=md.MultiDimImageFloat_from_ndarray(self.image[0][0].astype(np.float))
    
        
        #Clear metadata
        multidimimage.meta.clear()
        
        for key in self.metadata.keys():
            multidimimage.meta.add(key, str(self.metadata[key]))        
            
        return multidimimage



        
