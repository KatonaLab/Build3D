import time
import os

import a3dc

class main(object):

    def __init__(self):
        ####################################################Input####################################################
        tstart = time.clock()
        
        #Path for source files
        path = 'D:\\Playground\\refactoring'
        
        
        channelName1='vGlut'
        
        thresholdMeth1 = 'Manual'
        thresholdVal1=16
        filterDict1 = {'voxelCount': {'min': 27, 'max': 3125}}
        fileName1 = 'A15_1_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ome_channel_3_crop.tif'
       

        # InputParameters Channel 2
        channelName2='Bassoon'
        thresholdMeth2 = 'Manual'
        thresholdVal2=7
        filterDict2 = {'voxelCount': {'min': 4, 'max': 4000}}
        fileName2 = 'A15_1_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ome_channel_4_crop.tif'
        


        # Input Parameters Overlapping
        filterDictOvl = {'voxelCount': {'min': 4, 'max': 4000}}


        #Parameters to measure
        measurementList = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder', 'meanIntensity', 'variance']

        
        
        if thresholdMeth1!='Manual':
            thresholdVal1=None
        
        if thresholdMeth2!='Manual':
            thresholdVal2=None
       
        #Creata Output directory
        outputPath=os.path.join(path, 'Output')
        if not os.path.exists(outputPath):
            os.makedirs(outputPath)
            
        
        #Loand ch1 image and create property and settings  dictionary
        #ch1Img = io.load_image(os.path.join(path, fileName1))

        ch1FileName, _ = os.path.splitext(os.path.basename(fileName1))
        ch1Dict = {'FileName': ch1FileName,'Name':channelName1}
        ch1Img=a3dc.Image.load_image(os.path.join(path, fileName1), metadata=ch1Dict)
        ch1Settings={'filter':filterDict1, 'method':thresholdMeth1, 'thresholdVal':thresholdVal1, 'measurements':measurementList,'mode':"Stack"}

        #Loand ch2 image and createproperty and settings  dictionary
        ch2FileName, _ = os.path.splitext(os.path.basename(fileName2))
        ch2Dict = {'FileName': ch2FileName, 'Name':channelName2}
        ch2Img=a3dc.Image.load_image(os.path.join(path, fileName2), metadata=ch2Dict)
        ch2Settings={'filter':filterDict2, 'method':thresholdMeth2, 'thresholdVal':thresholdVal2, 'measurements':measurementList,'mode':"Stack"}
        
        #reate overlapping settings  dictionary
        ovlSettings={'filter':filterDictOvl }
        
        main.colocalize(ch1Img, ch1Settings, ch2Img, ch2Settings, ovlSettings, outputPath)

        tstop = time.clock()
        

        print('Total processing time was ' + str((tstop - tstart)) + ' seconds!')


               
    def colocalize(ch1Img, ch1Settings,  ch2Img, ch2Settings, ovlSettings, outputPath):
        #(ch1Img, ch2Img, ch1Dict, ch2Dict , threshMethod1, thresholdVal1, threshMethod2, thresholdVal2, filterDict1, filterDict2, filterDictOvl, outputPath, fileName, measurementList):
        #############################################################################################################################
        ######################################################Initialization#########################################################
        #############################################################################################################################
        #Inizialize log string and start measuring time

 
        #############################################################################################################################
        ###########################################Segmentation and Analysis#########################################################
        #############################################################################################################################

        #######################################################Channel 1#############################################################
        #Channel 1:Thresholding
        thresholdedImage1, logText = a3dc.threshold(ch1Img, method=ch1Settings['method'], mode=ch1Settings['mode'], lowerThreshold=0, upperThreshold=ch1Settings['thresholdVal'])
        print(logText)

        #Channel 1:Tagging Image
        taggedImage1, logText = a3dc.tagImage(thresholdedImage1)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage1, logText = a3dc.analyze(taggedImage1, imageList=[ch1Img], measurementInput=ch1Settings['measurements'])
        print(logText)
        
        taggedImage1, logText = a3dc.apply_filter(taggedImage1, filterDict=ch1Settings['filter'], removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        
        # Channel 1:Saving Intermediate Images
        suffix=ch1Img.metadata['FileName']+ "_" + ch1Settings['method'] +'_'+str(ch1Settings['thresholdVal'])
        a3dc.save_image(thresholdedImage1, outputPath, suffix )

        #######################################################Channel 2#############################################################
        #Channel 1:Thresholding
        thresholdedImage2, logText = a3dc.threshold(ch2Img, method=ch2Settings['method'], mode=ch2Settings['mode'], lowerThreshold=0, upperThreshold=ch2Settings['thresholdVal'])
        print(logText)

        #Channel 1:Tagging Image
        taggedImage2, logText = a3dc.tagImage(thresholdedImage2)
        print(logText)

        # Channel 1:Analysis and Filtering of objects
        taggedImage2, logText = a3dc.analyze(taggedImage2, imageList=[ch2Img], measurementInput=ch2Settings['measurements'])
        print(logText)

        
        taggedImage2, logText = a3dc.apply_filter(taggedImage2, filterDict=ch2Settings['filter'], removeFiltered=False)#{'tag':{'min': 2, 'max': 40}}
        print(logText)
        
        # Channel 1:Saving Intermediate Images
        suffix=ch2Img.metadata['FileName']+ "_" + ch2Settings['method'] +'_'+str(ch2Settings['thresholdVal'])
        a3dc.save_image(thresholdedImage2, outputPath, suffix)
        

        #############################################################################################################################
        ###################################################Colocalization############################################################
        #############################################################################################################################
        

        #Colocalization:
        overlappingImage, taggedImageList, logText = a3dc.colocalization( [taggedImage1, taggedImage2], overlappingFilter=ovlSettings['filter'], removeFiltered=False)
        print(logText)

        suffix = ch1Img.metadata['Name']+ "_" +ch2Img.metadata['Name']+ "_overlap"
        a3dc.save_image(overlappingImage, outputPath, suffix)
        
        logText='\nSaving object dataBases to xlsx and text!'
        print(logText)

        
        name1=ch1Img.metadata['Name']+'_'+ch1Settings['method']+"_th1_"+str(ch1Settings['thresholdVal'])
        name2=ch2Img.metadata['Name']+'_'+ch2Settings['method']+'_th2_'+str(ch2Settings['thresholdVal'])
        name=name1+'_'+name2
        
        a3dc.save_data([taggedImage1,taggedImage2, overlappingImage], path=outputPath, fileName=name+'.xlsx', toText=False)
        a3dc.save_data([taggedImage1], path=outputPath, fileName=name1+'.txt', toText=True)
        a3dc.save_data([taggedImage1], path=outputPath, fileName=name2+'.txt', toText=True)
        


if __name__ == '__main__':

    a = main()
