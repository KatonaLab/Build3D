import SimpleITK as sitk
import numpy as np
import pandas as pd
import copy
import sys


from .imageclass import VividImage
from . import segmentation

'''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
	the workflows to process images.
	'''
def overlap_image(img_list):


    # Create Overlapping Image
    output_array = img_list[0].image
    for i in range(1, len(img_list)):
        output_array = np.multiply(output_array, img_list[i].image)

    return output_array

def convert_array_type(array, dtype):
    
    #print('Warning: Image type has been converted from '
                 #+str(array.dtype)+' to '+str(dtype)+'!', file=sys.stderr) 
    array=array.astype(dtype)
    
    return array

def colocalization_connectivity(image_list, raw_img_list=None):
    '''TODO what happens if tagged image was not previouslz analyzed
    '''
    #Create list of names
    name_list = [x.metadata['Name'] for x in image_list] 
    
    
    # Create Overlapping Image
    ovl_array = overlap_image(image_list)
  
    #Create overlapping image metadata
    metadata=copy.deepcopy(image_list[0].metadata)
    metadata['Name']=str.join('_', name_list)
    #TempTempTemp
    if 'NormFactor' in metadata.keys():
            del metadata['NormFactor']
    
    #Create overlapping image
    ovl_image=VividImage(segmentation.tag_image(ovl_array), metadata)
    
    if raw_img_list==None:
        raw_img_list=[ovl_image]

    else:
        raw_img_list.append(ovl_image)
    
    ovl_image=analyze(ovl_image, raw_img_list)

    
    # Generate array lists and name lists
    

    for i in range(len(image_list)):
        itk_image = sitk.GetImageFromArray(image_list[i].image)

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
  
    return ovl_image


def colocalization_analysis(image_list, ovl_img):
    

    database_list=[x.database for x in image_list]
    ovl_database=ovl_img.database

    name_list = [x.metadata['Name'] for x in  image_list]


    obj_no = [len(x.database['tag']) for x in image_list]
    input_element_no=len(database_list)

    ovl_is_filtered = 'filter' in ovl_database.keys()
    dict_is_filtered = [('filter' in x.keys()) for x in database_list]
    
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



def analyze(tagged_img, img_list=None, meas_list=['voxelCount', 'meanIntensity']):

    #Convert tagged image to ITK image
    itk_img = image_to_itk(tagged_img)

    # Instatiate ITK LabelIntensityStatisticsImageFilter()
    itk_filter = sitk.LabelIntensityStatisticsImageFilter()

    single_img_functions = {'volume': itk_filter.GetPhysicalSize,
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

    multi_img_functions = {'meanIntensity': itk_filter.GetMean,
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

    single_img_meas_list = ['volume', 'voxelCount', 'pixelsOnBorder', 'centroid']
    multi_img_meas_list = ['meanIntensity', 'maximumPixel']

    if meas_list!=None:
        for key in meas_list:

            if key == 'getFeretDiameter':
                itk_filter.ComputeFeretDiameterOn()

            if key in ['getPerimeter', 'getPerimeterOnBorder', 'getPerimeterOnBorderRatio','getEquivalentSphericalPerimeter']:
                itk_filter.ComputePerimeterOn()

            if (key in single_img_functions.keys()) and (key not in single_img_meas_list):
                single_img_meas_list.append(key)

            elif (key in multi_img_functions.keys()) and (key not in multi_img_meas_list):
                multi_img_meas_list.append(key)

    # database dictionary to store results
    database = {'tag': [], }
    
    for key in single_img_meas_list:
        database[key] = []

    #Cycle through images to measure parameters
    if img_list == None:
        img_list = []
        img_list.append(tagged_img)

    for i in range(len(img_list)):

        itk_raw = image_to_itk(img_list[i])
        
        for key in multi_img_meas_list:
            database[key+' in '+img_list[i].metadata['Name']] = []

        #Execute Filter and get database
        itk_filter.Execute(itk_img, itk_raw)
        data=itk_filter.GetLabels()

        for label in data:
           
            if i==0:
                database['tag'].append(label)
                for key in single_img_meas_list:
                    database[key].append(single_img_functions[key](label))

            for key in multi_img_meas_list:
                database[key+' in '+img_list[i].metadata['Name']].append(multi_img_functions[key](label))
    
    
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
            database['surface'].append(single_img_functions['voxelCount'](label))
            
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
    
    if 'filter' in df.keys() and overwrite==True:
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



def remove_filtered(tagged_Img, database):

    change_dict={}

    if 'filter' in database.keys():

        for i in range(len(database['filter'])):#database should have a label key!!!
            if database['filter'][i]==False:
                change_dict[int(database['tag'][i])]=0
    
        itk_img = sitk.GetImageFromArray(tagged_Img)

        sitk_filter = sitk.ChangeLabelImageFilter()
        sitk_filter.SetChangeMap(change_dict)
        
        output_image=sitk.GetArrayFromImage(sitk_filter.Execute(itk_img))
        
        #Remove filtered from dictionary
        df = pd.DataFrame(database)
        df.drop(df[df['filter'] == False].index, inplace=True)
    
        output_database = df.to_dict(orient='list')
        
        return output_image , output_database
       
        
    else:
        return tagged_Img, database
    
def image_to_itk(image):
    #####!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!######### 
    image.reorder('ZXYCT')
    itk_img = sitk.GetImageFromArray(image.image[0,0])
    
    #Check if physical size metadata is available if any is missing raise Exeption
    size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
    missing_size=[s for s in size_list if s not in image.metadata.keys()]
    if len(missing_size)!=0:
        print('Missing :'+str(missing_size)+'! Unable to carry out analysis!', file=sys.stderr)
    else:
        itk_img.SetSpacing((image.metadata['PhysicalSizeX'],image.metadata['PhysicalSizeY'],image.metadata['PhysicalSizeZ']))
    
    #Check if unit metadata is available
    unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
    missing_unit=[u for u in unit_list if u not in image.metadata.keys()]
    if len(missing_size)!=0:
        print('Warning: DEFAULT value (um or micron) used for :'
             +str(missing_unit)+'!', file=sys.stderr)

    return itk_img    
        
    
    
    
    
    