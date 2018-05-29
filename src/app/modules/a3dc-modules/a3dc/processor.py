# -*- coding: utf-8 -*-
"""
Created on Sun Mar 25 22:51:43 2018

@author: Nerberus
"""

###############################################Class that contain main functions for A3DC####################################################



import SimpleITK as sitk
import numpy as np


def multiply(taggedImgList, sourceImageList=None):


    # Create Overlapping Image
    output_array = taggedImgList[0].array
    for i in range(1, len(taggedImgList)):
        output_array = np.multiply(output_array, taggedImgList[i].array)

    return output_array




def smoothingGaussianFilter(image, sigma):

    itkImage = sitk.GetImageFromArray(image)
    pixelType = itkImage.GetPixelID()

    sGaussian = sitk.SmoothingRecursiveGaussianImageFilter()
    sGaussian.SetSigma(float(sigma))
    itkImage = sGaussian.Execute(itkImage)

    caster = sitk.CastImageFilter()
    caster.SetOutputPixelType(pixelType)

    return sitk.GetArrayFromImage(caster.Execute(itkImage))


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


def medianFilter(image, radius):
    itkImage = sitk.GetImageFromArray(image)
    pixelType = itkImage.GetPixelID()

    median = sitk.MedianImageFilter()
    median.SetRadius(int(radius))
    itkImage = median.Execute(itkImage)

    caster = sitk.CastImageFilter(itkImage)
    caster.SetOutputPixelType(pixelType)

    return sitk.GetArrayFromImage(caster.Execute(itkImage))


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

 



