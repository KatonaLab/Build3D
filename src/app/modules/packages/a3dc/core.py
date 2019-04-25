import SimpleITK as sitk
import numpy as np
import pandas as pd
import copy
import os

try:
    from ImageClass import ImageClass
    from constants import SHAPE_DESCRIPTORS, INTENSITY_DESCRIPTORS, OTHER_DESCRIPTORS, NUMERIC_DTYPES
    from utils import reorder_list
    import segmentation 
except:
    from modules.packages.a3dc.constants import SHAPE_DESCRIPTORS, INTENSITY_DESCRIPTORS, OTHER_DESCRIPTORS, NUMERIC_DTYPES
    from modules.packages.a3dc.utils import reorder_list
    from  modules.packages.a3dc import segmentation
    from modules.packages.a3dc.ImageClass import ImageClass

#TODO:Fix unit tests
#     unit test for threshold 


def threshold(image, method="Otsu", **kwargs):
    '''

    :param image:
    :param imageDictionary:
    :param method:
    :param kwargs:
        lowerThreshold, upperThreshold, mode,blockSize=5, offSet=0

    '''

    # ITK Threshold methods
    auto_list = ['Otsu', 'Huang', 'IsoData', 'Li', 'MaxEntropy', 'KittlerIllingworth', 'Moments', 'Yen',
                         'RenyiEntropy', 'Shanbhag', 'Triangle']

    # Scikit-image Threshold methods
    auto_list_skimage = ['IsoData_skimage', 'Otsu_skimage','Li_skimage','Yen_skimage','Triangle_skimage']
    
    adaptive_list = ['Mean', 'Gaussian','Sauvola','Niblack']

    # Parse kwargs
    if kwargs != {}:
        if method in auto_list or method in auto_list_skimage:
            keyList=['mode']
        elif method in adaptive_list:
            keyList = ['blockSize', 'offSet']
        elif method == 'Manual':
            keyList =['lower', 'upper']
        else:
            raise KeyError('Thresholding method '+str(method)+' not available or valid!')

        kwargs = {your_key: kwargs[your_key] for your_key in keyList if your_key in kwargs}
    
    # Run thresholding functions
    thresholdValue=None    
    if method in auto_list:
        output_array, thresholdValue = segmentation.threshold_auto(image.get_3d_array(), method, **kwargs)

    
    elif method in auto_list_skimage:
        method=method.replace('_skimage', '')
        output_array, thresholdValue = segmentation.threshold_auto_skimage(image.get_3d_array(), method, **kwargs)
        
    elif method in adaptive_list:
        output_array = segmentation.threshold_adaptive(image.get_3d_array(), method, **kwargs)
        thresholdValue='Local'
        
    elif method == 'Manual':
        output_array = segmentation.threshold_manual(image.get_3d_array(), **kwargs)
        thresholdValue=kwargs['upper']
    else:
        raise LookupError("'" + str(method) + "' is Not a valid mode!")

    output_metadta=image.metadata
    output_metadta['Type']=str(output_array.dtype)
    

    return ImageClass(output_array, image.metadata), thresholdValue




###############################################################################
###############################Analysis########################################
###############################################################################


def analyze(img, img_list=None, meas_list=['volume', 'voxelCount', 'pixelsOnBorder', 'centroid', 'meanIntensity', 'maximumPixel']):
    
    
    '''       
    #TODO: Deal with case when image list contains channels with same name!!!!
    #TODO: Create separate function to validate
    '''
    
    #Tagg Image
    tagged_img=ImageClass(segmentation.tag_image(img.get_3d_array()), copy.deepcopy(img.metadata ))   
    

    #If image list is empty or None intensity parameters measured on tagged image
    if img_list == None or img_list ==[] :
        img_list = [tagged_img]
                                    
    #Validate input images
    #Check if images ara 3D 
    #!!!Repeated in colocalization
    for i in img_list+[tagged_img]:
        if not i.is_3d:
            raise Exception('Input images must be 3D')
            
    #Check if image size parameters match
    ImageClass.check_compatibility(img_list+[tagged_img], metadata_list=['SizeX','SizeY','SizeZ', 'SizeT'])

    #Check if channel names are unique if not rename
    name_list=[a.metadata['Name'] for a in img_list]

    if len(name_list)!=len(set(name_list)):
        raise Exception('Channel names in img_list have to be unique!')
  
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
                

    #Initialize database dictionary to store results
    database = {'tag': [], }
    
    for key in shape_meas_list:
        database[key] = []

    #Cycle through images to measure parameters
    for i in range(len(img_list)):

        itk_raw = img_list[i].to_itk()
        
        for key in intensity_meas_list:
            database[key+' in '+name_list[i]] = []

        #Execute ITK Filter and get database
        itk_filter.Execute(itk_img, itk_raw)
        data=itk_filter.GetLabels()

        for label in data:
           
            if i==0:
                database['tag'].append(label)
                for key in shape_meas_list:
                    database[key].append(shape_functions[key](label))

            for key in intensity_meas_list:
                database[key+' in '+name_list[i]].append(intensity_functions[key](label))
    
            
    #Add results to input image
    tagged_img.database=database

    return tagged_img

###############################################################################
#################################Filter########################################
###############################################################################
    
def apply_filter(image, filter_dict=None, remove_filtered=False, overwrite=True):
    '''
    Filters dictionary stored in the 'database' key of the inputDisctionary to 
    be filtered and removes filtered taggs if filterImage=True. Boolean mask is appended to inputDictionary['database']
    and returned through the output dictionary. If removeFiltered=True tags are removed from the output. If overWrite=True a new Boolean mask is created.

    :param inputDictionary: Dictionary containing informason related to inputImage
    :param inputImage: Tagged image
    :param filterDict: Dictionary contains the keywords to be filtered and the min/maximum value as the following example:

            dictFilter={'volume':{'min':2, 'max':11}}#, 'mean in '+taggedDictList[0]['name']: {'min':2, 'max':3}}

    :param outputDictionary
    :param inputImage
    :param removeFiltered: If True objects that are filtered out are removed
    :return:
    '''

    if filter_dict==None:
        filter_dict={}
        
    # 1. Check if image has database and raise warning     
    # 2. Check if keys in database are available and raise warning 
    # 3. Check if keys in filter_dict have numerical type in database
    #(__filter_database dismissed thes keys, however it is good for the users 
    #toknow)

    if hasattr(image,'database'):
        
        df = pd.DataFrame(image.database)
        
        non_numeric=[]
        missing=[]
        for key in filter_dict.keys():
                if key not in df.keys():
                    missing.append(str(key))
                else:
                    if  df[key].dtype not in NUMERIC_DTYPES:
                        non_numeric.append(str(key))
        if missing!=[]:
            raise KeyError('Object database is missing the '+str(missing)+' key(s)!')
        if non_numeric!=[]:
            raise ValueError('Object database keys(s) '+str(non_numeric)+' are not of numeric datatype!')
    
        # Filter dictionary
        output_database=__filter_database(df, filter_dict, overwrite).to_dict(orient='list')

        output_image=image.image
        
        
        # Remove Filtered objects from database and image
        if remove_filtered == True:
            output_image , output_database = __remove_filtered(image, output_database)
    
        output=ImageClass(output_image, image.metadata, output_database)
    
    else:
        raise AttributeError('Image is missing the "database" attribute!')
        output=copy.deepcopy(image)

    
    return output


def __filter_database(df, filter_dict, overwrite=True):
 

    
    if 'filter' in df.keys() and overwrite==False:
        df['filter'] = df['filter'].values
    else:
        df['filter']=np.array([True for i in range(len(df))])
        
    
    for key in filter_dict:

        if df[key].dtype in NUMERIC_DTYPES:
            
            curr_filter = (df[key] >= filter_dict[key]['min']).values & (df[key] <= filter_dict[key]['max']).values

            df['filter']  =  np.logical_and(df['filter'].values, curr_filter)


    return df



def __remove_filtered(tagged_img, database):

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
def colocalization(tagged_img_list, source_image_list=None, overlapping_filter=None,
                   remove_filtered=False, overWrite=True):
    '''
    :param tagged_img_list:
    :param taggedDictList:
    :param sourceImageList:
    :param overlappingFilterList:
    :param filterImage:
    :return:
    '''

    #If source_image_list is None it is converted to empty list
    if source_image_list == None:
        source_image_list = []
        
    #Validate input images   


    #Allow only images with single channel and timepoint 
    #(img.metadat['SizeT']==1 and img.metadata['SizeC']==1)      
    #Check if channel names are unique if not rename
    #Validate input images
    #Check if images ara 3D 
    for i in tagged_img_list+source_image_list:
        if not i.is_3d:
            raise Exception('Input images must be 3D')
            
    #Check if image size parameters match
    ImageClass.check_compatibility(tagged_img_list+source_image_list, metadata_list=['SizeX','SizeY','SizeZ', 'SizeT'])
    

    name_list=[a.metadata['Name'] for a in tagged_img_list]
    if len(name_list)!=len(set(name_list)):
        raise(Exception('Channel names in img_list have to be unique!'))

    #Create image list to validate
    if source_image_list==None:
        check_list=tagged_img_list
    else:
        check_list=tagged_img_list+source_image_list
        
    if source_image_list!=None and  isinstance(source_image_list, list):
        check_list+=source_image_list
    
    #Check if images ara 3D    
    for i in check_list:
        if not i.is_3d:
            raise Exception('Input images must be 3D.')
            
    #Check if image size parameters match
    ImageClass.check_compatibility(check_list, metadata_list=['SizeX','SizeY',
                                                        'SizeZ', 'SizeT',
                                                        'PhysicalSizeZUnit',
                                                        'PhysicalSizeXUnit',
                                                        'PhysicalSizeYUnit',
                                                        'PhysicalSizeX',
                                                        'PhysicalSizeY',
                                                        'PhysicalSizeZ'])

    #Check if images have database and contain the appropriate keys
    #All images are checked beforehand which helps users creating new scripts 
    #etc. exceptions are raised i
    missing_database=[]
    missing_key=[]
    for img in tagged_img_list:
        if  hasattr( img, 'database'):
            if 'voxelCount' not in img.database.keys():
                missing_key.append(img.metadata['Name'][0])
        else:
            
            missing_database.append(img.metadata['Name'][0])
        
    #Raise exception if database or voxelCount is missing
    if missing_database!=[]:
        error_msg='Images :'+str(missing_database)+' are missing attribute "database"!!'
        raise Exception(error_msg)
    if missing_key!=[]:
        error_msg='Database in :'+str(missing_key)+' is missing the "voxelCount" key!!'
        raise Exception(error_msg)        

               
    #Determine connectivity data
    overlapping_image=__colocalization_connectivity(tagged_img_list, source_image_list)
    
    #Filter database and image
    overlapping_image=apply_filter(overlapping_image, overlapping_filter, remove_filtered)

    #Analyze colocalization
    overlapping_image, tagged_img_list=__colocalization_analysis(tagged_img_list, overlapping_image)

    return overlapping_image, tagged_img_list       



def __colocalization_connectivity(image_list, raw_img_list=None):
    

    #Create list of names 
    name_list = [image_list[i].metadata['Name'] for i in range(len(image_list))]
     
    # Create Overlapping Image
    array_list=[x.get_3d_array() for x in image_list]
    ovl_array = segmentation.overlap_image(array_list)

    #Create overlapping image metadata
    metadata=copy.deepcopy(image_list[0].metadata)
    metadata['Name']=str.join('_', name_list)

    #Create overlapping image
    ovl_image=ImageClass(segmentation.tag_image(ovl_array), metadata)

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

def __colocalization_analysis(image_list, ovl_img):

    database_list=[x.database for x in image_list]
    ovl_database=ovl_img.database
    
    name_list = [image_list[i].metadata['Name'] for i in range(len(image_list))]

    obj_no = [len(x.database['tag']) for x in image_list]
    input_element_no=len(database_list)

    ovl_is_filtered = 'filter' in ovl_database.keys()
    dict_is_filtered = [('filter' in x.keys()) for x in database_list]
    
    #Check if images have database attribute
    if  not hasattr(ovl_img, 'database'):
        raise Exception('Overlapping image is missing attribute "database"!!')
    
    #Check if overlapping database has the appropriate keys
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


def __colocalization_analysis2(image_list, ovl_img):

    database_list=[x.database for x in image_list]
    ovl_database=ovl_img.database
    
    name_list = [image_list[i].metadata['Name'] for i in range(len(image_list))]

    obj_no = [len(x.database['tag']) for x in image_list]
    input_element_no=len(database_list)

    ovl_is_filtered = 'filter' in ovl_database.keys()
    dict_is_filtered = [('filter' in x.keys()) for x in database_list]
    
    #Check if images have database attribute
    if  not hasattr(ovl_img, 'database'):
        raise Exception('Overlapping image is missing attribute "database"!!')
    
    #Check if overlapping database has the appropriate keys
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
            print('#######################################################x')
            print(ovl_database['filter'][j])
            
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

###############################################################################
##############################Colocalization###################################
###############################################################################


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

            if str(df[key].dtype) in NUMERIC_DTYPES:
                numeric_keys.append(key)

            else:
                other_keys.append(key)

        #Rearange keylist
        numeric_keys=reorder_list(numeric_keys,['tag', 'volume', 'voxelCount', 'filter'])
        
        other_keys=reorder_list(other_keys,['centroid'])
        
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
                                                       
        #Create statistical report
        #data_dic={name_list[i]:dict_list[i] for i in range(len(dict_list))}
        #rep=report(data_dic, [], ['volume', 'voxelCount', 'totalOverlappingRatio', 'meanIntensity'])
        #report_to_xls(rep,  workbook)
    
        #Create graphical report
        #graphical_report(data_dic, workbook)
   
        
        
        # Close the Pandas Excel writer and save Excel file.
        writer.save()

    elif to_text==True:

        with open(os.path.join(path, file_name + '.txt'), 'w') as outputFile:

            for i in range(len(dataframe_list)):
                #if dataframe_list[i] != None:
                outputFile.write('name= '+name_list[i]+'\n')
                outputFile.write(dataframe_list[i].to_csv(sep='\t', columns=key_order_list[i], index=False, header=True))    
    
       
