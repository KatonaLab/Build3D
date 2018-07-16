# -*- coding: utf-8 -*-
"""
Created on Sun Mar 25 23:17:33 2018

@author: Nerberus
"""

import time
import numpy as np
import collections

from . import segmentation
from . import core
from .error import A3dcError as A3dcError
from .imageclass import Image

####################################################Interface to call from C++####################################################
def colocalize(ch1Array, ch1ThrArray, ch1Metadata, ch1Settings,  ch2Array, ch2ThrArray, ch2Metadata, ch2Settings, ovlSettings):
   
        #Parameters to measure
        measurementList = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder', 'meanIntensity', 'variance']
        
        
        ch1Img=Image(ch1Array, ch1Metadata)
        ch2Img=Image(ch2Array, ch2Metadata)
        thresholdedImage1=Image(ch1ThrArray, ch1Metadata)
        thresholdedImage2=Image(ch2ThrArray, ch2Metadata)
    
        outputPath=ch1Metadata['Path']
        #############################################################################################################################
        ###########################################Segmentation and Analysis#########################################################
        #############################################################################################################################

        #######################################################Channel 1#############################################################


        #Channel 1:Tagging Image
        taggedImage1, logText = tagImage(thresholdedImage1)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage1, logText = analyze(taggedImage1, imageList=[ch1Img], measurementInput=measurementList)
        print(logText)
        
        taggedImage1, logText = apply_filter(taggedImage1, filterDict=ch1Settings, removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        


        #######################################################Channel 2#############################################################
       


        #Channel 1:Tagging Image
        taggedImage2, logText = tagImage(thresholdedImage2)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage2, logText = analyze(taggedImage2, imageList=[ch2Img], measurementInput=measurementList)
        print(logText)

        
        taggedImage2, logText = apply_filter(taggedImage2, filterDict=ch2Settings, removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        

        

        #############################################################################################################################
        ###################################################Colocalization############################################################
        #############################################################################################################################
        

        #Colocalization:
        overlappingImage, taggedImageList, logText = colocalization( [taggedImage1, taggedImage2], overlappingFilter=ovlSettings, removeFiltered=False)
        print(logText)

        suffix = ch1Img.metadata['Name']+ "_" +ch2Img.metadata['Name']+ "_overlap"
        save_image(overlappingImage, outputPath, suffix)
        
        logText='\nSaving object dataBases to xlsx and text!'
        print(logText)

        
        name1=ch1Img.metadata['Name']+'_tagged'
        name2=ch2Img.metadata['Name']+'_tagged'
        name=ch1Img.metadata['Name']+'_'+ch2Img.metadata['Name']
        
        save_data([taggedImage1,taggedImage2, overlappingImage], path=outputPath, file_name=name+'.xlsx', to_text=False)
        save_data([taggedImage1], path=outputPath, file_name=name1+'.txt', to_text=True)
        save_data([taggedImage1], path=outputPath, file_name=name2+'.txt', to_text=True)

####################################################Interface to call from Python scripts####################################################
        
        
        
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

    try:

    
        outputArray=segmentation.tag_image(image.array)
      

    except Exception as e:
        raise A3dcError("Error occured while tagging image!",e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return Image(outputArray, image.metadata), logText


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
    autothresholdList = ['Otsu', 'Huang', 'IsoData', 'Li', 'MaxEntropy', 'KittlerIllingworth', 'Moments', 'Yen',
                         'RenyiEntropy', 'Shanbhag', 'Triangle']
    adaptiveThresholdList = ['Adaptive Mean', 'Adaptive Gaussian']


    # Creatre LogText and start logging
    logText = '\nThresholding: '+image.metadata['Name']
    logText += '\n\tMethod: ' + method

    # Parse kwargs
    if kwargs != {}:
        if method in autothresholdList:
            keyList=['mode']
        elif method in adaptiveThresholdList:
            keyList = ['blockSize', 'offSet']
        elif method == 'Manual':
            keyList =['lower', 'upper']
        else:
            raise KeyError('Thresholding method '+str(method)+' not available or valid!')

        kwargs = {your_key: kwargs[your_key] for your_key in keyList if your_key in kwargs}
        
    # Run thresholding functions
    try:
        if method in autothresholdList:
            outputArray, thresholdValue = segmentation.threshold_auto(image.array, method, **kwargs)
            logText += '\n\tThreshold values: ' + str(thresholdValue)

        elif method in adaptiveThresholdList:
            logText += '\n\tSettings: ' + str(kwargs)
            outputArray = segmentation.threshold_adaptive(image.array, method, **kwargs)

        elif method == 'Manual':
            logText += '\n\tSettings: ' + str(kwargs)
            outputArray = segmentation.threshold_manual(image.array, **kwargs)

        else:
            raise LookupError("'" + str(method) + "' is Not a valid mode!")

    except Exception as e:
        raise A3dcError("Error occured while thresholding image!",e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return Image(outputArray, image.metadata), logText


def analyze(tagged_img, imageList=None, measurementInput=['voxelCount', 'meanIntensity']):
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
    logText = '\nAnalyzing: ' + str(tagged_img.metadata['Name'])

    try:
        #Print list of images in Imagelist to log text
        if imageList != None:
            logText += '\n\tMeasuring intensity in: '
            for img in imageList:
                logText += img.metadata['Name']

        #Analyze image
        tagged_img=core.analyze(tagged_img, imageList, measurementInput)

        #Add number of objects to logText
        logText += '\n\tNumber of objects: '+str(len(tagged_img.database['tag']))
        
        

    except Exception as e:
        raise A3dcError("Error occured while analyzing image!",e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '
    
    return tagged_img, logText


def apply_filter(image, filterDict=None, removeFiltered=False, overWrite=True):
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
    logText += '\n\tFilter settings: '+str(filterDict).replace('{', ' ').replace('}', ' ')
    logText += '\n\t\tremoveFiltered=' + str(removeFiltered)
    logText += '\n\t\toverwrite=' + str(overWrite)

    try:
        if filterDict==None:
            filterDict={}

        # Filter dictionary
        output_database=core.filter_database(image.database, filterDict, overWrite, removeFiltered)

        # Create/Filter image
        if removeFiltered == True:
            output_image = core.filter_image(image)
        else:
            output_image=Image(image.array, image.metadata)

        #Add database to image
        output_image.database=output_database
    
    except Exception as e:
        raise A3dcError("Error occured while filtering database!",  e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '
    
    
    return output_image , logText


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

    try:

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

    except Exception as e:
        raise A3dcError("Error occured while filtering database!",e)

        # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return overlappingImage, tagged_img_list, logText


def save_data(image_list, path, file_name='output', to_text=True):
    '''
    :param dictionaryList: Save dictionaries in inputDictionaryList
    :param path: path where file is saved
    :param toText: if True data are saved to text
    :param fileName: fileneme WITHOUT extension
    :return:
    '''
    
    if not isinstance(image_list, collections.Iterable):
            image_list=[image_list]
    
    # Start timing
    tstart = time.clock()
    
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
    

    try:

        Image.save_data(image_list, path, file_name, to_text)

    except Exception as e:
        raise A3dcError("Error occured while filtering database!",e)



    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return logText


def save_image(img, path, file_name):
    
    
    # Start timing
    tstart = time.clock()
    
    # Creatre LogText and start logging
    logText = '\nSaving image: '
    # Add names of dictionary sources to logText

    logText += '\t' + str(img.metadata['Name'])
    #Add settings to logText
    # Add filter settings to logText
    logText += '\n\tPath: '+str(path)

    try:
        #Save image using tifffile save
        Image.save_image(img, path, file_name) 
        
    except Exception as e:
        raise A3dcError("Error occured while filtering database!",e)



    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return logText