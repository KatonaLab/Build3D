# -*- coding: utf-8 -*-
"""
Created on Sun Mar 25 22:51:43 2018

@author: Nerberus
"""

###############################################Class that contain main functions for A3DC####################################################



import SimpleITK as sitk
import numpy as np





def smoothingGaussianFilter(image, sigma):

    itk_img = sitk.GetImageFromArray(image)
    pixel_type = itk_img.GetPixelID()

    sGaussian = sitk.SmoothingRecursiveGaussianImageFilter()
    sGaussian.SetSigma(float(sigma))
    itk_img = sGaussian.Execute(itk_img)

    caster = sitk.CastImageFilter()
    caster.SetOutputpixel_type(pixel_type)

    return sitk.GetArrayFromImage(caster.Execute(itk_img))


def discreteGaussianFilter(image, sigma):
    itk_img = sitk.GetImageFromArray(image)
    pixel_type = itk_img.GetPixelID()

    dGaussian = sitk.DiscreteGaussianImageFilter ()
    dGaussian.SetVariance(int(sigma))
    dGaussian.SetUseImageSpacing(False)
    itk_img = dGaussian.Execute(itk_img)

    caster = sitk.CastImageFilter(itk_img)
    caster.SetOutputpixel_type(pixel_type)

    return sitk.GetArrayFromImage(caster.Execute(itk_img))


def medianFilter(image, radius):
    itk_img = sitk.GetImageFromArray(image)
    pixel_type = itk_img.GetPixelID()

    median = sitk.MedianImageFilter()
    median.SetRadius(int(radius))
    itk_img = median.Execute(itk_img)

    caster = sitk.CastImageFilter(itk_img)
    caster.SetOutputpixel_type(pixel_type)

    return sitk.GetArrayFromImage(caster.Execute(itk_img))


def regionGrowingSegmentation(image, seedList, initialNeighborhoodRadius=2, multiplier=2.5, NbrOfIterations=5,
                              replaceValue=255):
    itk_img = sitk.GetImageFromArray(image)


    filter = sitk.ConfidenceConnectedImageFilter()
    filter.SetSeedList(seedList)
    filter.SetMultiplier(multiplier)
    filter.SetNumberOfIterations(NbrOfIterations)
    filter.SetReplaceValue(replaceValue)
    filter.SetInitialNeighborhoodRadius(initialNeighborhoodRadius)
    itk_img = filter.Execute(itk_img)


    return sitk.GetArrayFromImage(itk_img)

 



