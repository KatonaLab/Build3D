import cv2
import numpy as np
import SimpleITK as sitk
from skimage import img_as_ubyte
from skimage.filters import threshold_otsu, threshold_yen, threshold_triangle, threshold_sauvola, threshold_niblack, threshold_minimum, threshold_mean, threshold_li, threshold_isodata

#from .utils import round_up_to_odd, convert_array_type
try:
    from utils import round_up_to_odd, convert_array_type
    from external.max_entropy import max_entropy
except:
    from modules.packages.a3dc.utils import  round_up_to_odd, convert_array_type
    from modules.packages.a3dc.external.max_entropy import max_entropy

    

#TODO:-unittest for 'Sauvola','Niblack' method in threshold_adaptive
#     -unittest for threshold_auto_skimage

def tag_image(ndarray):

    #Cast to 16-bit
    #ndarray = convert_array_type(ndarray, 'int16')

    #Convert ndarray to itk image
    itk_image = sitk.GetImageFromArray(ndarray)

    #Cast images to 8-bit as images should be binary anyway
    #itk_image=sitk.Cast(itk_image, sitk.sitkUInt)

    # Run ITK Connectedcomponents. Numpy array has to be converted to ITK image format.
    #output_image = sitk.ConnectedComponent(itk_image, fullyConnected=True) had to be changed.
    #https://discourse.itk.org/t/sitk-connectedcomponent/876%20Ziv%20Yaniv
    #Due to q problem/feature of SWIG and needs to be addressed there.
    #When we overload a function, named arguments don’t work. The ConnectedComponent 81 function has two variants now.
    #The solution in your case is to provide all arguments based on location (use the default values for those you didn’t provided in the past).
    output_image = sitk.ConnectedComponent(itk_image, True)
    

    return sitk.GetArrayFromImage(output_image)
 
def overlap_image(array_list):

    # Create Overlapping ImagE
    output_array = array_list[0]> 0
    for i in range(1, len(array_list)):
        output_array = np.multiply(output_array, array_list[i]>0)
        
    return output_array.astype(np.uint8)


def threshold_auto(ndarray, method, mode='Slice'):
    '''The first dimension of the array has to start with z (shape of (z,x,y) or (z,y,x)) 
    '''
    # Cast to 16-bit
    #ndarray = convert_array_type(ndarray, 'int16')
    
    #Initializaton
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
    
    #Check if method and mode is valid
    if method not in threshold_dict.keys():
        raise Exception('Method has to be amond the following:\n'+str(threshold_dict.keys))
    if mode not in mode_list:
        raise Exception('Mode has to be amond the following:\n'+str(mode_list))
    

    #Create ITK FIlter object
    threshold_filter=threshold_dict[method]
    
    #Suppress SimpleITK warnings
    threshold_filter.GlobalWarningDisplayOff()
    
    if mode=="Stack" or len(ndarray.shape)<3 :
        # Create ITK image
        itk_image = sitk.GetImageFromArray(ndarray)
        
        # Apply threshold, invert and convert to nd array
        try:
            output=sitk.GetArrayFromImage(sitk.InvertIntensity(threshold_filter.Execute(itk_image), 1))
            #Get threshold value
            threshold_val = threshold_filter.GetThreshold() 
        except RuntimeError:
            threshold_val=0
            output=ndarray>0
            
        
    elif mode=="Slice":

        try:
            #Remember taht the first dimension axis is taken as 
            threshold_val=[]
          
            output=np.zeros(ndarray.shape)
            for i in range(ndarray.shape[2]):
                
                # Create ITK image
                itk_image = sitk.GetImageFromArray(ndarray[:,:,i])
                
                # Apply threshold, invert and convert to nd array
                segmented_img = sitk.GetArrayFromImage(sitk.InvertIntensity(threshold_filter.Execute(itk_image), 1))
                
                #Append threshold value to threshodValue
                threshold_val.append(threshold_filter.GetThreshold())
                
                #Append slice to outputImage
                output[:,:,i]=segmented_img
            
            output=np.asarray(output)
           
        except RuntimeError:
            threshold_val=[0]*ndarray.shape[2]
            output=ndarray>0

    
    else:
        raise LookupError("'"+str(mode)+"' is Not a valid mode!")

    return convert_array_type(output, 'int8'), threshold_val

def threshold_auto_skimage(ndarray, method, mode='Slice'):
    '''The first dimension of the array has to start with z (shape of (z,x,y) or (z,y,x)) 
    '''
    # Cast to 16-bit
    #ndarray = convert_array_type(ndarray, 'int16')

    #Initializaton
    threshold_dict = {'IsoData': threshold_isodata, 'Otsu': threshold_otsu,
                            'Li': threshold_li,'Yen': threshold_yen,
                            'Triangle': threshold_triangle, 
                            'MaxEntropy':max_entropy}
    
        #'Mean': threshold_mean(),'Minimum': threshold_minimum()

    mode_list=['Stack','Slice']

    #Check if method and mode is valid
    if method not in threshold_dict.keys():
        raise Exception('Method has to be among the following:\n'+str(threshold_dict.keys()))
    if mode not in mode_list:
        raise Exception('Mode has to be among the following:\n'+str(mode_list))
    
    #Get thresholding method
    threshold_filter=threshold_dict[method]

    if mode=="Stack" or len(ndarray.shape)<3 :
  
        #Get threshold value and apply threshold
        if method!='MaxEntropy': 
            try:
        
                threshold_val = threshold_filter(ndarray)
                output = ndarray > threshold_val
                
            except:
                threshold_val=0
                output=ndarray>0
        else:
            #Remember taht the first dimension axis is taken as z
            threshold_list=[]
          
            output=np.zeros(ndarray.shape)
            for i in range(ndarray.shape[2]):
                
                #Get threshold value and apply threshold, append slice to outputImage
                threshold = threshold_filter(ndarray[:,:,i])
                output[:,:,i] = ndarray[:,:,i] > threshold
                              
                #Append threshold value to threshodValue
                threshold_list.append(threshold)

            threshold_val=np.mean(threshold_list)
            
            
        
    elif mode=="Slice":

        try:
            #Remember taht the first dimension axis is taken as z
            threshold_val=[]
          
            output=np.zeros(ndarray.shape)
            for i in range(ndarray.shape[2]):
                
                #Get threshold value and apply threshold, append slice to outputImage
                threshold = threshold_filter(ndarray[:,:,i])
                output[:,:,i] = ndarray[:,:,i] > threshold
                              
                #Append threshold value to threshodValue
                threshold_val.append(threshold)

            
            output=np.asarray(output)
           
        except RuntimeError:
            threshold_val=[0]*ndarray.shape[2]
            output=ndarray>0
    
    else:
        raise LookupError("'"+str(mode)+"' is Not a valid mode!")

    return convert_array_type(output, 'int8'), threshold_val


def threshold_manual(ndarray, upper=1, lower=0):
 
    # Cast to 16-bit
    #ndarray = convert_array_type(ndarray, 'int16')
    
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
    
    # Cast to 16-bit
    #ndarray = convert_array_type(ndarray, 'int16')
    
    #Inizialize
    method_list = ['Mean', 'Gaussian', 'Sauvola','Niblack']
    
    if method not in method_list:
        raise Exception('Mode has to be amond the following:\n'+str(method_list))
    
    blocksize=round_up_to_odd(blocksize)
    
    
    #For 2D images array needs to be reshaped to run properly through next cycle
    if len(ndarray.shape)<3:
        converted_image=[img_as_ubyte(ndarray)]
    else:
        converted_image=img_as_ubyte(ndarray)

    #Cycle through image
    outputImage = [] 
    for i in range(len(converted_image)):
    
        if method == 'Mean':
            outputImage.append(cv2.adaptiveThreshold(converted_image[i], 255, cv2.ADAPTIVE_THRESH_MEAN_C,
                                                cv2.THRESH_BINARY, blocksize, offset))

        elif method == 'Gaussian':  
            outputImage.append(cv2.adaptiveThreshold(converted_image[i], 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                                cv2.THRESH_BINARY, blocksize, offset))
        elif  method =='Sauvola':
            outputImage.append(threshold_sauvola(converted_image[i]))

        elif  method =='Niblack':
             outputImage.append(threshold_niblack(converted_image[i]))                          
        
        else:
            raise LookupError('Not a valid method!')
    
    #Remove singleton dimension (eg. if image was 2D)
    outputImage=np.squeeze(np.array(outputImage)>0).astype(np.uint8)
        
    return convert_array_type(outputImage, 'int8')



 

