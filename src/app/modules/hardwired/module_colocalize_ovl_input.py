import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.interface import tagImage, analyze, apply_filter, colocalization, save_data, save_image
from modules.a3dc_modules.a3dc.utils import os_open, quote

import os
import numpy as np


FILTERS_CH = ['volume', 'voxelCount',
           'pixelsOnBorder', 'meanIntensity',
           'variance']
FILTERS_OVL = ['volume', 'voxelCount',
           'pixelsOnBorder', 'colocalizationCount',
           'overlappingRatio', 'totalOverlappingRatio']
####################################################Interface to call from C++####################################################
def colocalize(ch1Array, ch1ThrArray, ch1Metadata, ch1Settings,  ch2Array, ch2ThrArray, ch2Metadata, ch2Settings, ovlSettings, show=True, to_text=False):
   
        #Parameters to measure
        measurementList = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder', 'meanIntensity', 'variance']
        
        #TEMP###########TEMP##############TEMP#################TEMP
        multi_img_keys = ['meanIntensity','medianIntensity', 'skewness', 'kurtosis', 'variance','maximumPixel',
                               'maximumValue', 'minimumValue','minimumPixel','centerOfMass','standardDeviation',
                               'cumulativeIntensity','getWeightedElongation','getWeightedFlatness','getWeightedPrincipalAxes',
                               'getWeightedPrincipalMoments']
        
        for key in ch1Settings:
            if key in multi_img_keys:
                ch1Settings[str(key)+' in '+str(ch1Metadata['Name'])] = ch1Settings[key]
                del ch1Settings[key]

        for key in ch2Settings:
            if key in multi_img_keys:
                ch2Settings[str(key)+' in '+str(ch2Metadata['Name'])] = ch2Settings[key]
                del ch2Settings[key]            
        
        ch1Img=Image(ch1Array, ch1Metadata)
        ch2Img=Image(ch2Array, ch2Metadata)
        thresholdedImage1=Image(ch1ThrArray, ch1Metadata)
        thresholdedImage2=Image(ch2ThrArray, ch2Metadata)
        
        #TEMP###########TEMP##############TEMP#################TEMP
        outputPath=ch1Metadata['Path']+'/Output'
        if not os.path.exists(outputPath):
            os.makedirs(outputPath)
        
        #############################################################################################################################
        ###########################################Segmentation and Analysis#########################################################
        #############################################################################################################################

        #######################################################Channel 1#############################################################
        #Channel 1:Tagging Image
        taggedImage1, logText = tagImage(thresholdedImage1)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage1, logText = analyze(taggedImage1, imageList=[ch1Img], measurementInput=measurementList)
        print(logText)
        
        taggedImage1, logText = apply_filter(taggedImage1, filterDict=ch1Settings, removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        


        #######################################################Channel 2#############################################################
        #Channel 1:Tagging Image
        taggedImage2, logText = tagImage(thresholdedImage2)

        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage2, logText = analyze(taggedImage2, imageList=[ch2Img], measurementInput=measurementList)
        print(logText)

        
        taggedImage2, logText = apply_filter(taggedImage2, filterDict=ch2Settings, removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        
        #############################################################################################################################
        ###################################################Colocalization############################################################
        #############################################################################################################################
        overlappingImage, taggedImageList, logText = colocalization( [taggedImage1, taggedImage2], overlappingFilter=ovlSettings, removeFiltered=False)
        print(logText)
        
        name = ch1Img.metadata['Name']+"_tagged"
        save_image(taggedImage1, outputPath, name)
        
        name = ch2Img.metadata['Name']+"_tagged"
        save_image(taggedImage2, outputPath, name)
        
        name = ch1Img.metadata['Name']+ "_" +ch2Img.metadata['Name']+ "_overlap"
        save_image(overlappingImage, outputPath, name)
        
        logText='\nSaving object dataBases to xlsx or text!'
        print(logText)

        #Save File
        name=ch1Img.metadata['Name']+'_'+ch2Img.metadata['Name']
        if to_text==True:
            file_name=name+'.txt'    
        else:
            file_name=name+'.xlsx'
        save_data([taggedImage1,taggedImage2, overlappingImage], path=outputPath, file_name=file_name, to_text=to_text)

        
        #Show file
        #if show==True:
            
            #os_open(os.path.join(outputPath, file_name))
        print('Colocalization analysis was run successfully!')
        print("\n%s\n" % str(quote()))
        return overlappingImage
    
    
def module_main(_):
    params = {'ch1': read_params('ch1', FILTERS_CH),
              'ch2': read_params('ch2', FILTERS_CH),
              'ovl': read_params('ovl', FILTERS_CH_OVL, use_images=False)}

    output=colocalize(params['ch1']['intensity'],
               params['ch1']['mask'],
               params['ch1']['intensity_meta'],
               params['ch1']['settings'],
               params['ch2']['intensity'],
               params['ch2']['mask'],
               params['ch2']['intensity_meta'],
               params['ch2']['settings'],
               params['ovl']['settings'])

    a3.outputs['Ovl_Image'] = a3.MultiDimImageFloat_from_ndarray(output.array.astype(np.float) / np.amax(output.array.astype(np.float)))
    a3.outputs['Ovl_db']=output.database

def add_input_fields(name, filters, use_images=True):
    config = []
    if use_images:
        config = [
            a3.Input('{} intensity'.format(name), a3.types.ImageFloat),
            a3.Input('{} mask'.format(name), a3.types.ImageFloat)]

    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {} {}'.format(name, f, m), a3.types.float)
                .setIntHint('min', 0)
                .setIntHint('max', 10000000)
                .setIntHint('default', 0 if m == 'min' else 10000000))
    return config


def add_input_fields_ovl(name, filters, use_images=True):
    config = []
    if use_images:
        config = [
            a3.Input('{} intensity'.format(name), a3.types.ImageFloat),
            a3.Input('{} mask'.format(name), a3.types.ImageFloat)]

    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {} {}'.format(name, f, m), a3.types.float)
                .setIntHint('min', 0)
                .setIntHint('max', 10000000)
                .setIntHint('default', 0 if m == 'min' else 10000000))
    return config


def read_params(name, filters, use_images=True):
    out_dict = {}
    if use_images:
        intensity_field = '{} intensity'.format(name)
        mask_field = '{} mask'.format(name)

        intensity_im = a3.MultiDimImageFloat_to_ndarray(
            a3.inputs[intensity_field])
        mask_im = a3.MultiDimImageFloat_to_ndarray(a3.inputs[mask_field])

        intensity_meta = {
            'Name': name + '_intensity',
            'Path': '.',
            'Type': np.float}

        mask_meta = {
            'Name': name + '_mask',
            'Path': '.',
            'Type': np.float}

        out_dict = {'intensity': intensity_im,
                    'intensity_meta': intensity_meta,
                    'mask': mask_im,
                    'mask_meta': mask_meta}

    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {} {}'.format(name, f, m)]

    out_dict['settings'] = settings
    return out_dict


config = [a3.Output('Ovl_Image', a3.types.ImageFloat),  a3.Output('Ovl_db', a3.types.GeneralPyType)]
config.extend(add_input_fields('ch1', FILTERS_CH))
config.extend(add_input_fields('ch2', FILTERS_CH))
config.extend(add_input_fields('ovl',FILTERS_OVL, use_images=False))

a3.def_process_module(config, module_main)
