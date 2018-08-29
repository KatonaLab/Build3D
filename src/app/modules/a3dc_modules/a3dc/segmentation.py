# -*- coding: utf-8 -*-
from skimage import img_as_ubyte, img_as_uint
from skimage.filters import threshold_local
import cv2
import numpy as np
import SimpleITK as sitk
from .utils import round_up_to_odd


def tag_image(ndarray):

    # Cast to 16-bit
    ndarray = img_as_uint(ndarray)
    
    #Convert ndarray to itk image
    itk_image = sitk.GetImageFromArray(ndarray)

    #Cast images to 8-bit as images should be binary anyway
    #itk_image=sitk.Cast(itk_image, sitk.sitkUInt)

    # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
    ndarray = sitk.ConnectedComponent(itk_image, fullyConnected=True)

    return sitk.GetArrayFromImage(ndarray)
 

def threshold_auto(ndarray, method, mode='Slice'):

    #Initialization
    threshold_dict = {'IsoData': sitk.IsoDataThresholdImageFilter(), 'Otsu': sitk.OtsuThresholdImageFilter(),
                            'Huang': sitk.HuangThresholdImageFilter(),
                            'MaxEntropy': sitk.MaximumEntropyThresholdImageFilter(),
                            'Li': sitk.LiThresholdImageFilter(),
                            'RenyiEntropy': sitk.RenyiEntropyThresholdImageFilter(),
                            'KittlerIllingworth': sitk.KittlerIllingworthThresholdImageFilter(),
                            'Moments': sitk.MomentsThresholdImageFilter(), 'Yen': sitk.YenThresholdImageFilter(),
                            'Shanbhag': sitk.ShanbhagThresholdImageFilter(),
                            'Triangle': sitk.TriangleThresholdImageFilter()}

    mode_list=['Stack','Slice']
    
    if method not in threshold_dict.keys():
        raise Exception('Method has to be amond the following:\n'+str(threshold_dict.keys))
    if mode not in mode_list:
        raise Exception('Mode has to be amond the following:\n'+str(mode_list))
    
    
    # Cast to 16-bit
    ndarray = img_as_uint(ndarray)

    # Cast to 8-bit
    #ndarray = img_as_ubyte(ndarray)
    

    #Check if method is valid
    if method not in threshold_dict.keys():
        raise LookupError("'" + str(method) + "' is Not a valid mode!")

    #Create ITK FIlter object
    threshold_filter=threshold_dict[method]

    if mode=="Stack" or len(ndarray.shape)<3 :
        # Create ITK image
        itk_image = sitk.GetImageFromArray(ndarray)
        # Apply threshold, invert and convert to nd array
        output=sitk.GetArrayFromImage(sitk.InvertIntensity(threshold_filter.Execute(itk_image), 1))
        #Get threshold value
        threshold_val = threshold_filter.GetThreshold()

    elif mode=="Slice":

        threshold_val=[]
        output=[]
        for i in range(len(ndarray)):
            # Create ITK image
            itk_image = sitk.GetImageFromArray(ndarray[i])
            # Apply threshold, invert and convert to nd array
            segmented_img = sitk.GetArrayFromImage(sitk.InvertIntensity(threshold_filter.Execute(itk_image), 1))
            #Append threshold value to threshodValue
            threshold_val.append(threshold_filter.GetThreshold())
            #Append slice to outputImage
            output.append(segmented_img)
        output=np.asarray(output)

    else:
        raise LookupError("'"+str(mode)+"' is Not a valid mode!")


    return output, threshold_val


def create_surfaceImage(ndarray):

    # Convert nd array to itk image
    itk_image = sitk.GetImageFromArray(ndarray)
    pixel_type = itk_image.GetPixelID()
    # Run binary threshold
    thresholded_itk_img = sitk.BinaryThreshold(itk_image, 0, 0, 0, 1)

    # Create an parametrize instance ofBinaryContourImageFilter()
    itk_filter = sitk.BinaryContourImageFilter()
    itk_filter.SetFullyConnected(False)
    #Execute to get a surface mask
    output = itk_filter.Execute(thresholded_itk_img)

    # Change pixel_type of the object map to be the same as the input image
    caster = sitk.CastImageFilter()
    caster.SetOutputpixel_type(pixel_type)
    output=caster.Execute(output)*itk_image

    return sitk.GetArrayFromImage(output)



def threshold_manual(ndarray, upper=1, lower=0):
 
    # Cast to 16-bit
    ndarray = img_as_uint(ndarray)
    
    # Convert nd Image to ITK image
    itk_image = sitk.GetImageFromArray(ndarray)

    #threshold=sitk.ThresholdImageFilter()
    threshold=sitk.BinaryThresholdImageFilter()
    threshold.SetUpperThreshold(float(upper))
    threshold.SetLowerThreshold(float(lower))
    
    
    segmented_img=threshold.Execute(itk_image)

    # Threshold and Invert
    return sitk.GetArrayFromImage(sitk.InvertIntensity(segmented_img, 1))



def threshold_adaptive(ndarray, method, blocksize=5, offset=0):
    
    #Inizialize
    method_list = ['Mean', 'Gaussian']
    
    if method not in method_list:
        raise Exception('Mode has to be amond the following:\n'+str(method_list))
    
    blocksize=round_up_to_odd(blocksize)
    
    #Cast to 8-bit
    converted_image = img_as_ubyte(ndarray)

    #Cycle through image
    outputImage = []
    for i in range(len(converted_image)):
        
        if method == 'Mean':
            outputImage.append(threshold_local(converted_image[i], blocksize, offset))

        elif method == 'Gaussian':
            outputImage.append(cv2.adaptiveThreshold(converted_image[i], 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                                cv2.THRESH_BINARY, blocksize, offset))
        else:
            raise LookupError('Not a valid method!')

    return np.asarray(outputImage)


