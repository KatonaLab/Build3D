import SimpleITK as sitk
import sys
import cv2
import numpy as np
from math import pow
from skimage import img_as_ubyte, exposure
from skimage.external.tifffile import imread, imsave, TiffWriter
from skimage.filters import threshold_local
import pandas as pd
from scipy.ndimage.filters import convolve
import time
import os
import multiprocessing as mp
import copy
from itertools import chain
import matplotlib.pyplot as plt
import pickle
from operator import add
import xml.etree.cElementTree as ET


####################################################Interface to call from C++####################################################
def tagImage(inputImage, outputImage):
    '''
    Function that runs ITK connected components on input image
    :param image: nd Array
    :param outputImage: nd Array
    '''
    outputImage=Segmentation.tag_image(image)

def autoThreshold(inputImage, method, outputImage):
    '''
    Apply autothreshold slice by slice. For later use there is an implementation that uses stack histograms
    and one that uses the mean of the slice by slice threshold values.
    :param inputImage: nd array
    :param method: threshold method name as string
            * 'Otsu'
            * 'Huang'
            * 'IsoData'
            * 'Li'
            * 'MaxEntropy'
            * 'KittlerIllingworth'
            * 'Moments'
            * 'Yen'
            * 'RenyiEntropy'
            * 'Shanbhag'
    :return:
    '''

    outputImage=Segmentation.threshold2D_auto(inputImage, method, outputImage)

def analyze(taggedImage, taggedDictionary, outputDictionary, outputImage, imageList=[], dictionaryList=[]):
    '''
    Analyzes tagedImage and appends 'dataBase' to its dictionary that contain measured values.
    :param taggedImage: tagged image
    :param taggedDictionary: dictionary with descriptors of tagged image
    :param imageList: image list where intensity is measured within objects of taggedImage
    :param dictionaryList: list of dictionaries that apartain to each element in imageList
    :param outputImage: output image
    :param outputDictionary: dictionary with descriptors of outputImage
    :return:
    '''

    outputDictionary =Measurement.analyze(taggedImage, taggedDictionary, imageList, dictionaryList)



def filter(inputImage ,inputDictionary, outputImage, outputDictionary, filterDict,  removeFiltered=False, overWrite=True, filterImage=False ):
    '''
    Filters dictionary stored in the 'dataBase' key of the inputDisctionary to be filtered and removes filtered taggs if filterImage=True. Boolean mask is appended to inputDictionary['Database']
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

    #Filter dictionary
    outputDictionary=Measurement.filter_dataBase(inputDictionary, filterDict, removeFiltered, overWrite)
    #Filter image
    if filterImage==True:
        outputImage=Measurement.filter_image(inputImage, outputDictionary)
    else:
        outputImage=inputImage


def colocalization(taggedImgList, taggedDictList, sourceImageList=[], sourceDictionayList=[], name=None):
    #Create overlapping image and overlapping dictionary
    overlappingImage, overlappingDataBase=Measurement.colocalization_overlap(taggedImgList, taggedDictList, sourceImageList, sourceDictionayList, name)
    #Filter overlapping image/dictionary

    overlappingDataBase=Measurement.colocalization_connectivity(taggedImgList, dataBaseList, overlappingDataBase)
    dataBaseList, overlappingDataBase=Measurement.colocalizaion_analysis(taggedImgList, dataBaseList, overlappingImage, overlappingDataBase)


def save(inputDictionaryList, path, fileName='output', toText=True):
    '''
    :param dictionaryList: Save dictionaries in inputDictionaryList
    :param path: path where file is saved
    :param toText: if True data are saved to text
    :param fileName: fileneme WITHOUT extension
    :return:
    '''

    Measurement.save(inputDictionaryList, path, fileName, toText)

#############################################Class to use as sandbox for python####################################################
class Main(object):

    def __init__(self):

        tstart = time.clock()

        '''
        Path = ("D:/OneDrive - MTA KOKI/Workspace/Playground/ImageTest/3ch_ome.tif")
        img = sitk.ReadImage(Path)
        metadataKeys=img.GetMetaDataKeys()
        print(metadataKeys)
        root = ET.fromstring(img.GetMetaData('ImageDescription'))
        print(root)
        for child in root:
            print('child: ')
            print(child)
            for child2 in child:
                print("################################################################################################")
                print(child2.tag)
                print(child2.attrib)

        print(root.findall("item"))
        #print(img.GetMetaData('ImageDescription'))
        for key in metadataKeys:
            print("################################################################################################")
            #print(img.GetMetaData(key))
        #img=Processor.load_image(Path)
        #print(img[0])
        '''

        #############################################Load Images####################################################
        sourceImageList=[]
        sourceDictList=[]
        # Channel 1

        ch1Path = ("D:/OneDrive - MTA KOKI/Workspace/Playground/img_1.tif")
        #ch1Path = ('F:/Workspace/TestImages/test_1.tif')
        ch1Img=Processor.load_image(ch1Path)
        ch1Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch1Img)
        sourceDictList.append(ch1Dict)


        # Channel 2
        ch2Path =("D:/OneDrive - MTA KOKI/Workspace/Playground/img_2.tif")
        #ch2Path =('F:/Workspace/TestImages/test_2.tif')

        ch2Img = Processor.load_image(ch2Path)

        #b=np.array([65535,65535/2, 50], dtype=np.uint16)
        #b=exposure.rescale_intensity(b,out_range='uint8')
        #b = img_as_ubyte(ch2Img)
        #exposure.equalize_hist(b)
        b=Segmentation.threshold_adaptive(ch2Img[0], 'Adaptive Gaussian', blockSize=5, offSet=0)



        ch2Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch2Img)
        sourceDictList.append(ch2Dict)

        # Channel 3
        ch3Path = ("D:/OneDrive - MTA KOKI/Workspace/Playground/img_3.tif")
        #ch3Path =('F:/Workspace/TestImages/test_3.tif')
        ch3Img = Processor.load_image(ch3Path)

        ch3Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch3Img)
        sourceDictList.append(ch3Dict)
        #############################################################################################################


        #######################################Create Tagged Image List##############################################
        taggedImageList = []
        taggedDictList = []

        # Channel 1
        thresholdedImage1=Segmentation.threshold_auto(sourceImageList[0], method="MaxEntropy", mode="3D")
        taggedImage1=Segmentation.tag_image(thresholdedImage1)

        taggedDict1=sourceDictList[0]
        taggedDict1=Measurement.analyze(taggedImage1, taggedDict1, imageList=sourceImageList, dictionaryList=sourceDictList)
        taggedDict1['name']='Channel1'

        taggedImageList.append(taggedImage1)
        taggedDictList.append(taggedDict1)


        # Channel 2
        thresholdedImage2 = Segmentation.threshold_auto(sourceImageList[1], method="MaxEntropy", mode="3D")
        taggedImage2 = Segmentation.tag_image(thresholdedImage2)

        taggedDict2=sourceDictList[1]
        taggedDict2 = Measurement.analyze(taggedImage2, taggedDict2, imageList=[sourceImageList[1]], dictionaryList=sourceDictList)
        taggedDict2['name']='Channel2'

        taggedImageList.append(taggedImage2)
        taggedDictList.append(taggedDict2)

        # Channel 3
        thresholdedImage3 = Segmentation.threshold_auto(sourceImageList[2], method="Otsu", mode="2D")
        taggedImage3 = Segmentation.tag_image(thresholdedImage3)

        taggedDict3 =sourceDictList[2]
        taggedDict3 = Measurement.analyze(taggedImage3, taggedDict3, imageList=sourceImageList, dictionaryList=sourceDictList)
        taggedDict3['name'] = 'Channel3'

        taggedImageList.append(taggedImage3)
        taggedDictList.append(taggedDict3)
        #############################################################################################################

        #############################################Analysis Images####################################################

        #Filter database
        #dictFilter={'volume':{'min':2, 'max':11}}#, 'mean in '+taggedDictList[0]['name']: {'min':2, 'max':3}}

        #taggedDictList[1]=Measurement.filter_dataBase(taggedDictList[1], {'tag':{'min':2,'max':3}}) #'ellipsoidDiameter':{'min':2,'max':3}})


        #Colocalization analysis
        overlappingImage, overlappingDataBase=Measurement.colocalization_overlap(taggedImageList, taggedDictList, sourceImageList=sourceImageList, sourceDictionayList=sourceDictList)
        overlappingDataBase=Measurement.colocalization_connectivity(taggedImageList, taggedDictList, overlappingDataBase)
        taggedDataBaseList, overlappingDataBase=Measurement.colocalizaion_analysis(taggedImageList, taggedDictList, overlappingImage, overlappingDataBase)

        #print(overlappingDataBase)

        #Save results
        taggedDictList.append(overlappingDataBase)
        Measurement.saveData(taggedDictList, "D:/OneDrive - MTA KOKI/Workspace/Playground", toText=False)

        taggedImageList.append(overlappingImage)
        print(type(taggedImageList[0]))
        print(taggedImageList)
        Measurement.saveImage(taggedImageList, "D:/OneDrive - MTA KOKI/Workspace/Playground")
       #Processor.imsave(path, nparray)



        tstop = time.clock()
        print('ITK STATS: ' + str(tstop - tstart))







#############################################Class that contain main functions for A3DC###################################################
class Measurement(object):
    '''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
    	the workflows to process images.
    	'''

    @staticmethod
    def filter_image(taggedImg, taggedDict):

        dataBase=taggedDict['dataBase']
        changeDict={}
        if 'filter' in dataBase.keys():
            for i in range(len(dataBase['filter'])):#dataBase should have a label key!!!
                if dataBase['filter'][i]==False:
                    changeDict[int(dataBase['tag'])]=0

            # change label
            itkImage = sitk.GetImageFromArray(taggedImg)

            sitkFilter = sitk.ChangeLabelImageFilter()
            sitkFilter.SetChangeMap(changeDict)

            outputImage = sitk.GetArrayFromImage(sitkFilter.Execute(itkImage))
        else:
            outputImage = taggedImg

        return outputImage




    @staticmethod
    def colocalization_overlap(taggedImgList, taggedDictList, sourceImageList=[], sourceDictionayList=[], name=None):

        #Create Overlapping Image
        overlappingImage = Segmentation.tag_image(Segmentation.create_overlappingImage(taggedImgList))

        # Create Overlapping dataBase
        if name==None:
            name='Overlapping'
            for dict in taggedDictList:
                name=name+'_'+dict['name']
        overlappingDataBase = {'name': name}

        sourceImageList.append(overlappingImage)
        sourceDictionayList.append(overlappingDataBase)

        overlappingDataBase=Measurement.analyze(overlappingImage, overlappingDataBase, sourceImageList, sourceDictionayList)

        return overlappingImage, overlappingDataBase

    @staticmethod
    def colocalization_connectivity(taggedImgList, dataBaseList, overlappingDataBase):

        # Generate array lists and name lists
        nameList = [x['name'] for x in dataBaseList]

        for i in range(len(taggedImgList)):
            itk_image = sitk.GetImageFromArray(taggedImgList[i])

            #Get pixel from database
            overlappingPixels=overlappingDataBase['dataBase']['maximumPixel in '+overlappingDataBase['name']]

            objectList = [None for i in range(len(overlappingPixels))]
            ovlRatioList = [None for i in range(len(overlappingPixels))]

            for j in range(len(overlappingPixels)):
                ovlPosition = overlappingPixels[j]
                objectList[j] = itk_image.GetPixel(ovlPosition)
                ovlRatioList[j] = overlappingDataBase['dataBase']['voxelCount'][j] / dataBaseList[i]['dataBase']['voxelCount'][objectList[j] - 1]

            overlappingDataBase['dataBase']['object in ' + nameList[i]] = objectList
            overlappingDataBase['dataBase']['overlappingRatio in ' + nameList[i]] = ovlRatioList

        return overlappingDataBase

    @staticmethod
    def colocalizaion_analysis( taggedImgList, dictionaryList, overlappingImage, overlappingDictionary):

        dataBaseList=[x['dataBase'] for x in dictionaryList]
        overlappingDataBase=overlappingDictionary['dataBase']
        nameList = [x['name'] for x in dictionaryList]
        NbObjects = [len(x['dataBase']['tag']) for x in dictionaryList]
        NbOfInputElements=len(dataBaseList)

        ovlFiltered = 'filter' in overlappingDataBase.keys()
        dictFiltered = [('filter' in x.keys()) for x in dataBaseList]

        # Create list of dictionaries to store data
        outputList=[{} for x in range(NbOfInputElements)]
        for i in range(NbOfInputElements):

            outputList[i]['colocalizationCount']=[0  for i in range(NbObjects[i])]
            outputList[i]['totalOverlappingRatio'] = [0  for i in range(NbObjects[i])]

            positionList = [x for x in range(NbOfInputElements) if x != i]
            for j in range(len(positionList)):
                outputList[i]['objects in '+nameList[positionList[j]]]=[[] for i in range(NbObjects[i])]

        for j in range(len(overlappingDataBase['tag'])):

            #Determine if any of the objects have been filtered out and retrieve tags and their rispected position
            flag=True# = [True for i in range(len(dataBaseList))]

            if ovlFiltered==True:
                flag=overlappingDataBase['dataBase']['filter'][j]

            currentTagList=[]
            currentPositionList=[]
            for i in range(NbOfInputElements):
                currentTag=overlappingDataBase['object in ' + nameList[i]][j]
                currentTagList.append(currentTag)
                currentPositionList.append(dataBaseList[i]['tag'].index(currentTag))

                if dictFiltered[i]==True:
                    flag*=dataBaseList[i]['filter'][j]

            #If none of the objects have been filtered out add info to output
            if flag==True:
                for i in range(NbOfInputElements):

                    outputList[i]['colocalizationCount'][currentPositionList[i]]+=1
                    outputList[i]['totalOverlappingRatio'][currentPositionList[i]] += overlappingDataBase['overlappingRatio in ' + nameList[i]][j-1]

                    positionList = [x for x in range(NbOfInputElements) if x != i]
                    for j in range(len(positionList)):
                        outputList[i]['objects in ' + nameList[positionList[j]]][currentPositionList[i]].append(currentTagList[positionList[j-1]])

        for i in range(NbOfInputElements):
            for key in outputList[i]:
                dictionaryList[i]['dataBase'][key]=outputList[i][key]

                overlappingDictionary['dataBase']=overlappingDataBase

        return dictionaryList, overlappingDictionary


    @staticmethod
    def analyze(taggedImage, dictionary, imageList=[], dictionaryList=[], measurementInput = ['surface']):



        #Convert tagged image to ITK image
        itkImage = sitk.GetImageFromArray(taggedImage)

        # Instatiate ITK LabelIntensityStatisticsImageFilter()
        itkFilter = sitk.LabelIntensityStatisticsImageFilter()

        singleChannelFunctionDict = {'volume': itkFilter.GetPhysicalSize,
                                     'voxelCount': itkFilter.GetNumberOfPixels,
                                     'centroid': itkFilter.GetCentroid,
                                     'ellipsoidDiameter': itkFilter.GetEquivalentEllipsoidDiameter,
                                     'boundingBox': itkFilter.GetBoundingBox,
                                     'pixelsOnBorder': itkFilter.GetNumberOfPixelsOnBorder,
                                     'getElongation': itkFilter.GetElongation,
                                     'getEquivalentSphericalRadius': itkFilter.GetEquivalentSphericalRadius,
                                     'getFlatness': itkFilter.GetFlatness,
                                     'getPrincipalAxes': itkFilter.GetPrincipalAxes,
                                     'getPrincipalMoments': itkFilter.GetPrincipalMoments,
                                     'getRoundness': itkFilter.GetRoundness,
                                     'getPrincipalAxes': itkFilter.GetPrincipalAxes,
                                     'getFeretDiameter': itkFilter.GetFeretDiameter,
                                     'getPerimeter': itkFilter.GetPerimeter,
                                     'getPerimeterOnBorder': itkFilter.GetPerimeterOnBorder,
                                     'getPerimeterOnBorderRatio': itkFilter.GetPerimeterOnBorderRatio,
                                     'getEquivalentSphericalPerimeter': itkFilter.GetEquivalentSphericalPerimeter}

        dualChannelFunctionDict = {'meanIntensity': itkFilter.GetMean,
                                   'medianIntensity': itkFilter.GetMedian,
                                   'skewness': itkFilter.GetSkewness,
                                   'kurtosis': itkFilter.GetKurtosis,
                                   'variance': itkFilter.GetVariance,
                                   'maximumPixel': itkFilter.GetMaximumIndex,
                                   'maximumValue': itkFilter.GetMaximum,
                                   'minimumValue': itkFilter.GetMinimum,
                                   'minimumPixel': itkFilter.GetMaximumIndex,
                                   'centerOfMass': itkFilter.GetCenterOfGravity,
                                   'standardDeviation': itkFilter.GetStandardDeviation,
                                   'cumulativeIntensity': itkFilter.GetSum,
                                   'getWeightedElongation': itkFilter.GetWeightedElongation,
                                   'getWeightedFlatness': itkFilter.GetWeightedFlatness,
                                   'getWeightedPrincipalAxes': itkFilter.GetWeightedPrincipalAxes,
                                   'getWeightedPrincipalMoments': itkFilter.GetWeightedPrincipalMoments}

        singleChMeasurementList = ['voxelCount', 'pixelsOnBorder', 'centroid']
        dualChMeasurementList = ['meanIntensity', 'maximumPixel']
        for key in measurementInput:
            if key == 'getFeretDiameter':
                itkFilter.ComputeFeretDiameterOn()

            if key in ['getPerimeter', 'getPerimeterOnBorder', 'getPerimeterOnBorderRatio',
                       'getEquivalentSphericalPerimeter']:
                itkFilter.ComputePerimeterOn()

            if key in singleChannelFunctionDict.keys() and not singleChMeasurementList:
                singleChMeasurementList.append(key)

            if key in dualChannelFunctionDict.keys() and not dualChMeasurementList:
                dualChMeasurementList.append(key)
                # main_list = [item for item in list_2 if item not in list_1]

        # dataBase dictionary to store results
        dataBase = {'tag': [], }

        for key in singleChMeasurementList:
            dataBase[key] = []

        #Cycle through images to measure parameters
        if imageList == []:
            imageList.append(taggedImage)
            dictionaryList.append(dictionary)

        for i in range(len(imageList)):


            itkRaw = sitk.GetImageFromArray(imageList[0])

            for key in dualChMeasurementList:
                dataBase[key+' in '+dictionaryList[i]['name']] = []

            #Execute Filter and get database
            itkFilter.Execute(itkImage, itkRaw)
            data=itkFilter.GetLabels()

            for label in data:
                if i==0:
                    dataBase['tag'].append(label)
                    for key in singleChMeasurementList:
                        dataBase[key].append(singleChannelFunctionDict[key](label))

                for key in dualChMeasurementList:
                    dataBase[key+' in '+dictionaryList[i]['name']].append(dualChannelFunctionDict[key](label))

        if 'surface' in measurementInput:
            #Create and measure Surface Image
            surface=Segmentation.create_surfaceImage(taggedImage)
            surfaceItkImage=sitk.GetImageFromArray(surface)

            itkFilter=sitk.LabelShapeStatisticsImageFilter()
            itkFilter.Execute(surfaceItkImage)
            surfaceData=itkFilter.GetLabels()

            dataBase['surface']=[]
            for label in surfaceData:
                dataBase['surface'].append(singleChannelFunctionDict['voxelCount'](label))

        dictionary['dataBase']=dataBase

        return dictionary


    @staticmethod
    def filter_dataBase(dictionary, filterDict, removeFiltered=False, overWrite=True):

        dataFrame=pd.DataFrame(dictionary['dataBase'])

        if 'filter' in dictionary['dataBase'].keys():
            originalFilter = dictionary['dataBase']['filter']

        #Only run filter if key has numerical value
        typeList=['int', 'float', 'bool', 'complex',  'int_','intc', 'intp', 'int8' ,'int16' ,'int32' ,'int64'
                        ,'uint8' ,'uint16' ,'uint32' ,'uint64' ,'float_' ,'float16' ,'float32' ,'float64','loat64' ,'complex_' ,'complex64' ,'complex128' ]
        for key in filterDict:

            if dataFrame[key].dtype in typeList:

                currentFilter=(dataFrame[key]>=filterDict[key]['min'])&(dataFrame[key]<=filterDict[key]['max'])
                if ('filter' in dictionary['dataBase'].keys()) & (overWrite == False):
                    dataFrame['filter']=np.multiply(originalFilter,currentFilter)
                else:
                    dataFrame['filter']=currentFilter

        if removeFiltered == True:
            dataFrame=dataFrame[(dataFrame['filter']==True)]

        dictionary['dataBase'] = dataFrame.to_dict(orient='list')

        return dictionary

    @staticmethod
    def saveData(dictionaryList, path, fileName='output', toText=False):

        dataFrameList=[]
        keyOrderList=[]
        columnWidthsList=[]
        nameList = [x['name'] for x in dictionaryList]

        for dict in  dictionaryList:

            if 'dataBase' in dict.keys():

                # Convert to Pandas dataframe
                dataFrame=pd.DataFrame(dict['dataBase'])

                dataFrameList.append(dataFrame)

                # Sort dictionary with numerical types first (tag, volume, voxelCount,  first) and all others after (centroid, center of mass, bounding box first)
                numericalKeys = []
                otherKeys = []

                for key in dict['dataBase'].keys():

                    if str(dataFrame[key].dtype) in ['int', 'float', 'bool', 'complex', 'Bool_', 'int_','intc', 'intp', 'int8' ,'int16' ,'int32' ,'int64'
                        ,'uint8' ,'uint16' ,'uint32' ,'uint64' ,'float_' ,'float16' ,'float32' ,'float64','loat64' ,'complex_' ,'complex64' ,'complex128' ]:
                        numericalKeys.append(key)

                    else:
                        otherKeys.append(key)

                #Rearange keylist
                presetOrder=['tag', 'volume', 'voxelCount', 'filter']
                numericalKeys=Measurement.reorderList(numericalKeys,presetOrder)
                presetOrder = ['centroid']
                otherKeys=Measurement.reorderList(otherKeys,presetOrder)
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

                # Convert the dataframe to an XlsxWriter Excel object. Crop worksheet name if too long
                name =str(nameList[i])
                if len(name) > 30:
                    name=(str(nameList[i])[:30] + '_')
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
                    if dataFrameList[i] != None:

                        outputFile.write('name= '+nameList[i]+'\n')
                        outputFile.write(dataFrameList[i].to_csv(sep='\t', columns=keyOrderList[i], index=False, header=True))

        return 0

    @staticmethod
    def saveImage(imageList, path, fileName='output.tif', append=False, bigtiff=False, byteorder=None, software='A3DC', imagej=False, photometric=None, planarconfig=None, tile=None, contiguous=True, compress=0, colormap=None, description=None, datetime=None, resolution=None, metadata={}, extratags=()):

        if byteorder is None:
            byteorder = '<' if sys.byteorder == 'little' else '>'
        print(byteorder)

        with TiffWriter(os.path.join(path, fileName), append, bigtiff, byteorder, software, imagej) as tif:
            for i in range(len(imageList[0])):
                for image in imageList:
                    print(type(image))
            tif.save( np.array(imageList),  photometric, planarconfig, tile, contiguous, compress, colormap, description, datetime, resolution, metadata, extratags)

    @staticmethod
    def reorderList(list, valueList):

        for element in reversed(valueList):
            if element in list:
                list.remove(element)
                list.insert(0, element)

        return list


    #######################################################################################################################################
    ####################################################################Helper Functions###################################################
    @staticmethod
    def measure_volume(objectList):

        volumes = np.zeros(len(objectList)) #dtype='uint32')

        for i in range(0, len(objectList)):
            volumes[i]=len(objectList[i])

        return volumes

    @staticmethod
    def isOnEdge(itkFilter, label):
        value = True if itkFilter.GetNumberOfPixelsOnBorder(label) > 0  else False
        return value




#############################################Class that contain main functions for A3DC####################################################
class Segmentation(object):

    @staticmethod
    def threshold_auto(image, method="Otsu", mode="2D"):

        '''
        Apply autothreshold slice by slice
        :param img: nd array
        :param thresholdMethod: threshold method name as string
                * 'Otsu'
                * 'Huang'
                * 'IsoData'
                * 'Li'
                * 'MaxEntropy'
                * 'KittlerIllingworth'
                * 'Moments'
                * 'Yen'
                * 'RenyiEntropy'
                * 'Shanbhag'
        :return: nd array
        '''

        if mode=="3D":
            outputImage, _=Segmentation.itkThresholder(image, method)

        elif mode=="2D":

            outputImage = []
            for i in range(len(image)):
                segmentedImage, _ = Segmentation.itkThresholder(image[i], method)
                outputImage.append(segmentedImage)

        return outputImage

    @staticmethod
    def itkThresholder(image, method):

        itkThresholdDict = {'IsoData': sitk.IsoDataThresholdImageFilter(), 'Otsu': sitk.OtsuThresholdImageFilter(),
                                'Huang': sitk.HuangThresholdImageFilter(),
                                'MaxEntropy': sitk.MaximumEntropyThresholdImageFilter(),
                                'Li': sitk.LiThresholdImageFilter(),
                                'RenyiEntropy': sitk.RenyiEntropyThresholdImageFilter(),
                                'KittlerIllingworth': sitk.KittlerIllingworthThresholdImageFilter(),
                                'Moments': sitk.MomentsThresholdImageFilter(), 'Yen': sitk.YenThresholdImageFilter(),
                                'Shanbhag': sitk.ShanbhagThresholdImageFilter(),
                            'Triangle': sitk.TriangleThresholdImageFilter()}
        #Create ITK image
        itkImage = sitk.GetImageFromArray(image)
        #Create ITK FIlter object
        threshold=itkThresholdDict[method]
        # Get and apply threshold and invert
        segmentedImage = sitk.InvertIntensity(threshold.Execute(itkImage), 1)
        threshold = threshold.GetThreshold()

        return sitk.GetArrayFromImage(segmentedImage), threshold

    @staticmethod
    def create_surfaceImage(taggedImage):

        # Convert nd array to itk image
        itkImage = sitk.GetImageFromArray(taggedImage)
        pixelType = itkImage.GetPixelID()
        # Run binary threshold
        thresholdedItkImage = sitk.BinaryThreshold(itkImage, 0, 0, 0, 1)



        # Create an parametrize instance ofBinaryContourImageFilter()
        itkFilter = sitk.BinaryContourImageFilter()
        itkFilter.SetFullyConnected(False)
        #Execute to get a surface mask
        output = itkFilter.Execute(thresholdedItkImage)

        # Change pixeltype of the object map to be the same as the input image
        caster = sitk.CastImageFilter()
        caster.SetOutputPixelType(pixelType)
        output=caster.Execute(output)*itkImage

        return sitk.GetArrayFromImage(output)

    @staticmethod
    def create_overlappingImage(taggedImageList):
        # Create an overlapping image by pixelwise multiplication
        img=taggedImageList[0]

        for i in range(1,len(taggedImageList)):
            img=np.multiply(img, taggedImageList[i])

        return img


    @staticmethod
    def tag_image(image):

        # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
        itk_image = sitk.GetImageFromArray(image)
        tagged_image = sitk.ConnectedComponent(itk_image, fullyConnected=True)

        return sitk.GetArrayFromImage(tagged_image)

    @staticmethod
    def threshold_manual(img, lowerThreshold, upperThreshold):

        # Convert nd Image to ITK image
        itkImage = sitk.GetImageFromArray(img)

        #threshold=sitk.ThresholdImageFilter()
        threshold=sitk.BinaryThresholdImageFilter()
        threshold.SetUpperThreshold(float(upperThreshold))
        threshold.SetLowerThreshold(float(lowerThreshold))
        segmentedImage=threshold.Execute(itkImage)

        # Threshold and Invert
        return sitk.GetArrayFromImage(sitk.InvertIntensity(segmentedImage, 1))


    @staticmethod
    def threshold_adaptive(image, method, blockSize=5, offSet=0):

        #Cast to 8-bit
        convertedImage = img_as_ubyte(image)

        #Cycle through image
        outputImage = []
        for i in range(len(image)):
            if method == 'Adaptive Mean':
                outputImage.append(threshold_local(convertedImage, blockSize, offSet))

            elif method == 'Adaptive Gaussian':
                outputImage.append(cv2.adaptiveThreshold(convertedImage, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                                    cv2.THRESH_BINARY, blockSize, offSet))
            else:
                raise LookupError('Not a valid method!')
                break


        return outputImage






###############################################Class that contain main functions for A3DC####################################################
    class ImageProcessing(object):

        @staticmethod
        def smoothingGaussianFilter(image, sigma):

            itkImage = sitk.GetImageFromArray(image)
            pixelType = itkImage.GetPixelID()

            sGaussian = sitk.SmoothingRecursiveGaussianImageFilter()
            sGaussian.SetSigma(float(sigma))
            itkImage = sGaussian.Execute(itkImage)

            caster = sitk.CastImageFilter()
            caster.SetOutputPixelType(pixelType)

            return sitk.GetArrayFromImage(caster.Execute(itkImage))

        @staticmethod
        def discreteGaussianFilter(image, sigma):
            itkImage = sitk.GetImageFromArray(image)
            pixelType = itkImage.GetPixelID()

            dGaussian = sitk.DiscreteGaussianImageFilter ()
            dGaussian.SetVariance(int(sigma))
            dGaussian.SetUseImageSpacing(False)
            itkImage = dGaussian.Execute(itkImage)

            caster = sitk.CastImageFilter(itkImage)
            caster.SetOutputPixelType(pixelType)

            return sitk.GetArrayFromImage(caster.Execute(itkImage))

        @staticmethod
        def medianFilter(image, radius):
            itkImage = sitk.GetImageFromArray(image)
            pixelType = itkImage.GetPixelID()

            median = sitk.MedianImageFilter()
            median.SetRadius(int(radius))
            itkImage = median.Execute(itkImage)

            caster = sitk.CastImageFilter(itkImage)
            caster.SetOutputPixelType(pixelType)

            return sitk.GetArrayFromImage(caster.Execute(itkImage))

        @staticmethod
        def regionGrowingSegmentation(image, seedList, initialNeighborhoodRadius=2, multiplier=2.5, NbrOfIterations=5,
                                      replaceValue=255):
            itkImage = sitk.GetImageFromArray(image)


            filter = sitk.ConfidenceConnectedImageFilter()
            filter.SetSeedList(seedList)
            filter.SetMultiplier(multiplier)
            filter.SetNumberOfIterations(NbrOfIterations)
            filter.SetReplaceValue(replaceValue)
            filter.SetInitialNeighborhoodRadius(initialNeighborhoodRadius)
            itkImage = filter.Execute(itkImage)


            return sitk.GetArrayFromImage(itkImage)

        #Ideas:
           # Histogram equilization, depth correction: itk.AdaptiveHistogramEqualizationImageFilter, equalize_adapthist, equalize_hist¶
            #Seed based segmentation: skimage.segmentation.random_walker, watershed voronoi
            #Geodescic active contour itk, skimage.segmentation.morphological_geodesic_active_contour
            #Add, subtract ,multiply
            #close, open, dilate,erode, local maxima/minima, thinning?
            #Denoising:denoise_wavelet¶

############################################Helper Functions for testing in python###################################
class Processor(object):

    #This class is a collection of methods most of which are static methods used to process images and image arrays.

    @staticmethod
    def load_image(filePath):
        #Load image using scikit/image im read
        return imread(str(filePath))

    @staticmethod
    def save_image(nparray, path):
        #Save image using tifffile save
        imsave(path, nparray)

    @staticmethod
    def threshold_manual(image,  lower, upper=pow(2, 32)):
        # Threshold image using lower and upper as range
        return (lower < img) & (img < upper)

    @staticmethod
    def colocalization_overlap(taggedImgList, sourceImageList=[], overlappingAnalysisInput={}):

        overlappingImage = Segmentation.tag_image(Segmentation.create_overlappingImage(taggedImgList))
        overlappingDataBase = Measurement.analyze(overlappingImage, 'Overlapping', sourceImageList,
                                                  **overlappingAnalysisInput)

        return overlappingImage, overlappingDataBase

    @staticmethod
    def colocalization_connectivity(taggedImgList, dataBaseList, overlappingDataBase):

        # Generate array lists and name lists
        taggedArrayList = [x.flatten() for x in taggedImgList]
        nameList = [x['Name'] for x in dataBaseList]

        for i in range(len(taggedArrayList)):

            objectList = [None for i in range(len(overlappingDataBase['Index']))]
            ovlRatioList = [None for i in range(len(overlappingDataBase['Index']))]

            for j in range(len(overlappingDataBase['Index'])):
                ovlPosition = overlappingDataBase['Index'][j][0]
                objectList[j] = taggedArrayList[i][ovlPosition]
                ovlRatioList[j] = overlappingDataBase['Volume'][j] / dataBaseList[i]['Volume'][objectList[j] - 1]

            overlappingDataBase['Object in ' + nameList[i]] = objectList
            overlappingDataBase['OverlappingRatio in ' + nameList[i]] = ovlRatioList

        return overlappingDataBase

    @staticmethod
    def colocalizaion_analysis(taggedImgList, dataBaseList, overlappingDataBase):

        # Generate array lists and name lists

        nameList = [x['Name'] for x in dataBaseList]

        # Update dataBase for segmented images
        for i in range(len(taggedImgList)):

            positionList = [x for x in range(len(taggedImgList)) if x != i]

            buffer = []
            ovlRatioBuffer = []
            for m in range(len(positionList)):
                buffer.append([[] for n in range(dataBaseList[positionList[m]]['NbOfObjects'])])
                ovlRatioBuffer.append([0 for n in range(dataBaseList[positionList[m]]['NbOfObjects'])])

            for j in range(overlappingDataBase['NbOfObjects']):

                tag = overlappingDataBase['Object in ' + nameList[i]][j]

                for k in range(len(positionList)):
                    currentTag = overlappingDataBase['Object in ' + nameList[positionList[k]]][j]
                    buffer[k][currentTag - 1].append(tag)
                    ovlRatioBuffer[k][currentTag - 1] += overlappingDataBase['Volume'][
                        j]  # /dataBaseList[k]['Volume'][tag-1]

            for q in range(len(positionList)):
                dataBaseList[positionList[q]]['Objects in ' + nameList[i]] = buffer[q]
                dataBaseList[positionList[q]]['TotalOverlappingRatios in ' + nameList[i]] = np.divide(ovlRatioBuffer[q],
                                                                                                      dataBaseList[
                                                                                                          positionList[
                                                                                                              q]][
                                                                                                          'Volume'])
                dataBaseList[positionList[q]]['Colocalization Count'] = Measurement.measure_volume(buffer[q])


        return dataBaseList, overlappingDataBase

    @staticmethod
    def generate_surface(tinyImg_tagged):

        img = copy.copy(tinyImg_tagged)

        kernel = np.array([[[1, 1, 1],
                            [1, 1, 1],
                            [1, 1, 1]],
                           [[1, 1, 1],
                            [1, 0, 1],
                            [1, 1, 1]],
                           [[1, 1, 1],
                            [1, 1, 1],
                            [1, 1, 1]]])

        # Normalize non zero pixels to 1
        img[img > 0] = 1
        # print(tinyImg)
        # Generate a map showing the number of non zero pixels surrounding each pixel
        convolved = convolve(img, kernel, mode='constant', cval=1.0)
        # Remove elements that have 26 neighbours (non surface pixels)
        # convolved=np.multiply(tinyImg, convolved)
        convolved[convolved == 26] = 0
        # Normalize surface pixels
        convolved[convolved > 0] = 1

        # Generate surface map
        convolved = np.multiply(img, convolved)

        return convolved

    @staticmethod
    def analyze(taggedImg, name, sourceImageList, chanelList=[], isOnEdge=True, boundingBox=True, surface=True,
                centroid=True):

        # Output Dictionaries
        dataBase = {}  # Dictionary for measured parameters

        dataBase['Name'] = name
        dataBase['NbOfObjects'] = np.amax(taggedImg)

        imgshape = taggedImg.shape
        dataBase['Depth'] = imgshape[0]
        dataBase['Height'] = imgshape[1]
        dataBase['Width'] = imgshape[2]

        # Always Measured
        pixelCoordinateList = Measurement.create_pixelCoordinateList(taggedImg)
        dataBase['Index'] = pixelCoordinateList
        dataBase['Volume'] = Measurement.measure_volume(pixelCoordinateList)

        # Measured conditionally
        if isOnEdge == True:
            dataBase['isOnEdge'] = Measurement.isOnEdge(taggedImg)

        if surface == True:
            surfacePixelList = Measurement.create_surfacePixelList(taggedImg, pixelCoordinateList)
            dataBase['Surface'] = surfacePixelList
            dataBase['Surface Area'] = Measurement.measure_volume(surfacePixelList)

        if (centroid or boundingBox) == True:
            cartesianCoordinateList = Measurement.create_cartesianList(taggedImg, pixelCoordinateList)
            dataBase['Cartesian X'] = cartesianCoordinateList[0];
            dataBase['Cartesian Y'] = cartesianCoordinateList[1];
            dataBase['Cartesian Z'] = cartesianCoordinateList[2]

            if centroid == True:
                dataBase['Centroid X'], dataBase['Centroid Y'], dataBase[
                    'Centroid Z'] = Measurement.create_centroidList(cartesianCoordinateList[0],
                                                                    cartesianCoordinateList[1],
                                                                    cartesianCoordinateList[2])
            if boundingBox == True:
                dataBase['Bounding box X1'], dataBase['Bounding box X2'], dataBase['Bounding box Y1'], dataBase[
                    'Bounding box Y2'], dataBase['Bounding box Z1'], dataBase[
                    'Bounding box Z2'] = Measurement.measure_boundingBox(cartesianCoordinateList[0],
                                                                         cartesianCoordinateList[1],
                                                                         cartesianCoordinateList[2])

        if len(chanelList) != []:
            for i in chanelList:
                intensityList = Measurement.create_intensityList(sourceImageList[i], pixelCoordinateList)
                meanIntensityList = Measurement.measure_mean(intensityList)
                dataBase['Intensity in Ch' + str(i)] = intensityList;
                dataBase['Mean intensity in Ch' + str(i)] = meanIntensityList

        return dataBase

    @staticmethod
    def create_connectivityList(pixelList, taggedImage):

        connectivityList = [[] for i in range(pixelList)]

        # Flatten images
        array = taggedImage.flatten

        for i in range(len(pixelList)):
            if array(pixelList[i][0]) > 0:
                connectivityList.append(array(pixelList[i][0] - 1))

        return connectivityList

    @staticmethod
    def tag_image(image):
        # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
        itk_image = sitk.GetImageFromArray(image)
        tagged_image = sitk.ConnectedComponent(itk_image, fullyConnected=True)

        return sitk.GetArrayFromImage(tagged_image)

    @staticmethod
    def create_pixelCoordinateList_core(input):

        imgArray = input[0]
        length = input[1]
        arrayOffset = input[2]
        coordinates = [[] for i in range(length)]

        i = 0
        for k in range(imgArray.size):
            i += 1
            if imgArray[k] != 0:
                coordinates[imgArray[k] - 1].append(k + arrayOffset)

        return coordinates

    @staticmethod
    def create_pixelCoordinateList(image_tagged):

        NUM_WORKERS = mp.cpu_count() + 2
        imgArray = image_tagged.flatten()

        # Split array to feed to processes
        split = np.array_split(imgArray, NUM_WORKERS)
        length = np.amax(imgArray)

        # Input Parameters for the separate processes
        objectListLength = [length for i in range(0, NUM_WORKERS)]
        arrayOffsetList = [0 for i in range(0, NUM_WORKERS)]
        # Offset of pixels
        for i in range(1, NUM_WORKERS):
            arrayOffsetList[i] = arrayOffsetList[i - 1] + len(split[i - 1])
        input = zip(split, objectListLength, arrayOffsetList)

        # Paralel processesing of split lists
        with mp.Pool(processes=NUM_WORKERS) as pool:
            multiple_results = pool.map(Measurement.create_pixelCoordinateList_core, input)

        # Combine Results from the different processes
        objectCoordinateList = [[] for i in range(0, len(multiple_results[0]))]

        arrayOffset = 0
        for j in range(length):
            for i in range(len(multiple_results)):
                objectCoordinateList[j] = objectCoordinateList[j] + multiple_results[i][j]
            arrayOffset += len(split[i])

        return objectCoordinateList

    @staticmethod
    def isOnEdge(tinyImg_tagged):

        shape = tinyImg_tagged.shape
        NbOverlappingObj = np.amax(tinyImg_tagged)
        matrixShape = shape
        sidePixelList = [tinyImg_tagged[0, :, :], tinyImg_tagged[-1, :, :], tinyImg_tagged[1:matrixShape[0] - 1, 0, :],
                         tinyImg_tagged[1:matrixShape[0] - 1, matrixShape[1] - 1, :],
                         tinyImg_tagged[1:matrixShape[0] - 1, 1:matrixShape[0] - 2, 0],
                         tinyImg_tagged[1:matrixShape[0] - 1, 1:matrixShape[0] - 2, matrixShape[2] - 1]]

        isOnEdgeList = [False for i in range(0, NbOverlappingObj)]
        for i in range(0, len(sidePixelList)):
            for pix in sidePixelList[i].flatten():
                if pix > 0:
                    isOnEdgeList[pix - 1] = True

        return isOnEdgeList

    @staticmethod
    def measure_volume(objectList):

        volumes = np.zeros(len(objectList), dtype='uint32')

        for i in range(0, len(objectList)):
            volumes[i]=len(objectList[i])

        return volumes

    @staticmethod
    def measure_volume(objectList):

        volumes = np.zeros(len(objectList), dtype='uint32')

        for i in range(0, len(objectList)):
            volumes[i] = len(objectList[i])

        return volumes

    @staticmethod
    def create_surfacePixelList(image, objectCoordinateList):

        image_copy = copy.copy(image)

        # Measure the number of neighbors using convolution, objects that have one pixel are deleted (see later)
        kernel = np.array([[[1, 1, 1],
                            [1, 1, 1],
                            [1, 1, 1]],
                           [[1, 1, 1],
                            [1, 0, 1],
                            [1, 1, 1]],
                           [[1, 1, 1],
                            [1, 1, 1],
                            [1, 1, 1]]])

        # Normalize non zero pixels to 1
        image_copy[image_copy > 0] = 1
        # Generate a map showing the number of non zero pixels surrounding each pixel
        convolved = convolve(image_copy, kernel, mode='constant', cval=1.0)
        # Remove elements that have 26 neighbours (non surface pixels)
        # convolved=np.multiply(tinyImg, convolved)
        convolved[convolved == 26] = 0
        # Normalize surface pixels
        convolved[convolved > 0] = 1

        # Generate surface map
        convolved = np.multiply(image_copy, convolved)

        # Create surfaceList from surface image. Objects with a single pixel are added to the list here.
        length = len(objectCoordinateList)
        surfaceArray = convolved.flatten()
        surfaceCoordinates = [[] for i in range(0, length)]
        for i in range(length):
            for j in range(len(objectCoordinateList[i])):
                buffer = surfaceArray[objectCoordinateList[i][j]]
                if len(objectCoordinateList[i]) == 1 or buffer > 0:
                    surfaceCoordinates[i].append(objectCoordinateList[i][j])

        return surfaceCoordinates

    @staticmethod
    def create_cartesianList(tinyImg_tagged, objectList):

        imageShape = tinyImg_tagged.shape
        length = len(objectList)

        slicePixelnumber = int(imageShape[1] * imageShape[2])

        x = [[] for i in range(length)]
        y = [[] for i in range(length)]
        z = [[] for i in range(length)]

        for i in range(len(objectList)):
            for j in range(len(objectList[i])):
                index = objectList[i][j]

                zcoord = int(index / slicePixelnumber)
                ycoord = int((index - zcoord * slicePixelnumber) / imageShape[2])
                xcoord = (index - zcoord * slicePixelnumber - ycoord * imageShape[2])

                z[i].append(zcoord)
                y[i].append(ycoord)
                x[i].append(xcoord)

        return x, y, z

    @staticmethod
    def create_centroidList(cartesianListX, cartesianListY, cartesianListZ):

        length = len(cartesianListX)

        centroids_x = np.zeros(length)
        centroids_y = np.zeros(length)
        centroids_z = np.zeros(length)
        for i in range(length):
            centroids_x[i] = np.mean(cartesianListX[i])
            centroids_y[i] = np.mean(cartesianListY[i])
            centroids_z[i] = np.mean(cartesianListZ[i])

        return centroids_x, centroids_y, centroids_z

    @staticmethod
    def measure_boundingBox(cartesianListX, cartesianListY, cartesianListZ):

        length = len(cartesianListX)

        boundingX1 = [[] for i in range(0, length)]
        boundingX2 = [[] for i in range(0, length)]
        boundingY1 = [[] for i in range(0, length)]
        boundingY2 = [[] for i in range(0, length)]
        boundingZ1 = [[] for i in range(0, length)]
        boundingZ2 = [[] for i in range(0, length)]

        for i in range(length):
            boundingX1[i] = np.amin(cartesianListX[i])
            boundingX2[i] = np.amax(cartesianListX[i])
            boundingY1[i] = np.amin(cartesianListY[i])
            boundingY2[i] = np.amax(cartesianListY[i])
            boundingZ1[i] = np.amin(cartesianListZ[i])
            boundingZ2[i] = np.amax(cartesianListZ[i])

        return boundingX1, boundingX2, boundingY1, boundingY2, boundingZ1, boundingZ2

    @staticmethod
    def measure_mean(objectList):

        length = len(objectList)
        meanList = np.zeros(length)

        for i in range(length):
            meanList[i] = np.mean(objectList[i])

        return meanList

    @staticmethod
    def create_intensityList(image, objectList):

        imgArray = image.flatten()
        intensityList = [[] for i in range(0, len(objectList))]

        for j in range(len(objectList)):
            for i in range(len(objectList[j])):
                intensityList[j].append(imgArray[objectList[j][i]])

        return intensityList


if __name__ == '__main__':

    a = Main()

