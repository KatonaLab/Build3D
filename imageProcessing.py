import SimpleITK as sitk
import cv2
import numpy as np
from math import pow
from skimage.external.tifffile import imread, imsave
from skimage.filters import threshold_isodata, threshold_li, threshold_local, threshold_mean, threshold_minimum, threshold_niblack, threshold_otsu, threshold_sauvola, threshold_triangle, threshold_yen

from scipy.ndimage.filters import convolve
import time
import os
import multiprocessing as mp
import copy
from itertools import chain
import matplotlib.pyplot as plt



class Main(object):

    def __init__(self):
        tstart = time.clock()
        tinyPath = ("D:/Playground/test3.tif")  # tinytest.tif")
        # Channel 1

        # Channel 2
        image=Processor.load_image(tinyPath)
        Measurement.analyze(image)

        tstop = time.clock()
        print('Time to Tagg Image: ' + str(tstop - tstart))
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

class Segmentation(object):

    @staticmethod
    def threshold_manual(image,  lower, upper=pow(2, 32)):
        # Threshold image using lower and upper as range
        return (lower < img) & (img < upper)

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

class Measurement(object):
    '''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
    	the workflows to process images.
    	'''

    @staticmethod
    def analyze(img):
        taggedImg = Segmentation.tag_image(img)
        pixelCoordinateList = Measurement.create_pixelCoordinateList(taggedImg)
        volumeList = Measurement.measure_volume(pixelCoordinateList)
        intensityList = Measurement.create_intensityList(img, pixelCoordinateList)
        print(len(intensityList))
        meanIntensityList=Measurement.measure_mean(intensityList)
        isOnEdgeList = Measurement.isOnEdge(taggedImg)
        surfacePixelCoordinateList = Measurement.create_surfacePixelList(taggedImg, pixelCoordinateList)
        cartesianCoordinateList = Measurement.create_cartesianList(taggedImg, pixelCoordinateList)
        centroidList = Measurement.create_centroidList(cartesianCoordinateList[0], cartesianCoordinateList[1],cartesianCoordinateList[2])
        boundingBoxList=Measurement.measure_boundingBox(cartesianCoordinateList[0],cartesianCoordinateList[1], cartesianCoordinateList[2])
        print(meanIntensityList)
        #print(isOnEdgeList)
        #print(volumeList)
        #print(intensityList)

        #print(isOnEdgeList)
        #print(surfacePixelCoordinateList)
        #print(cartesianCoordinateList)
        #print(centroidList)

    @staticmethod
    def tag_image(image):
        # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
        itk_image=sitk.GetImageFromArray(image)
        tagged_image = sitk.ConnectedComponent(itk_image, fullyConnected=True)

        return sitk.GetArrayFromImage(tagged_image)

    @staticmethod
    def create_pixelCoordinateList_core(input):

        imgArray=input[0]
        length=input[1]
        arrayOffset=input[2]
        coordinates = [[] for i in range(length)]

        i = 0
        for k in range(imgArray.size):
            i += 1
            if imgArray[k] != 0:
                coordinates[imgArray[k] - 1].append(k+arrayOffset)

        return coordinates

    @staticmethod
    def create_pixelCoordinateList(image_tagged):

        NUM_WORKERS = mp.cpu_count() + 2
        imgArray = image_tagged.flatten()

        #Split array to feed to processes
        split = np.array_split(imgArray, NUM_WORKERS)
        length = np.amax(imgArray)

        #Input Parameters for the separate processes
        objectListLength = [length for i in range(0, NUM_WORKERS)]
        arrayOffsetList=[0 for i in range(0, NUM_WORKERS)]
        #Offset of pixels
        for i in range(1,NUM_WORKERS):
            arrayOffsetList[i]=arrayOffsetList[i-1]+len(split[i-1])
        input = zip(split, objectListLength, arrayOffsetList)

        #Paralel processesing of split lists
        with mp.Pool(processes=NUM_WORKERS) as pool:
            multiple_results = pool.map(Measurement.create_pixelCoordinateList_core, input)

        #Combine Results from the different processes
        objectCoordinateList = [[] for i in range(0, len(multiple_results[0]))]

        arrayOffset=0
        for j in range(length):
            for i in range(len(multiple_results)):
                objectCoordinateList[j] = objectCoordinateList[j] + multiple_results[i][j]
            arrayOffset+=len(split[i])

        return objectCoordinateList

    @staticmethod
    def isOnEdge(tinyImg_tagged):

        shape=tinyImg_tagged.shape
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
    def create_surfacePixelList(image, objectCoordinateList):

        image_copy = copy.copy(image)

        #Measure the number of neighbors using convolution, objects that have one pixel are deleted (see later)
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

        #Create surfaceList from surface image. Objects with a single pixel are added to the list here.
        length = len(objectCoordinateList)
        surfaceArray=convolved.flatten()
        surfaceCoordinates=[[] for i in range(0, length)]
        for i in range(length):
            for j in range(len(objectCoordinateList[i])):
                buffer=surfaceArray[objectCoordinateList[i][j]]
                if len(objectCoordinateList[i])==1:
                    surfaceCoordinates[i].append(objectCoordinateList[i][j])
                if buffer>0:
                    surfaceCoordinates[i].append(objectCoordinateList[i][j])

        return surfaceCoordinates

    @staticmethod
    def create_cartesianList(tinyImg_tagged,objectList):

        imageShape = tinyImg_tagged.shape
        length = len(objectList)

        slicePixelnumber = int(imageShape[1] * imageShape[2])

        x = [[] for i in range(length)]
        y = [[] for i in range(length)]
        z = [[] for i in range(length)]

        for i in range(len(objectList)):
            for j in range(len(objectList[i])):
                location = objectList[i][j]

                zcoord=int(location / slicePixelnumber)
                ycoord=int((location - zcoord * slicePixelnumber) / imageShape[2])
                xcoord=(location - zcoord * slicePixelnumber - ycoord * imageShape[2])

                z[i].append(zcoord)
                y[i].append(ycoord)
                x[i].append(xcoord)

        return x, y, z

    @staticmethod
    def create_centroidList(cartesianListX,cartesianListY, cartesianListZ):


        length=len(cartesianListX)

        centroids_x=np.zeros(length)
        centroids_y = np.zeros(length)
        centroids_z = np.zeros(length)
        for i in range(length):

            centroids_x[i]=np.mean(cartesianListX[i])
            centroids_y[i]=np.mean(cartesianListY[i])
            centroids_z[i]=np.mean(cartesianListZ[i])

        return centroids_x, centroids_y, centroids_z

    @staticmethod
    def measure_boundingBox(cartesianListX,cartesianListY, cartesianListZ):

        length=len(cartesianListX)

        boundingX = [[] for i in range(0, length)]
        boundingY = [[] for i in range(0, length)]
        boundingZ = [[] for i in range(0, length)]

        for i in range(length):
            boundingX.append((np.amax(cartesianListX[i]),np.amin(cartesianListX[i])))
            boundingY.append((np.amax(cartesianListY[i]),np.amin(cartesianListY[i])))
            boundingZ.append((np.amax(cartesianListZ[i]),np.amin(cartesianListZ[i])))

        return boundingX, boundingY, boundingZ

    @staticmethod
    def measure_mean(objectList):

        length=len(objectList)

        meanList = np.zeros(length)

        for i in range(length):
            meanList[i] = np.mean(objectList[i])
        print('Flowerpot')
        print(meanList)
        return meanList

    @staticmethod
    def create_intensityList(image, objectList):


        imgArray = image.flatten()
        intensityList = [[] for i in range(0, len(objectList))]
        print(len(intensityList))

        for j in range(len(objectList)):
            for i in range(len(objectList[j])):
                intensityList[j].append(imgArray[objectList[j][i]])

        print(intensityList)
        return intensityList

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
    def find_overlapping(image1,  image2):
        # Create an overlapping image by pixelwise multiplication
        return np.multiply(image1,image2)

    @staticmethod
    def find_objects(image):
        # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
        itk_image=sitk.GetImageFromArray(image)
        tagged_image = sitk.ConnectedComponent(itk_image, fullyConnected=True)

        return sitk.GetArrayFromImage(tagged_image)



    @staticmethod
    def identify_overlapping_objects(image1, image2, ImageOverlapping):
        """Identifies overlapping objects in two tagged image pixel arrays (taggedObjArray1 and taggedObjArray2) and returns using
        a pixel array of their overlapping image (taggedOverlappingArray ). These arrays
        are Java arrays to speed up processing times. Returns a dictionarry containing the connectivity information

        taggedObjArray1, taggedObjArray2, taggedOverlappingArray: Java arrays
        """
        # Check if image arrays at input have the same length

        #Get image properties and check if them shape of numpy array is the same
        image1_shape=image1.shape
        image2_shape=image2.shape
        ImageOverlapping_shape=ImageOverlapping.shape

        if image1_shape != image2_shape != ImageOverlapping_shape:
            raise CoexpressError('Input image arrays have to have the same length!', '')


        # Generate Java array for output: overlappingTaggs1(2) contain the taggs in taggedObjArray1(2) that overlap with
        # taggedOverlappingArray.
        NbOverlappingObj = np.amax(ImageOverlapping)
        taggs_ch1 = np.zeros(NbOverlappingObj, dtype='uint32')
        taggs_ch2 = np.zeros(NbOverlappingObj, dtype='uint32')

        # Check which objects overlap: if
        for depth in  range(0, image1_shape[0]):
            for width in range(0, image1_shape[1]):
                for height in range(0, image1_shape[2]):
                    currOvlID = ImageOverlapping[depth][width][height] - 1
                    if (ImageOverlapping[depth][width][height] > 0 and (taggs_ch1[currOvlID] == 0 or taggs_ch2[currOvlID] == 0)):
                        taggs_ch1[currOvlID] = image1[depth][width][height]
                        taggs_ch2[currOvlID] = image2[depth][width][height]

        outputDict = {"taggs_ch1": taggs_ch1, "taggs_ch2": taggs_ch2}

        return outputDict

    @staticmethod
    def scanImage(image):

        image_shape = image.shape
        a=image.flatten()

        for i in range(0, len(a)):
            currID=a[i]

        print(a, currID)

        objectList = [[] for i in range(0, np.amax(image))]
        # objectDict=[objectDict[i]=[] for i in range(0, np.amax(image))]
        # objectDict = {k: [] for k in range(0, np.amax(image))}
        #itk_image = sitk.GetImageFromArray(image)
        #for depth in range(0, itk_image.GetSize()[2]):
            #for width in range(0, itk_image.GetSize()[1]):
                #for height in range(0, itk_image.GetSize()[0]):
                    #currID = image[depth][width][height] - 1
                    #itk_image[width, height, depth ]


        return print("Hell Yeah!")

    @staticmethod
    def image_to_objectList(image):

        image_shape = image.shape

        objectList = [[] for i in range(0, np.amax(image))]
        #objectDict=[objectDict[i]=[] for i in range(0, np.amax(image))]
        #objectDict = {k: [] for k in range(0, np.amax(image))}

        for depth in range(0, image_shape[0]):
            for width in range(0, image_shape[1]):
                for height in range(0, image_shape[2]):
                    currID = image[depth][width][height] - 1
                    #print((depth, width, height))
                    if currID>=0 :
                        objectList[currID].append((depth, width, height))
                        #objectDict[currID].append((depth, width, height))
        #
        #objectDict={}
        #for k in range(0, len(objectList)):
            #objectDict[k]=np.array(objectList[k], dtype='uint32')

        return np.array(objectList, dtype='uint32')

    @staticmethod
    def measure_volume(objectList):

        volumes = np.zeros(len(objectList), dtype='uint32')

        for i in range(0, len(objectList)):
            volumes[i]=len(objectList[i])

        return volumes

    @staticmethod
    def measure_intensity(objectList, image):

        intensities = np.zeros(len(objectList), dtype='uint32')

        for i in range(0, len(objectList)):

            intContainer=0

            for j in range(0, len(objectList[i])):
                intContainer += image[objectList[i][j][0]][objectList[i][j][1]][objectList[i][j][2]]

            intensities[i]=intContainer

        return intensities


    @staticmethod
    def measure_centroid(objectList, volumeList):

        centroids = [[] for i in range(0, len(objectList))]

        for i in range(0, len(objectList)):

            centroidContainer = (0,0,0)

            for j in range(0, len(objectList[i])):

                centroidContainer[0]+=objectList[i][j][0]
                centroidContainer[1] += objectList[i][j][1]
                centroidContainer[2] += objectList[i][j][2]

            centroids[i]=centroidContainer

        return np.divide( np.array(centroids, dtype='uint32'), volumeList)


    @staticmethod
    def measure_baricenter(objectList, intensityList, image):

        baricenters = [[] for i in range(0, len(objectList))]

        for i in range(0, len(objectList)):
            baricenterContainer = (0, 0, 0)

            for j in range(0, len(objectList[i])):
                intContainer += image[objectList[i][j][0]][objectList[i][j][1]][objectList[i][j][2]]

                pixelintensity=image[objectList[i][j][0]][objectList[i][j][1]][objectList[i][j][2]]

                baricenterContainer[0] += pixelintensity*objectList[i][j][0]
                baricenterContainer[1] += pixelintensity*objectList[i][j][1]
                baricenterContainer[2] += pixelintensity*objectList[i][j][2]

            baricenters[i] = baricenterContainer

        return np.divide(np.array(baricenters, dtype='uint32'), intensityList)



    @staticmethod
    def analyze_tagged_array(width, height, NbSlices, pixelArray=None, taggedArray=None):
        """Analyze tagged image array. Measures the number of objects, volume, cumulative intensity and mean intensity of each object in 'taggedImgArray' using pixel values in
        'imgArray'(the original image). Return value is a dictionary containing the number of objects and lists of the tags of each object with their respective volumes and intensities

        imgArray: pixel array with the original image
        taggedImgArray: tagged pixel array
        Widht: Widht of the image
        Height: height of the image
        NbSlices: number of slices in the stack
        """

        simpleFlag = False
        if pixelArray == None:
            simpleFlag = True

        length = width * height * NbSlices
        maxTag = taggedArray[0]
        for i in xrange(1, length):
            if (taggedArray[i] > maxTag):
                maxTag = int(taggedArray[i])

        #######################################################Inizialize output arrays##############################################################
        # Initialize Java arrays to store volume,intensity and isOnEdge state
        volumeList = zeros(maxTag, 'i')
        isOnEdgeList = zeros(maxTag, 'z')
        centroidXList = zeros(maxTag, 'd')
        centroidYList = zeros(maxTag, 'd')
        centroidZList = zeros(maxTag, 'd')

        if simpleFlag == False:
            intensityList = zeros(maxTag, 'd')
            barycenterXList = zeros(maxTag, 'd')
            barycenterYList = zeros(maxTag, 'd')
            barycenterZList = zeros(maxTag, 'd')

        #################################Mesaure volume and intensity and determine if object is on edge using tagged image###########################
        arrayIndex = 0
        for z in xrange(1, NbSlices + 1):
            for y in xrange(0, width):
                for x in xrange(0, height):

                    # print taggedArray[arrayIndex]
                    if (taggedArray[arrayIndex] != 0):
                        currentID = int(taggedArray[arrayIndex]) - 1

                        volumeList[currentID] = volumeList[currentID] + 1

                        centroidXList[currentID] = centroidXList[currentID] + x
                        centroidYList[currentID] = centroidYList[currentID] + y
                        centroidZList[currentID] = centroidZList[currentID] + z

                        if simpleFlag == False:
                            intensityList[currentID] = intensityList[currentID] + pixelArray[arrayIndex]
                            barycenterXList[currentID] = barycenterXList[currentID] + pixelArray[arrayIndex] * x
                            barycenterYList[currentID] = barycenterYList[currentID] + pixelArray[arrayIndex] * y
                            barycenterZList[currentID] = barycenterZList[currentID] + pixelArray[arrayIndex] * z

                        # Check if the current particle is touching an edge
                        if (x == 0 or y == 0 or x == width - 1 or y == height - 1 or (
                                NbSlices != 1 and (z == 1 or z == NbSlices))):
                            isOnEdgeList[currentID] = True
                    arrayIndex += 1

        ##########################Finalize output data:calculate barycenters, centroids and remove zero elements, sort based on tags##########################################
        #Calculate barycenters/centroidsCreate output lists that contains tuples of output parameters that can be easily sorted
        dataMatrix = []
        NbObjects = 0
        for i in xrange(0, len(volumeList)):
            if volumeList[i] > 0:
                NbObjects += 1

                # Calculate centroid coordinates
                centroidX = centroidXList[i] / float(volumeList[i])
                centroidY = centroidYList[i] / float(volumeList[i])
                centroidZ = centroidZList[i] / float(volumeList[i])

                if simpleFlag == False:
                    # Calculate MeanGrey intensity
                    meanGreyintensity = float(intensityList[i]) / float(volumeList[i])

                    # Calculate baricenter coordinates if intensity is larger than zero (eg. if the imgArray is randomized or from an other chanel!!!)
                    if intensityList[i] > 0:
                        barycenterX = barycenterXList[i] / float(intensityList[i])
                        barycenterY = barycenterYList[i] / float(intensityList[i])
                        barycenterZ = barycenterZList[i] / float(intensityList[i])
                    else:
                        barycenterX = 'N/A'
                        barycenterY = 'N/A'
                        barycenterZ = 'N/A'

                    dataMatrix.append((i + 1, volumeList[i], isOnEdgeList[i], centroidX, centroidY, centroidZ,
                                   intensityList[i], meanGreyintensity, barycenterX, barycenterY, barycenterZ))
                if simpleFlag == True:
                    # dataMatrix.append((i,volumeList[i],intensityList[i],meanGreyintensity,isOnEdgeList[i],barycenterX,barycenterY,barycenterZ,centroidX,centroidY,centroidZ))
                    dataMatrix.append((i + 1, volumeList[i], isOnEdgeList[i], centroidX, centroidY, centroidZ))

        # sort output based on tag number in ascending order, Sorting taggs is important when looking for tag values
        dataMatrix.sort(reverse=False, key=lambda x: x[0])

        # Create list for each output parameter
        outputVolumeList = []
        outputIsOnEdgeList = []
        outputMeanIntensityList = []
        outputTagList = []
        outputCentroidXList = []
        outputCentroidYList = []
        outputCentroidZList = []

        if simpleFlag == False:
            outputIntensityList = []
            outputBarycenterXList = []
            outputBarycenterYList = []
            outputBarycenterZList = []

        for i in range(0, len(dataMatrix)):
            outputTagList.append(dataMatrix[i][0])
            outputVolumeList.append(dataMatrix[i][1])
            outputIsOnEdgeList.append(dataMatrix[i][2])
            outputCentroidXList.append(dataMatrix[i][3])
            outputCentroidYList.append(dataMatrix[i][4])
            outputCentroidZList.append(dataMatrix[i][5])

            if simpleFlag == False:
                outputIntensityList.append(dataMatrix[i][6])
                outputMeanIntensityList.append(dataMatrix[i][7])
                outputBarycenterXList.append(dataMatrix[i][8])
                outputBarycenterYList.append(dataMatrix[i][9])
                outputBarycenterZList.append(dataMatrix[i][10])

        outputDict = {"volume": outputVolumeList, "isOnEdge": outputIsOnEdgeList, 'tag': outputTagList,
                    "centroidX": outputCentroidXList, "centroidY": outputCentroidYList, "centroidZ": outputCentroidZList}

        if simpleFlag == False:
            outputDict["intensity"] = outputIntensityList
            outputDict["meanIntensity"] = outputMeanIntensityList
            outputDict["barycenterX"] = outputBarycenterXList
            outputDict["barycenterY"] = outputBarycenterYList
            outputDict["barycenterZ"] = outputBarycenterZList

        return outputDict

class CoexpressError(Exception):

    '''Class for error handling '''

    def __init__(self, message, errors):
        super(CoExpressError, self).__init__(message)
        self.errors = errors

    def __str__(self):
        return repr(self.message)


if __name__ == '__main__':

    a = Main()

