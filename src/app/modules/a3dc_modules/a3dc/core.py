import SimpleITK as sitk
import numpy as np
import pandas as pd
import copy

from .imageclass import Image
from . import segmentation

'''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains
	the workflows to process images.
	'''

def colocalization_connectivity(taggedImgList, sourceImageList=None):
    '''TODO what happens if tagged image was not previouslz analyzed
    '''
    
    # Create Overlapping Image
    overlapping_array = taggedImgList[0].array
    for i in range(1, len(taggedImgList)):
        overlapping_array = np.multiply(overlapping_array, taggedImgList[i].array)

    #Create overlapping image metadata
    metadata=copy.deepcopy(taggedImgList[0].metadata)
    name=''
    for img in taggedImgList:
        name+=img.metadata['Name']
    metadata['Name']=name
    
    #Create overlapping image
    overlapping_image=Image(segmentation.tag_image(overlapping_array), metadata)
    
    if sourceImageList==None:
        sourceImageList=[overlapping_image]

    else:
        sourceImageList.append(overlapping_image)
    
    overlapping_image=analyze(overlapping_image, sourceImageList)

    # Generate array lists and name lists
    nameList = [x.metadata['Name'] for x in taggedImgList]

    for i in range(len(taggedImgList)):
        itk_image = sitk.GetImageFromArray(taggedImgList[i].array)

        #Get pixel from database
        overlappingPixels=overlapping_image.database['maximumPixel in '+overlapping_image.metadata['Name']]

        objectList = [None for i in range(len(overlappingPixels))]
        ovlRatioList = [None for i in range(len(overlappingPixels))]

        for j in range(len(overlappingPixels)):
            ovlPosition = overlappingPixels[j]
            objectList[j] = itk_image.GetPixel(ovlPosition)

            #if objectList[j] in dataBaseList[i]['dataBase']['tag']:
            ovlRatioList[j] = overlapping_image.database['voxelCount'][j] / taggedImgList[i].database['voxelCount'][taggedImgList[i].database['tag'].index(objectList[j])]
            #else:
                #ovlRatioList[j]=0

        overlapping_image.database['object in ' + nameList[i]] = objectList
        overlapping_image.database['overlappingRatio in ' + nameList[i]] = ovlRatioList


    return overlapping_image


def colocalization_analysis(taggedImgList, overlappingImg):
    

    dataBaseList=[x.database for x in taggedImgList]
    overlappingDataBase=overlappingImg.database

    nameList = [x.metadata['Name'] for x in  taggedImgList]


    NbObjects = [len(x.database['tag']) for x in taggedImgList]
    NbOfInputElements=len(dataBaseList)

    ovlFiltered = 'filter' in overlappingDataBase.keys()
    dictFiltered = [('filter' in x.keys()) for x in dataBaseList]
    
    # Create list of dictionaries to store data
    outputList=[{} for x in range(NbOfInputElements)]

    for i in range(NbOfInputElements):

        outputList[i]['colocalizationCount']=[0  for i in range(NbObjects[i])]
        outputList[i]['totalOverlappingRatio'] = [0  for i in range(NbObjects[i])]

        positionList = [x for x in range(NbOfInputElements) if x != i]

        for j in range(0, len(positionList)):
            outputList[i]['object in '+nameList[positionList[j]]]=[[] for i in range(NbObjects[i])]

    for j in range(len(overlappingDataBase['tag'])):

        #Determine if any of the objects have been filtered out and retrieve tags and their rispected position
        flag=True

        if ovlFiltered==True:
            flag=overlappingDataBase['filter'][j]
            

        currentTagList=[]
        currentPositionList=[]
        for i in range(0,NbOfInputElements):

            currentTag=overlappingDataBase['object in ' + nameList[i]][j]

            currentPosition=dataBaseList[i]['tag'].index(currentTag)

            currentTagList.append(currentTag)
            currentPositionList.append(currentPosition)



            if dictFiltered[i]==True :#and 'filter' in dataBaseList[i].keys() :

                    flag*=dataBaseList[i]['filter'][currentPosition]
                    

        #If none of the objects have been filtered out add info to output
        if flag==True:

            for i in range(NbOfInputElements):

                if currentTagList[i] in dataBaseList[i]['tag']:

                        outputList[i]['colocalizationCount'][currentPositionList[i]]+=1
                        outputList[i]['totalOverlappingRatio'][currentPositionList[i]] += overlappingDataBase['overlappingRatio in ' + nameList[i]][j]
           
                positionList = [x for x in range(NbOfInputElements) if x != i]
                for k in range(len(positionList)):
                    outputList[i]['object in ' + nameList[positionList[k]]][currentPositionList[i]].append(currentTagList[positionList[k]])


    for i in range(NbOfInputElements):
        for key in outputList[i]:
            taggedImgList[i].database[key]=outputList[i][key]
        overlappingImg.database=overlappingDataBase
    
    
    return overlappingImg, taggedImgList 



def analyze(taggedImage, imageList=None, measurementInput=['voxelCount', 'meanIntensity']):

    #Convert tagged image to ITK image
    itkImage = sitk.GetImageFromArray(taggedImage.array)

    # Instatiate ITK LabelIntensityStatisticsImageFilter()
    itkFilter = sitk.LabelIntensityStatisticsImageFilter()

    singleChannelFunctionDict = {'volume': itkFilter.GetPhysicalSize,
                                 'voxelCount': itkFilter.GetNumberOfPixels,
                                 'centroid': itkFilter.GetCentroid,
                                 'ellipsoidDiameter': itkFilter.GetEquivalentEllipsoidDiameter,
                                 'boundingBox': itkFilter.GetBoundingBox,
                                 'pixelsOnBorder': itkFilter.GetNumberOfPixelsOnBorder,
                                 'elongation': itkFilter.GetElongation,
                                 'equivalentSphericalRadius': itkFilter.GetEquivalentSphericalRadius,
                                 'flatness': itkFilter.GetFlatness,
                                 'principalAxes': itkFilter.GetPrincipalAxes,
                                 'principalMoments': itkFilter.GetPrincipalMoments,
                                 'roundness': itkFilter.GetRoundness,
                                 'feretDiameter': itkFilter.GetFeretDiameter,
                                 'perimeter': itkFilter.GetPerimeter,
                                 'perimeterOnBorder': itkFilter.GetPerimeterOnBorder,
                                 'perimeterOnBorderRatio': itkFilter.GetPerimeterOnBorderRatio,
                                 'equivalentSphericalPerimeter': itkFilter.GetEquivalentSphericalPerimeter}

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

    if measurementInput!=None:
        for key in measurementInput:

            if key == 'getFeretDiameter':
                itkFilter.ComputeFeretDiameterOn()

            if key in ['getPerimeter', 'getPerimeterOnBorder', 'getPerimeterOnBorderRatio','getEquivalentSphericalPerimeter']:
                itkFilter.ComputePerimeterOn()

            if (key in singleChannelFunctionDict.keys()) and (key not in singleChMeasurementList):
                singleChMeasurementList.append(key)

            elif (key in dualChannelFunctionDict.keys()) and (key not in dualChMeasurementList):
                dualChMeasurementList.append(key)

    # dataBase dictionary to store results
    dataBase = {'tag': [], }
    
    for key in singleChMeasurementList:
        dataBase[key] = []

    #Cycle through images to measure parameters
    if imageList == None:
        imageList = []
        imageList.append(taggedImage)

    
    for i in range(len(imageList)):

        itkRaw = sitk.GetImageFromArray(imageList[i].array)
        
        for key in dualChMeasurementList:
            dataBase[key+' in '+imageList[i].metadata['Name']] = []

        #Execute Filter and get database
        itkFilter.Execute(itkImage, itkRaw)
        data=itkFilter.GetLabels()

        for label in data:
           
            if i==0:
                dataBase['tag'].append(label)
                for key in singleChMeasurementList:
                    dataBase[key].append(singleChannelFunctionDict[key](label))

            for key in dualChMeasurementList:
                dataBase[key+' in '+imageList[i].metadata['Name']].append(dualChannelFunctionDict[key](label))
    
    #Measure object surface
    if 'surface' in measurementInput:
        #Create and measure Surface Image
        surface=segmentation.create_surfaceImage(taggedImage)
      
        surfaceItkImage=sitk.GetImageFromArray(surface)
        

        itkFilter=sitk.LabelShapeStatisticsImageFilter()
        itkFilter.Execute(surfaceItkImage)
        surfaceData=itkFilter.GetLabels()

        dataBase['surface']=[]
        for label in surfaceData:
            dataBase['surface'].append(singleChannelFunctionDict['voxelCount'](label))
            
    #Determine objects on front and back surface
    if 'onFaces' in measurementInput:
     
        #Create and measure Surface Image
        #☺faces=segmentation.create_surfaceImage()
        facesItkImage=sitk.GetImageFromArray([taggedImage.array[0],taggedImage.array[-1]])
       
      
        itkFilter=sitk.LabelShapeStatisticsImageFilter()
        itkFilter.Execute(facesItkImage)
        facesData=itkFilter.GetLabels()
        
        dataBase['onFaces']=[False]*np.amax(taggedImage.array)
        for label in facesData:
            dataBase['onFaces'][label-1]=True
    
    #Add results to input image
    taggedImage.database=dataBase

    return taggedImage


def filter_dataBase(dictionary, filterDict, overWrite=True, removeFiltered=False):

    dataFrame = pd.DataFrame(dictionary)

    if 'filter' in dictionary.keys():
        oldFilter = dictionary['filter']

    # Only run filter if key has numerical value
    typeList = ['int', 'float', 'bool', 'complex', 'int_', 'intc', 'intp', 'int8', 'int16', 'int32', 'int64'
        , 'uint8', 'uint16', 'uint32', 'uint64', 'float_', 'float16', 'float32', 'float64', 'loat64', 'complex_',
                'complex64', 'complex128']

    for key in filterDict:
        
  
        if dataFrame[key].dtype in typeList:
         
            if 'filter' in dataFrame.keys():
                oldFilter = dataFrame['filter'].values
      
         
            newFilter = (dataFrame[key] >= filterDict[key]['min']) & (dataFrame[key] <= filterDict[key]['max']).values
                

    if ('filter' in dataFrame.keys()) & (overWrite == False):
        dataFrame['filter'] = np.multiply(oldFilter, newFilter)
    
    if removeFiltered==True and 'filter' in dataFrame.keys() :
        dataFrame.drop(dataFrame[dataFrame['filter'] == False].index, inplace=True)

    
    
    dictionary = dataFrame.to_dict(orient='list')
   

    return dictionary


def filter_image(taggedImg):

    dataBase=taggedImg.database

    changeDict={}
    if 'filter' in dataBase.keys():
        for i in range(len(dataBase['filter'])):#dataBase should have a label key!!!
            if dataBase['filter'][i]==False:
                changeDict[int(dataBase['tag'][i])]=0

        itkImage = sitk.GetImageFromArray(taggedImg)

        sitkFilter = sitk.ChangeLabelImageFilter()
        sitkFilter.SetChangeMap(changeDict)

        taggedImg.image = sitk.GetArrayFromImage(sitkFilter.Execute(itkImage))
    else:
        taggedImg.image

    return taggedImg 


#######################################################################################################################################
####################################################################Helper Functions###################################################






#############################################Class that contain main functions for A3DC####################################################






