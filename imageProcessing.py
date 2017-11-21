import SimpleITK as sitk
import cv2
import numpy as np
from math import pow
from skimage.external.tifffile import imread, imsave
from skimage.filters import threshold_isodata, threshold_li, threshold_local, threshold_mean, threshold_minimum, threshold_niblack, threshold_otsu, threshold_sauvola, threshold_triangle, threshold_yen
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

        #############################################Load Images####################################################
        sourceImageList=[]
        sourceDictList=[]
        # Channel 1
        ch1Path = ('F:/Workspace/TestImages/test_1.tif')#("D:/OneDrive - MTA KOKI/Workspace/Playground/test7_1.tif")

        ch1Img=Processor.load_image(ch1Path)
        ch1Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch1Img)
        sourceDictList.append(ch1Dict)


        # Channel 2
        ch2Path = ('F:/Workspace/TestImages/test_2.tif')#("D:/OneDrive - MTA KOKI/Workspace/Playground/test7_2.tif")

        ch2Img = Processor.load_image(ch2Path)

        ch2Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch2Img)
        sourceDictList.append(ch2Dict)

        # Channel 3
        ch3Path = ('F:/Workspace/TestImages/test_3.tif')#("D:/OneDrive - MTA KOKI/Workspace/Playground/test7_3.tif")
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
        thresholdedImage1=Segmentation.threshold2D_auto(sourceImageList[0], "Otsu")
        taggedImage1=Segmentation.tag_image(thresholdedImage1)

        taggedDict1=sourceDictList[0]
        taggedDict1=Measurement.analyze(taggedImage1, taggedDict1, imageList=sourceImageList, dictionaryList=sourceDictList)
        taggedDict1['name']='Channel1'

        taggedImageList.append(taggedImage1)
        taggedDictList.append(taggedDict1)


        # Channel 2
        thresholdedImage2 = Segmentation.threshold2D_auto(sourceImageList[1], "MaxEntropy")
        taggedImage2 = Segmentation.tag_image(thresholdedImage2)



        taggedDict2=sourceDictList[1]
        taggedDict2 = Measurement.analyze(taggedImage2, taggedDict2, imageList=[sourceImageList[1]], dictionaryList=sourceDictList)
        taggedDict2['name']='Channel2'


        taggedImageList.append(taggedImage2)
        taggedDictList.append(taggedDict2)

        # Channel 3
        thresholdedImage3 = Segmentation.threshold2D_auto(sourceImageList[2], "MaxEntropy")
        taggedImage3 = Segmentation.tag_image(thresholdedImage3)

        taggedDict3 =sourceDictList[2]
        taggedDict3 = Measurement.analyze(taggedImage3, taggedDict3, imageList=sourceImageList, dictionaryList=sourceDictList)
        taggedDict3['name'] = 'Channel3'

        taggedImageList.append(taggedImage3)
        taggedDictList.append(taggedDict3)
        #############################################################################################################

        #############################################Analysis Images####################################################
        #print(taggedDictList[0])
        #print(sourceDictList[0])
        Measurement.analyze(taggedImageList[0], taggedDictList[0])


        dictFilter={'volume':{'min':2, 'max':11}}#, 'mean in '+taggedDictList[0]['name']: {'min':2, 'max':3}}
        #print(taggedDictList[2])
        overlappingImage, overlappingDataBase=Measurement.colocalization_overlap(taggedImageList, taggedDictList, sourceImageList=sourceImageList, sourceDictionayList=sourceDictList)
        overlappingDataBase=Measurement.colocalization_connectivity(taggedImageList, taggedDictList, overlappingDataBase)
        print('###############################')
        print(taggedDict2)
        filteredDict2=Measurement.filter_dataBase(taggedDict2, {'mean in Channel1':{'min':2,'max':3}, 'ellipsoidDiameter':{'min':2,'max':3}})
        print('###############################')
        print(filteredDict2)

        #filteredDict2 = Measurement.filter_dataBase(taggedDict2, {'mean in Channel1': {'min': 3, 'max': 4}})
        #print('###############################')
        #print(filteredDict2)
        #filtIm=Measurement.filter_image(overlappingImage, overlappingDataBase)
        #print(overlappingImage)
        #print(filtIm)
        #print(overlappingImage)
        #print(overlappingDataBase['dataBase'])
        #print(a)
        #print(taggedImage)
        tstop = time.clock()
        print('ITK STATS: ' + str(tstop - tstart))

        print(False*True)





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
        # Create Overlapping Dictionary
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
        taggedArrayList = [x.flatten() for x in taggedImgList]
        nameList = [x['name'] for x in dataBaseList]

        for i in range(len(taggedImgList)):
            itk_image = sitk.GetImageFromArray(taggedImgList[i])

            overlappingPixels=overlappingDataBase['dataBase']['pixel in '+overlappingDataBase['name']]

            objectList = [None for i in range(len(overlappingPixels))]
            ovlRatioList = [None for i in range(len(overlappingPixels))]

            for j in range(len(overlappingPixels)):
                ovlPosition = overlappingPixels[j]
                objectList[j] = itk_image.GetPixel(ovlPosition)
                ovlRatioList[j] = overlappingDataBase['dataBase']['volume'][j] / dataBaseList[i]['dataBase']['volume'][objectList[j] - 1]

            overlappingDataBase['dataBase']['object in ' + nameList[i]] = objectList
            overlappingDataBase['dataBase']['overlappingRatio in ' + nameList[i]] = ovlRatioList

        return overlappingDataBase

    @staticmethod
    def colocalizaion_analysis( taggedImgList, dataBaseList, overlappingImage, overlappingDataBase):

        # Generate array lists and name lists

        nameList = [x['name'] for x in dataBaseList]

        # Update dataBase for segmented images
        for i in range(len(dataBaseList)):

            positionList = [x for x in range(len(dataBaseList)) if x != i]
            buffer = []
            ovlRatioBuffer = []

            for m in range(len(positionList)):
                NbOfObjects=np.amax(taggedImgList[positionList[m]])
                buffer.append([[] for n in range(NbOfObjects)])
                ovlRatioBuffer.append([0 for n in range(NbOfObjects)])

            for j in range(np.amax(overlappingImage)):

                tag = overlappingDataBase['dataBase']['object in ' + nameList[i]][j]

                for k in range(len(positionList)):
                        currentTag = overlappingDataBase['dataBase']['object in ' + nameList[positionList[k]]][j]

                        condition=((('filter' not in dataBaseList[i]['dataBase'].keys())&\
                            ('filter' not in dataBaseList[positionList[k]]['dataBase'].keys())&\
                            ('filter' not in overlappingDataBase['dataBase'])) or\
                            ((dataBaseList[i]['dataBase']['filter'][tag] == True)&\
                            (dataBaseList[positionList[k]]['dataBase']['filter'][currentTag] == True)&\
                            (overlappingDataBase['dataBase']['filter'][j]==True)))

                        if condition==True:

                            buffer[k][currentTag - 1].append(tag)
                            ovlRatioBuffer[k][currentTag - 1] += overlappingDataBase['dataBase']['volume'][j]

            for q in range(len(positionList)):
                dataBaseList[positionList[q]]['dataBase']['objects in ' + nameList[i]] = buffer[q]
                dataBaseList[positionList[q]]['dataBase']['totalOverlappingRatios in ' + nameList[i]] = np.divide(ovlRatioBuffer[q],dataBaseList[positionList[q]]['dataBase']['volume'])
                dataBaseList[positionList[q]]['dataBase']['colocalization count'] = Measurement.measure_volume(buffer[q])

        return dataBaseList, overlappingDataBase

    @staticmethod
    def analyze(taggedImage, dictionary, imageList=[], dictionaryList=[]):

        if imageList==[]:
            imageList.append(taggedImage)
            dictionaryList.append(dictionary)

        dataBase = {'tag': [], 'volume': [], 'voxelCount': [], 'centroid': [], 'ellipsoidDiameter': [],
                    'boundingBox': []}

        for i in range(len(imageList)):

            itkImage = sitk.GetImageFromArray(taggedImage)
            itkRaw = sitk.GetImageFromArray(imageList[0])
            # Measurements for parameters that do not need two images (data like volume, centroid etc.)  in ITK can
            # can be done with itkFilter = sitk.LabelShapeStatisticsImageFilter()!!!!

            itkFilter = sitk.LabelIntensityStatisticsImageFilter()
            itkFilter.Execute(itkImage, itkRaw)
            data = itkFilter.GetLabels()

            dataBase['pixel in '+dictionaryList[i]['name']]= []
            dataBase['mean in ' + dictionaryList[i]['name']]=[]
            dataBase['centerOfMass in ' + dictionaryList[i]['name']]=[]

            for label in data:
                if i==0:
                    dataBase['tag'].append(label)
                    dataBase['volume'].append(itkFilter.GetPhysicalSize(label))
                    dataBase['voxelCount'].append(itkFilter.GetNumberOfPixels(label))
                    dataBase['centroid'].append(itkFilter.GetCentroid(label))
                    dataBase['ellipsoidDiameter'].append(itkFilter.GetEquivalentEllipsoidDiameter(label))
                    dataBase['boundingBox'].append(itkFilter.GetBoundingBox(label))

                dataBase['mean in '+dictionaryList[i]['name']].append(itkFilter.GetMean(label))
                dataBase['pixel in '+dictionaryList[i]['name']].append(itkFilter.GetMaximumIndex(label))
                dataBase['centerOfMass in '+dictionaryList[i]['name']].append(itkFilter.GetCenterOfGravity(label))


        dictionary['dataBase']=dataBase

        return dictionary


    @staticmethod
    def filter_dataBase(dictionary, filterDict, removeFiltered=False, overWrite=True):

        dataFrame=pd.DataFrame(dictionary['dataBase'])

        if 'filter' in dictionary['dataBase'].keys():
            originalFilter = dictionary['dataBase']['filter']

        #print(dataFrame)
        for key in filterDict:

            if dataFrame[key].dtype in [int, float, bool, complex]:
                print(key)


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
    def save(dictionaryList, path, fileName='output', toText=True):

        if toText==False:

            # Create a Pandas Excel writer using XlsxWriter as the engine.
            writer = pd.ExcelWriter(os.path.join(path, fileName+'.xls'), engine='xlsxwriter')

            for dict in dictionaryList:
                if 'dataBase' in dict:
                    dataFrame = pd.DataFrame(dict['dataBase'])

                    # Convert the dataframe to an XlsxWriter Excel object.
                    dataFrame.to_excel(writer, sheet_name=dict['name'])

            # Close the Pandas Excel writer and output the Excel file.
            writer.save()

        elif toText==True:

            with open(os.path.join(path, fileName + '.txt'), 'w') as outputFile:

                for dict in dictionaryList:
                    if 'dataBase' in dict:
                        dataFrame = pd.DataFrame(dict['dataBase'])

                        outputFile.write('name= '+dict['name']+'\n')
                        outputFile.write(dataFrame.to_csv(sep='\t', index=False, header=True))

        return 0








#############################################Class that contain main functions for A3DC####################################################
class Segmentation(object):


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
    def threshold_manual(img, lowerThreshold=None, upperThreshold=None):

        if lowerThreshold==None:
            lowerThreshold=np.amin(img)

        if upperThreshold == None:
            upperThreshold = np.amax(img)

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
    def threshold_auto(img, method):
        '''
        Apply autothreshold slice by slice
        Run autothreshold on image
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

        #Convert nd Image to ITK image
        itkImage = sitk.GetImageFromArray(img)

        # Get threshold value
        if method == 'IsoData':
            threshold = sitk.IsoDataThresholdImageFilter()

        elif method == 'Otsu':
            threshold = sitk.OtsuThresholdImageFilter()

        elif method == 'Huang':
            threshold = sitk.HuangThresholdImageFilter()

        elif method == 'MaxEntropy':
            threshold = sitk.MaximumEntropyThresholdImageFilter()

        elif method == 'Li':
            threshold = sitk.LiThresholdImageFilter()

        elif method == 'RenyiEntropy':
            threshold = sitk.RenyiEntropyThresholdImageFilter()

        elif method == 'KittlerIllingworth':
            threshold = sitk.KittlerIllingworthThresholdImageFilter()

        elif method == 'Moments':
            threshold = sitk.MomentsThresholdImageFilter()

        elif method == 'Yen':
            threshold = sitk.YenThresholdImageFilter()

        elif method == 'Shanbhag':
            threshold = sitk.ShanbhagThresholdImageFilter()

        else:
            raise LookupError('Not a valid Auto Threshold method!')

        # Get and apply threshold and invert
        segmentedImage = sitk.InvertIntensity(threshold.Execute(itkImage), 1)
        threshold = threshold.GetThreshold()


        return sitk.GetArrayFromImage(segmentedImage), threshold

    @staticmethod
    def threshold2D_auto(img, method):
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

        stack = []
        for i in range(len(img)):
            segmentedImage, _ =Segmentation.threshold_auto(img[i], method)
            stack.append(segmentedImage)

        return stack

    @staticmethod
    def threshold2DMean_auto(img, method, filter='Median'):
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

        thresholdArray=np.zeros(len(img))
        for i in range(len(img)):
            _, threshold = Segmentation.threshold_auto(img[i], method)

            thresholdArray[i]=threshold

        #Calculate final threshold
        if filter=='Mean':
            finalThreshold=np.median(thresholdArray)
        elif filter=='Median':
            finalThreshold = np.mean(thresholdArray)
        else:
            raise LookupError('Not a valid method!')


        #Apply threshold
        return Segmentation.threshold_manual(img, lowerThreshold=0, upperThreshold=finalThreshold)

    @staticmethod
    def tag_image(img):
        # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
        itk_image=sitk.GetImageFromArray(img)
        tagged_image = sitk.ConnectedComponent(itk_image, fullyConnected=True)

        return sitk.GetArrayFromImage(tagged_image)

    @staticmethod
    def generate_surface(tinyImg_tagged):

        img=copy.copy(tinyImg_tagged)

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

        print(overlappingDataBase)
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

        print(dataBaseList[0])
        print(dataBaseList[1])
        print(dataBaseList[2])
        print(overlappingDataBase)

        return dataBaseList, overlappingDataBase

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


if __name__ == '__main__':

    a = Main()

