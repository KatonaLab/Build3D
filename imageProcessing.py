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


class Main(object):

    def __init__(self):

        tstart = time.clock()

        #############################################Load Images####################################################
        sourceImageList=[]
        sourceDictList=[]
        # Channel 1
        ch1Path = ("F:/Workspace/TestImages/test_1.tif")

        ch1Img=Processor.load_image(ch1Path)
        ch1Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch1Img)
        sourceDictList.append(ch1Dict)


        # Channel 2
        ch2Path = ("F:/Workspace/TestImages/test_2.tif")

        ch2Img = Processor.load_image(ch2Path)
        ch2Dict={'name': 'RawImage1',
                 'width':ch1Img.shape[2], 'height':ch1Img.shape[1],'depth':ch1Img.shape[0],
                 'pixelSizeX':0.5,'pixelSizeY':0.5, 'pixelSizeZ':0.5, 'pixelSizeUnit':'um' }

        sourceImageList.append(ch2Img)
        sourceDictList.append(ch2Dict)

        # Channel 3
        ch3Path = ("F:/Workspace/TestImages/test_3.tif")
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
        thresholdedImage=Segmentation.threshold2D_auto(sourceImageList[0], "Otsu")
        taggedImage=Segmentation.tag_image(thresholdedImage)

        taggedDict1=sourceDictList[0]
        taggedDict1['name']='Channel1'

        taggedImageList.append(taggedImage)
        taggedDictList.append(taggedDict1)


        # Channel 2
        thresholdedImage = Segmentation.threshold2D_auto(sourceImageList[1], "MaxEntropy")
        taggedImage = Segmentation.tag_image(thresholdedImage)

        taggedDict2=sourceDictList[1]
        taggedDict2['name']='Channel2'


        taggedImageList.append(taggedImage)
        taggedDictList.append(taggedDict2)

        # Channel 3
        thresholdedImage = Segmentation.threshold2D_auto(sourceImageList[2], "MaxEntropy")
        taggedImage = Segmentation.tag_image(thresholdedImage)

        taggedDict3 =sourceDictList[2]
        taggedDict3['name'] = 'Channel3'

        taggedImageList.append(taggedImage)
        taggedDictList.append(taggedDict3)
        #############################################################################################################

        #############################################Analysis Images####################################################
        #print(taggedDictList[0])
        #print(sourceDictList[0])
        Measurement.analyze(taggedImageList[0], taggedDictList[0])

        print(taggedDictList[0]['dataBase'])

        dictFilter={'volume':{'min':2, 'max':11}}#, 'mean in '+taggedDictList[0]['name']: {'min':2, 'max':3}}

        Measurement.filter(taggedDictList[0],dictFilter)


        #print(taggedImage)
        tstop = time.clock()
        print('ITK STATS: ' + str(tstop - tstart))







        '''
        # Channel 1
        taggedImageList.append(Segmentation.tag_image(sourceImageList[0]))
        #Channel 3
        taggedImageList.append(Segmentation.tag_image(sourceImageList[1]))
        #Channel 2
        taggedImageList.append(Segmentation.tag_image(sourceImageList[2]))


        #print('ch1')
        #print(Segmentation.tag_image(sourceImageList[0]))
        #print('ch2')
        #print(Segmentation.tag_image(sourceImageList[1]))
        #print('ch3')
        #print(Segmentation.tag_image(sourceImageList[2]))
        #############################################################################################################
        ############################################Analyze Input####################################################
        analysisInput = []
        analysisInput.append({'chanelList':[0, 1, 2], 'isOnEdge': True, 'boundingBox': True, 'surface':True, 'centroid':True})#channel 1)
        analysisInput.append({'chanelList': [0, 1, 2], 'isOnEdge': True, 'boundingBox': True, 'surface': True,'centroid': True})  # Channel 3
        analysisInput.append({'chanelList':[0, 1, 2], 'isOnEdge': True, 'boundingBox': True, 'surface':True, 'centroid':True})#Channel 4

        overlappingAnalysisInput=  {'chanelList':[], 'isOnEdge': False, 'boundingBox': False, 'surface':False, 'centroid':True}#Overlapping

        ###############################################Results######################################################
        dataBaseList = []
        dataBaseList.append(Measurement.analyze(taggedImageList[0], 'Ch' + str(1), sourceImageList, **analysisInput[0]))
        dataBaseList.append(Measurement.analyze(taggedImageList[1], 'Ch' + str(2), sourceImageList, **analysisInput[1]))
        dataBaseList.append(Measurement.analyze(taggedImageList[2], 'Ch' + str(3), sourceImageList, **analysisInput[2]))


        #############################################MAIN FUNCTION############################################################


        #Generate overlapping map
        # Generate and analyzeOverlapping area

        #print('overlappingImage')
        #print(overlappingImage)
        #print(overlappingDataBase)

        #print(len(taggedImageList))
        #print(len(dataBaseList))
        #print(len(overlappingDataBase))
        overlappingimage, overlappingDataBase=Measurement.colocalization_overlap(taggedImageList, overlappingAnalysisInput)

        overlappingDataBase=Measurement.colocalization_connectivity(taggedImageList, dataBaseList, overlappingDataBase)


        Measurement.colocalizaion_analysis(taggedImageList, dataBaseList, overlappingDataBase)



        #Measurement.colocalize([taggedImageList[0],taggedImageList[1]],[dataBaseList[0],dataBaseList[1]], overlappingDataBase)


        #dataDict=Measurement.analyze(taggedImageList[0], sourceImageList)
        #tstart = time.clock()
        #with open('D:/Playground/data.txt','wb') as fin:
            #pickle.dump(dataDict, fin)
        #tstop = time.clock()
        #print('Time to PickleObject: ' + str(tstop - tstart))

        #tstart = time.clock()
        #dataDict = pickle.load(open('D:/Playground/data.txt', "rb"))
        #tstop = time.clock()
        #print('Time to losad PickleObject: ' + str(tstop - tstart))

        #print(dataDict)




        #resultsDict=Measurement.analyze(taggedImg, ch1Image)
        #print(resultsList)
        #############################################################################################################
        ############################################Colocalization###################################################

        '''


        #Measurement.colocalization(taggedImageList, sourceImageList)


        #############################################################################################################
        #print('Index:')
        #print(resultsDict['PixelLists']['Index'])
        #print('Cartesian:')
        #print(resultsDict['PixelLists']['Cartesian X'])
        #print(resultsDict['PixelLists']['Cartesian Y'])
        #print(resultsDict['PixelLists']['Cartesian Z'])
        #print('Bounding Box:')
        #print(resultsDict['dataBase']['Bounding box X1'])
        #print(resultsDict['dataBase']['Bounding box X2'])
        #print(resultsDict['dataBase']['Bounding box Y1'])
        #print(resultsDict['dataBase']['Bounding box Y2'])
        #print(resultsDict['dataBase']['Bounding box Z1'])
        #print(resultsDict['dataBase']['Bounding box Z2'])
        #print('Surface:')
        #print(resultsDict['PixelLists']['Surface'])
        #print('Surface List:')
        #print(resultsDict['dataBase']['Surface Area'])


        #import os
        #dirPath = "D:\Playground\coloc"
        #for file in os.listdir(dirPath):
            #if file.endswith(".tif"):
                #filePath = os.path.join(dirPath, file)

                #img = processor.load_image(filePath)

                #tstart = time.clock()
                #analyze(img)
                #tstop = time.clock()
                #print('Time to Tagg Image: ' + str(tstop - tstart))


class Measurement(object):
    '''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
    	the workflows to process images.
    	'''



    @staticmethod
    def analyze(taggedImage, dictionary, imageList=[], dictionaryList=[]):

        if imageList==[]:
            imageList.append(taggedImage)
            dictionaryList.append(dictionary)

        dataBase = {'volume': [], 'voxelCount': [], 'centroid': [], 'ellipsoidDiameter': [],
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
                    dataBase['volume'].append(itkFilter.GetPhysicalSize(label))
                    dataBase['voxelCount'].append(itkFilter.GetNumberOfPixels(label))
                    dataBase['centroid'].append(itkFilter.GetCentroid(label))
                    dataBase['ellipsoidDiameter'].append(itkFilter.GetEquivalentEllipsoidDiameter(label))
                    dataBase['boundingBox'].append(itkFilter.GetBoundingBox(label))

                dataBase['mean in '+dictionaryList[i]['name']].append(itkFilter.GetMean(label))
                dataBase['pixel in '+dictionaryList[i]['name']].append(itkFilter.GetMaximumIndex(label))
                dataBase['centerOfMass in '+dictionaryList[i]['name']].append(itkFilter.GetCenterOfGravity(label))


        dictionary['dataBase']=dataBase

        return dataBase


    @staticmethod
    def filter(dictionary, filterDict, removeFiltered=True):

        dataFrame=pd.DataFrame(dictionary['dataBase'])

        #print(dataFrame)
        for key in filterDict:
            if removeFiltered==False:
                dataFrame['filterd']=(dataFrame[key]>=filterDict[key]['min'])&(dataFrame[key]<=filterDict[key]['max'])

            elif removeFiltered == True:
                dataFrame=dataFrame[(dataFrame[key]>=filterDict[key]['min'])&(dataFrame[key]<=filterDict[key]['max'])]

        dictionary['dataBase'] = dataFrame.to_dict(orient='list')
       
        return dictionary











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
    def threshold_manual(img,  lower, upper=pow(2, 32)):
        # Threshold image using lower and upper as range
        return (lower < img) & (img < upper)


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

