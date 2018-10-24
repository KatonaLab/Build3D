import SimpleITK as sitk
import numpy as np
import pandas as pd
import copy
import os
import sys
from ast import literal_eval

from . import segmentation
from .utils import reorder_list, warning
from modules.packages.PythImage.ImageClass import ImageClass as PythImage
from.constants import SHAPE_DESCRIPTORS, INTENSITY_DESCRIPTORS, OTHER_DESCRIPTORS


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
            self.reorder('ZYXCT')
            array=output.image[0][0]
            
        else:
            raise Exception('Image has to be 3D only (must contain one time point or one channel)!')


        return array

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
    
    
        #get image array
        array=md.MultiDimImageFloat_to_ndarray(multidimimage)
    
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


###############################################################################
##############################IO Functio#######################################
###############################################################################        

def save_data(img_list, path, file_name, to_text=True):
    '''
    :param dict_list: Save dictionaries in inputdict_list
    :param path: path where file is saved
    :param to_text: if True data are saved to text
    :param fileName: fileneme WITHOUT extension
    :return:
    '''

    dataframe_list=[]
    key_order_list=[]

                
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
        numeric_keys=reorder_list(numeric_keys,preset_order)
        preset_order = ['centroid']
        other_keys=reorder_list(other_keys,preset_order)
        key_order_list.append(numeric_keys+other_keys)
        

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
            
            #Get workbook, worksheet
            workbook = writer.book
            
            worksheet=writer.sheets[name]
            worksheet.set_zoom(90)        
            worksheet.freeze_panes(1, 0)
        
            #Set column widths based on header
            cell_format=workbook.add_format()
            cell_format.set_shrink('auto')
            cell_format.set_align('center')
            cell_format.set_align('vcenter')
            cell_format.set_text_wrap()
            cell_format.set_shrink()
            

            
            
            #Set format of the first column
            worksheet.set_column(1, 1, 20, cell_format) 
            
            #Set column format from the second onwards
            for j in range(len(key_order_list[i])):
                worksheet.set_column(j+1, j+1, len(key_order_list[i][j])*1.5, cell_format)  
            
            #Set formatt of the first row
            row_format=workbook.add_format()
            row_format.set_align('center')
            row_format.set_align('vcenter')
            row_format.set_bottom(1)
            worksheet.set_row(0, 70, row_format)

            #Add comments to excel cells
            for j in range(len(key_order_list[i])):
                
                comment_string='N/A'
                key=key_order_list[i][j]
                
                if key in SHAPE_DESCRIPTORS.keys():
                    comment_string=SHAPE_DESCRIPTORS[key]
                else:
                    int_key=key.split(' ')[0]
                    if int_key in INTENSITY_DESCRIPTORS.keys():
                        comment_string=INTENSITY_DESCRIPTORS[int_key]
                    if int_key in OTHER_DESCRIPTORS.keys():
                        comment_string=OTHER_DESCRIPTORS[int_key]
                        
                worksheet.write_comment(0, j+1, comment_string, {'height': 80, 'width':300, 'color': '#00ffffcc'})

        # Close the Pandas Excel writer and save Excel file.
        writer.save()

    elif to_text==True:

        with open(os.path.join(path, file_name + '.txt'), 'w') as outputFile:

            for i in range(len(dataframe_list)):
                #if dataframe_list[i] != None:
                outputFile.write('name= '+name_list[i]+'\n')
                outputFile.write(dataframe_list[i].to_csv(sep='\t', columns=key_order_list[i], index=False, header=True))    
    
       
def save_image(img_list, path, file_name):
    '''
    '''

    #Combine images
    for idx, img in  enumerate(img_list):
        
        #Remove 'Path' key
        if 'Path' in img.metadata.keys():
            del img.metadata['Path']
            
        if idx==0:
            output=copy.deepcopy(img)
        else:
            output.append_to_dimension(img)
    
    #save image
    output.save(path, file_name)

###############################################################################
###############################Analysis########################################
###############################################################################
    
def analyze(tagged_img, img_list=None, meas_list=['volume', 'voxelCount', 'pixelsOnBorder', 'centroid', 'meanIntensity', 'maximumPixel']):

    #Convert tagged image to ITK image
    itk_img = tagged_img.to_itk()

    
    #Measurements apartain to two groups, one  with single input (shape descriptors) 
    #and multi image measurements (parameters that depend on intensity )
    
    # Instatiate ITK LabelIntensityStatisticsImageFilter()
    itk_filter = sitk.LabelIntensityStatisticsImageFilter()

    shape_functions = {'volume': itk_filter.GetPhysicalSize,
                                 'voxelCount': itk_filter.GetNumberOfPixels,
                                 'centroid': itk_filter.GetCentroid,
                                 'ellipsoidDiameter': itk_filter.GetEquivalentEllipsoidDiameter,
                                 'boundingBox': itk_filter.GetBoundingBox,
                                 'pixelsOnBorder': itk_filter.GetNumberOfPixelsOnBorder,
                                 'elongation': itk_filter.GetElongation,
                                 'equivalentSphericalRadius': itk_filter.GetEquivalentSphericalRadius,
                                 'flatness': itk_filter.GetFlatness,
                                 'principalAxes': itk_filter.GetPrincipalAxes,
                                 'principalMoments': itk_filter.GetPrincipalMoments,
                                 'roundness': itk_filter.GetRoundness,
                                 'feretDiameter': itk_filter.GetFeretDiameter,
                                 'perimeter': itk_filter.GetPerimeter,
                                 'perimeterOnBorder': itk_filter.GetPerimeterOnBorder,
                                 'perimeterOnBorderRatio': itk_filter.GetPerimeterOnBorderRatio,
                                 'equivalentSphericalPerimeter': itk_filter.GetEquivalentSphericalPerimeter}

    intensity_functions = {'meanIntensity': itk_filter.GetMean,
                               'medianIntensity': itk_filter.GetMedian,
                               'skewness': itk_filter.GetSkewness,
                               'kurtosis': itk_filter.GetKurtosis,
                               'variance': itk_filter.GetVariance,
                               'maximumPixel': itk_filter.GetMaximumIndex,
                               'maximumValue': itk_filter.GetMaximum,
                               'minimumValue': itk_filter.GetMinimum,
                               'minimumPixel': itk_filter.GetMaximumIndex,
                               'centerOfMass': itk_filter.GetCenterOfGravity,
                               'standardDeviation': itk_filter.GetStandardDeviation,
                               'cumulativeIntensity': itk_filter.GetSum,
                               'getWeightedElongation': itk_filter.GetWeightedElongation,
                               'getWeightedFlatness': itk_filter.GetWeightedFlatness,
                               'getWeightedPrincipalAxes': itk_filter.GetWeightedPrincipalAxes,
                               'getWeightedPrincipalMoments': itk_filter.GetWeightedPrincipalMoments}

    
    #Sort measurement list and parametrize itk_filter
    shape_meas_list = []
    intensity_meas_list = []

    for key in meas_list:

        if key == 'getFeretDiameter':
            itk_filter.ComputeFeretDiameterOn()

        if key in ['getPerimeter', 'getPerimeterOnBorder', 'getPerimeterOnBorderRatio','getEquivalentSphericalPerimeter']:
            itk_filter.ComputePerimeterOn()

        if (key in shape_functions.keys()):
            shape_meas_list.append(key)

        if (key in intensity_functions.keys()):
            intensity_meas_list.append(key)
                

    # database dictionary to store results
    database = {'tag': [], }
    
    for key in shape_meas_list:
        database[key] = []

    #Cycle through images to measure parameters
    if img_list == None:
        img_list = []
        img_list.append(tagged_img)

    for i in range(len(img_list)):

        itk_raw = img_list[i].to_itk()
        
        for key in intensity_meas_list:
            database[key+' in '+img_list[i].metadata['Name']] = []

        #Execute Filter and get database
        itk_filter.Execute(itk_img, itk_raw)
        data=itk_filter.GetLabels()

        for label in data:
           
            if i==0:
                database['tag'].append(label)
                for key in shape_meas_list:
                    database[key].append(shape_functions[key](label))

            for key in intensity_meas_list:
                database[key+' in '+img_list[i].metadata['Name']].append(intensity_functions[key](label))
    
    #Measure object surface
    if 'surface' in meas_list:
        #Create and measure Surface Image
        surface=segmentation.create_surfaceImage(tagged_img)
      
        surfaceitk_img=sitk.GetImageFromArray(surface)
        
        itk_filter=sitk.LabelShapeStatisticsImageFilter()
        itk_filter.Execute(surfaceitk_img)
        surface_data=itk_filter.GetLabels()

        database['surface']=[]
        for label in surface_data:
            database['surface'].append(shape_functions['voxelCount'](label))
            
    #Determine objects on front and back surface
    if 'onFaces' in meas_list:
     
        #Create and measure Surface Image
        facesitk_img=sitk.GetImageFromArray([tagged_img.image[0]])
       
        itk_filter=sitk.LabelShapeStatisticsImageFilter()
        itk_filter.Execute(facesitk_img)
        faces_data=itk_filter.GetLabels()
        
        database['onFaces']=[False]*np.amax(tagged_img.image)
        for label in faces_data:
            database['onFaces'][label-1]=True
    
    #Add results to input image
    tagged_img.database=database

    return tagged_img


def filter_database(dictionary, filter_dict, overwrite=True):
 

    df = pd.DataFrame(dictionary)
    
    if 'filter' in df.keys() and overwrite==False:
        final_filter = df['filter'].values
    else:
        final_filter=[True for i in range(len(df))]

    # Only run filter if key has numerical value
    typeList = ['int', 'float', 'bool', 'complex', 'int_', 'intc', 'intp', 'int8', 'int16', 'int32', 'int64'
        , 'uint8', 'uint16', 'uint32', 'uint64', 'float_', 'float16', 'float32', 'float64', 'loat64', 'complex_',
                'complex64', 'complex128']

    for key in filter_dict:
        

        if df[key].dtype in typeList:
         
            curr_filter = (df[key] >= filter_dict[key]['min']) & (df[key] <= filter_dict[key]['max']).values
                
            final_filter = np.multiply(final_filter, curr_filter)

    df['filter'] = final_filter     
    
    if remove_filtered==True and 'filter' in df.keys() :
        df.drop(df[df['filter'] == False].index, inplace=True)


    dictionary = df.to_dict(orient='list')
   
    return dictionary



def remove_filtered(tagged_img, database):

    change_dict={}
 
    if 'filter' in database.keys():

        for i in range(len(database['filter'])):#database should have a label key!!!
            if database['filter'][i]==False:
                change_dict[int(database['tag'][i])]=0
                
        itk_img = tagged_img.to_itk()

        sitk_filter = sitk.ChangeLabelImageFilter()
        sitk_filter.SetChangeMap(change_dict)
        
        output_image=sitk.GetArrayFromImage(sitk_filter.Execute(itk_img))
        
        #Remove filtered from dictionary
        df = pd.DataFrame(database)
        df.drop(df[df['filter'] == False].index, inplace=True)
    
        output_database = df.to_dict(orient='list')
        
        return output_image , output_database
       
        
    else:
        return tagged_img, database
    
    
        
###############################################################################
##############################Colocalization###################################
###############################################################################




def overlap_image(array_list):

    # Create Overlapping ImagE
    output_array = array_list[0]> 0
    for i in range(1, len(array_list)):
        output_array = np.multiply(output_array, array_list[i]>0)
        
    return output_array


def colocalization_connectivity(image_list, raw_img_list=None):

    
    #Check if images have database and contain the appropriate measurements
    missing_database=[]
    missing_key=[]
    for img in image_list:
        if  hasattr( img, 'database'):
            if 'voxelCount' not in img.database.keys():
                missing_key.append(img.metadata['Name'])
        else:
            missing_database.append(img.metadata['Name'])
    
    #Raise exception if database or voxelCount is missing
    if  missing_database!=[] or missing_key!=[]:
        error_msg=''
        if missing_database!=[]:
            error_msg+='Images :'+str(missing_database)+' are missing attribute "database"!!'
        if missing_database!=[]:
            error_msg+='Database in :'+str(missing_key)+' is missing the "voxelCount" key!!'        
        
        raise Exception(error_msg)
            
            
            
    #Create list of names 
    name_list = [x.metadata['Name'] for x in image_list]
     
    # Create Overlapping Image
    array_list=[x.get_3d_array() for x in image_list]
    ovl_array = overlap_image(array_list)

    #Create overlapping image metadata
    metadata=copy.deepcopy(image_list[0].metadata)
    metadata['Name']=str.join('_', name_list)
    
    #Create overlapping image
    ovl_image=VividImage(segmentation.tag_image(ovl_array), metadata)
    
    if raw_img_list==None:
        raw_img_list=[ovl_image]

    else:
        raw_img_list.append(ovl_image)
    
    ovl_image=analyze(ovl_image, raw_img_list, meas_list=['volume', 'voxelCount', 'centroid', 'pixelsOnBorder', 'meanIntensity', 'maximumPixel'])

    
    # Generate array lists and name lists
    for i in range(len(image_list)):
        itk_image = sitk.GetImageFromArray(image_list[i].get_3d_array())

        #Get pixel from database
        ovl_pixels=ovl_image.database['maximumPixel in '+ovl_image.metadata['Name']]

        object_list = [None for i in range(len(ovl_pixels))]
        ovl_ratio_list = [None for i in range(len(ovl_pixels))]

        
        for j in range(len(ovl_pixels)):
            ovl_position = ovl_pixels[j]
            
            object_list[j] = itk_image.GetPixel(ovl_position)
 
            ovl_ratio_list[j] = ovl_image.database['voxelCount'][j] / image_list[i].database['voxelCount'][image_list[i].database['tag'].index(object_list[j])]

        ovl_image.database['object in ' + name_list[i]] = object_list
        ovl_image.database['overlappingRatio in ' + name_list[i]] = ovl_ratio_list
    
    #Remove unused keys from overlapping database
    for key in list(ovl_image.database.keys()):
        for k in ['meanIntensity','maximumPixel']:
            if k==key.split(' ')[0]:
                del ovl_image.database[key]
    
    return ovl_image


def colocalization_analysis(image_list, ovl_img):
    
    #Start analysis
    database_list=[x.database for x in image_list]
    ovl_database=ovl_img.database

    name_list = [x.metadata['Name'] for x in  image_list]

    obj_no = [len(x.database['tag']) for x in image_list]
    input_element_no=len(database_list)

    ovl_is_filtered = 'filter' in ovl_database.keys()
    dict_is_filtered = [('filter' in x.keys()) for x in database_list]
    
    #Check if images have database attribute
    missing_database=[]
    for img in image_list+[ovl_img]:
        if  not hasattr(img, 'database'):
            missing_database.append(img.metadata['Name'])
    if missing_database!=[]:
       raise Exception('Images :'+str(missing_database)+' are missing attribute "database"!!')
    
    #Check if overlapping database has the appropriatekeys
    missing_key=[]
    required_keys=['object in '+name for name in name_list]+['overlappingRatio in '+name for name in name_list]
    for key in required_keys:
        if key not in ovl_img.database.keys():
            missing_key.append(key)
    if missing_key!=[]:
        raise Exception('Overlapping database is missing the following keys :'+str(missing_key)+'!!')

 
    # Create list of dictionaries to store data
    output_list=[{} for x in range(input_element_no)]


    for i in range(input_element_no):
        output_list[i]['colocalizationCount']=[0  for i in range(obj_no[i])]
        output_list[i]['totalOverlappingRatio'] = [0  for i in range(obj_no[i])]

        position_list = [x for x in range(input_element_no) if x != i]

        for j in range(0, len(position_list)):
            
            output_list[i]['object in '+name_list[position_list[j]]]=[[] for i in range(obj_no[i])]


    for j in range(len(ovl_database['tag'])):
        #Determine if any of the objects have been filtered out and retrieve tags and their rispected position
        flag=True

        if ovl_is_filtered==True:
            flag=ovl_database['filter'][j]
            
        curr_tag_list=[]
        curr_pos_list=[]
        for i in range(0,input_element_no):

            currentTag=ovl_database['object in ' + name_list[i]][j]

            currentPosition=database_list[i]['tag'].index(currentTag)

            curr_tag_list.append(currentTag)
            curr_pos_list.append(currentPosition)


            if dict_is_filtered[i]==True :#and 'filter' in database_list[i].keys() :

                    flag*=database_list[i]['filter'][currentPosition]
                    
        #If none of the objects have been filtered out add info to output
        if flag==True:

            for i in range(input_element_no):

                if curr_tag_list[i] in database_list[i]['tag']:

                        output_list[i]['colocalizationCount'][curr_pos_list[i]]+=1
                        output_list[i]['totalOverlappingRatio'][curr_pos_list[i]] += ovl_database['overlappingRatio in ' + name_list[i]][j]
           
                position_list = [x for x in range(input_element_no) if x != i]
                for k in range(len(position_list)):
                    output_list[i]['object in ' + name_list[position_list[k]]][curr_pos_list[i]].append(curr_tag_list[position_list[k]])


    for i in range(input_element_no):
        for key in output_list[i]:
            image_list[i].database[key]=output_list[i][key]
        ovl_img.database=ovl_database
           
    return ovl_img, image_list 


