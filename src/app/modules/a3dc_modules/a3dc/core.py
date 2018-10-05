import SimpleITK as sitk
import numpy as np
import pandas as pd
import copy
import os


from .imageclass import VividImage
from . import segmentation
from .utils import reorder_list

'''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
	the workflows to process images.
	'''
def overlap_image(array_list):

    # Create Overlapping Image
    output_array = array_list[0]
    for i in range(1, len(array_list)):
        output_array = np.multiply(output_array, array_list[i])

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
    array_list=[x.get_3d_array() for x in image_list]
    ovl_array = overlap_image(array_list)
  
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
    itk_img = tagged_img.to_itk()

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

        itk_raw = img_list[i].to_itk()
        
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
        return tagged_Img, database
    
    
        
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
        numeric_keys=reorder_list(numeric_keys,preset_order)
        preset_order = ['centroid']
        other_keys=reorder_list(other_keys,preset_order)
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
    
    
    
def save_image(img_list, path, file_name):
    '''
    '''

    #Combine images
    for idx, img in  enumerate(img_list):

        if idx==0:
            output=copy.deepcopy(img)
        else:
            output.append_to_dimension(img)
    
    #save image
    output.save(path, file_name)