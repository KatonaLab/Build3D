import time
import numpy as np
import collections

from . import segmentation
from . import core
from .imageclass import VividImage

   
        
        
def tagImage(image):

    '''
    Function that runs ITK connected components on input image
    :param image: nd Array
    :param outputImage: nd Array
    '''

    # Start timing
    tstart = time.clock()

    # Creatre LogText and start logging
    logText = '\nRunning connected components on : ' + str(image.metadata['Name'])

    #Tag image
    output_array=segmentation.tag_image(image.get_3d_array())
    
    #Create metadata ditionary and set type to match tagged image
    output_metadata=image.metadata
    image.metadata['Type']=str(output_array.dtype)
    

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return VividImage(output_array, output_metadata), logText


def threshold(image, method="Otsu", **kwargs):
    '''

    :param image:
    :param imageDictionary:
    :param method:
    :param kwargs:
        lowerThreshold, upperThreshold, mode,blockSize=5, offSet=0

    :return:
        LogText
    '''
    
    # Start timing
    tstart = time.clock()

        

    # Threshold methods
    auto_list = ['Otsu', 'Huang', 'IsoData', 'Li', 'MaxEntropy', 'KittlerIllingworth', 'Moments', 'Yen',
                         'RenyiEntropy', 'Shanbhag', 'Triangle']
    adaptive_list = ['Adaptive Mean', 'Adaptive Gaussian']


    # Creatre LogText and start logging
    logText = '\nThresholding: '+image.metadata['Name']
    logText += '\n\tMethod: ' + method

    # Parse kwargs
    if kwargs != {}:
        if method in auto_list:
            keyList=['mode']
        elif method in adaptive_list:
            keyList = ['blockSize', 'offSet']
        elif method == 'Manual':
            keyList =['lower', 'upper']
        else:
            raise KeyError('Thresholding method '+str(method)+' not available or valid!')

        kwargs = {your_key: kwargs[your_key] for your_key in keyList if your_key in kwargs}
    

    # Run thresholding functions    
    if method in auto_list:
        output_array, thresholdValue = segmentation.threshold_auto(image.get_3d_array(), method, **kwargs)
        logText += '\n\tThreshold values: ' + str(thresholdValue)

    elif method in adaptive_list:
        logText += '\n\tSettings: ' + str(kwargs)
        output_array = segmentation.threshold_adaptive(image.get_3d_array(), method, **kwargs)

    elif method == 'Manual':
        logText += '\n\tSettings: ' + str(kwargs)
        output_array = segmentation.threshold_manual(image.get_3d_array(), **kwargs)

    else:
        raise LookupError("'" + str(method) + "' is Not a valid mode!")

    
    output_metadta=image.metadata
    output_metadta['Type']=str(output_array.dtype)
 
    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '
    
    return VividImage(output_array, image.metadata), logText


def analyze(tagged_image, image_list=None, measurementInput=['voxelCount', 'meanIntensity']):
    '''
    Analyzes tagedImage and appends 'database' to its dictionary that contain measured values.
    :param tagged_img: tagged image
    :param taggedDictionary: dictionary with descriptors of tagged image
    :param imageList: image list where intensity is measured within objects of tagged_img
    :param dictionaryList: list of dictionaries that apartain to each element in imageList
    :param outputImage: output image
    :param outputDictionary: dictionary with descriptors of outputImage
    :return:
    '''

    # Start timing
    tstart = time.clock()

    # Creatre LogText and start logging
    logText = '\nAnalyzing: ' + str(tagged_image.metadata['Name'])


    #Print list of images in Imagelist to log text
    if image_list != None:
        logText += '\n\tMeasuring intensity in: '
        for img in image_list:
            logText += img.metadata['Name']

    #Analyze image
    tagged_img=core.analyze(tagged_image, image_list, measurementInput)

    #Add number of objects to logText
    logText += '\n\tNumber of objects: '+str(len(tagged_img.database['tag']))

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '
    
    return tagged_img, logText


def apply_filter(image, filter_dict=None, remove_filtered=False, overwrite=True):
    '''
    Filters dictionary stored in the 'database' key of the inputDisctionary to be filtered and removes filtered taggs if filterImage=True. Boolean mask is appended to inputDictionary['database']
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
    
    # Start timing
    tstart = time.clock()

    # Creatre LogText and start logging
    logText = '\nFiltering: ' + str(image.metadata['Name'])
    logText += '\n\tFilter settings: '+str(filter_dict).replace('{', ' ').replace('}', ' ')
    logText += '\n\t\tremoveFiltered=' + str(remove_filtered)
    logText += '\n\t\toverwrite=' + str(overwrite)


    if filter_dict==None:
        filter_dict={}

    # Filter dictionary
    output_database=core.filter_database(image.database, filter_dict, overwrite)
    output_image=image.image
    
    # Remove Filtered objects from database and image
    if remove_filtered == True:
        output_image , output_database = core.remove_filtered(image.image, output_database)
            

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '
    
    
    return VividImage(output_image, image.metadata, output_database) , logText


def colocalization(tagged_img_list, sourceImageList=None, overlappingFilter=None,
                   removeFiltered=False, overWrite=True):
    '''

    :param tagged_img_list:
    :param taggedDictList:
    :param sourceImageList:
    :param overlappingFilterList:
    :param filterImage:
    :return:
    '''
    # Start timingsourceDictionayList
    tstart = time.clock()

    # Creatre LogText
    logText = '\nColocalization analysis started using: '
    for img in tagged_img_list:
        logText += '\t ' + str(img.metadata['Name'])

    # Add Filter settings
    logText += '\n\tFilter settings: ' + str(overlappingFilter).replace('{', ' ').replace('}', ' ')
    logText += '\n\t\tremoveFiltered=' + str(removeFiltered)
    logText += '\n\t\toverwrite=' + str(overWrite)

    # Determine connectivity data
    overlappingImage = core.colocalization_connectivity(tagged_img_list, sourceImageList)
    
    # Filter database and image
    overlappingImage, _ = apply_filter(overlappingImage, overlappingFilter, removeFiltered)
    
    # Analyze colocalization
    overlappingImage, _ = core.colocalization_analysis(tagged_img_list, overlappingImage)


    #Print number of objects to logText
    logText += '\n\tNumber of Overlapping Objects: '+str(len(overlappingImage.database['tag']))

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return overlappingImage, logText


def save_data(image_list, path, file_name='output', to_text=True):
    '''
    :param dictionaryList: Save dictionaries in inputDictionaryList
    :param path: path where file is saved
    :param toText: if True data are saved to text
    :param fileName: fileneme WITHOUT extension
    :return:
    '''
    

    
    # Start timing
    tstart = time.clock()
    
    #If input is not list create list
    if not isinstance(image_list, collections.Iterable):
        image_list=[image_list]
        
    # Creatre LogText and start logging
    logText = '\nSaving database: '
    # Add names of dictionary sources to logText
    for img in image_list:
        logText += '\t' + str(img.metadata['Name'])
    
    #Add settings to logText
    # Add filter settings to logText
    logText += '\n\tPath: '+str(path)
    logText += '\n\tFilename: '+str(file_name)
    if to_text==True: logText += '.txt'
    elif to_text==False:logText += '.xlsx'

    core.save_data(image_list, path, file_name, to_text)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return logText


def save_image(image_list, path, file_name):
    
    # Start timing
    tstart = time.clock()
    
    #If input is not list create list
    if not isinstance(image_list, collections.Iterable):
        image_list=[image_list]
    
    # Creatre LogText and start logging
    logText = '\nSaving image: '
    logText += '\t' + str([x.metadata['Name'] for x in image_list])
    logText += '\n\tPath: '+str(path)
    logText += '\n\tFile Name: '+str(file_name)
    
    #Save image
    core.save_image(image_list, path, file_name) 

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return logText