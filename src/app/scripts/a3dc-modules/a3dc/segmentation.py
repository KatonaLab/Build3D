# -*- coding: utf-8 -*-
from skimage import img_as_ubyte, img_as_uint
from skimage.filters import threshold_local
import cv2
import numpy as np
import SimpleITK as sitk


def tag_image(ndarray):

    #Convert ndarray to itk image
    itk_image = sitk.GetImageFromArray(ndarray)

    #Cast images to 8-bit as images should be binary anyway
    #itk_image=sitk.Cast(itk_image, sitk.sitkUInt8)

    # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
    ndarray = sitk.ConnectedComponent(itk_image, fullyConnected=True)

    return sitk.GetArrayFromImage(ndarray)
 

def threshold_auto(ndarray, method, mode='Slice'):

    # Cast to 16-bit
    #image = img_as_uint(image)

    # Cast to 16-bit
    ndarray = img_as_ubyte(ndarray)

    itkThresholdDict = {'IsoData': sitk.IsoDataThresholdImageFilter(), 'Otsu': sitk.OtsuThresholdImageFilter(),
                            'Huang': sitk.HuangThresholdImageFilter(),
                            'MaxEntropy': sitk.MaximumEntropyThresholdImageFilter(),
                            'Li': sitk.LiThresholdImageFilter(),
                            'RenyiEntropy': sitk.RenyiEntropyThresholdImageFilter(),
                            'KittlerIllingworth': sitk.KittlerIllingworthThresholdImageFilter(),
                            'Moments': sitk.MomentsThresholdImageFilter(), 'Yen': sitk.YenThresholdImageFilter(),
                            'Shanbhag': sitk.ShanbhagThresholdImageFilter(),
                            'Triangle': sitk.TriangleThresholdImageFilter()}

    #Check if method is valid
    if method not in itkThresholdDict.keys():
        raise LookupError("'" + str(method) + "' is Not a valid mode!")

    #Create ITK FIlter object
    thresholdFilter=itkThresholdDict[method]

    if mode=="Stack" or len(ndarray.shape)<3 :
        # Create ITK image
        itkImage = sitk.GetImageFromArray(ndarray)
        # Apply threshold, invert and convert to nd array
        outputImage=sitk.GetArrayFromImage(sitk.InvertIntensity(thresholdFilter.Execute(itkImage), 1))
        #Get threshold value
        thresholdValue = thresholdFilter.GetThreshold()

    elif mode=="Slice":

        thresholdValue=[]
        outputImage=[]
        for i in range(len(ndarray)):
            # Create ITK image
            itkImage = sitk.GetImageFromArray(ndarray[i])
            # Apply threshold, invert and convert to nd array
            segmentedImage = sitk.GetArrayFromImage(sitk.InvertIntensity(thresholdFilter.Execute(itkImage), 1))
            #Append threshold value to threshodValue
            thresholdValue.append(thresholdFilter.GetThreshold())
            #Append slice to outputImage
            outputImage.append(segmentedImage)
        outputImage=np.asarray(outputImage)

    else:
        raise LookupError("'"+str(mode)+"' is Not a valid mode!")


    return outputImage, thresholdValue


def create_surfaceImage(ndarray):

    # Convert nd array to itk image
    itkImage = sitk.GetImageFromArray(ndarray)
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



def threshold_manual(ndarray, lowerThreshold=0, upperThreshold=1):

    # Convert nd Image to ITK image
    itkImage = sitk.GetImageFromArray(ndarray)

    #threshold=sitk.ThresholdImageFilter()
    threshold=sitk.BinaryThresholdImageFilter()
    threshold.SetUpperThreshold(float(upperThreshold))
    threshold.SetLowerThreshold(float(lowerThreshold))
    segmentedImage=threshold.Execute(itkImage)

    # Threshold and Invert
    return sitk.GetArrayFromImage(sitk.InvertIntensity(segmentedImage, 1))



def threshold_adaptive(ndarray, method, blockSize=5, offSet=0):
    
    #Round to add as blocksize has to be odd
    def round_up_to_odd(f):
        return int(np.ceil(f) // 2 * 2 + 1)
    
    blockSize=round_up_to_odd(blockSize)
    
    #Cast to 8-bit
    convertedImage = img_as_ubyte(ndarray)

    #Cycle through image
    outputImage = []
    for i in range(len(convertedImage)):
        if method == 'Adaptive Mean':
            outputImage.append(threshold_local(convertedImage[i], blockSize, offSet))

        elif method == 'Adaptive Gaussian':

            outputImage.append(cv2.adaptiveThreshold(convertedImage[i], 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                                cv2.THRESH_BINARY, blockSize, offSet))
        else:
            raise LookupError('Not a valid method!')

    return np.asarray(outputImage)


