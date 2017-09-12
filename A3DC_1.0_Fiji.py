from __future__ import with_statement, division
import traceback
import time
from random import randint

from jarray import array, zeros
from java.lang import Double, Boolean, Long

from sys import exit
from os import system #path#,
from os.path import splitext, join, isfile,exists,dirname,basename
from os import listdir,makedirs,remove
from gc import collect


from ij import IJ, ImagePlus, ImageStack, WindowManager
from ij.io import DirectoryChooser, OpenDialog, SaveDialog
from ij.gui import GenericDialog, DialogListener, NonBlockingGenericDialog
from ij.measure import ResultsTable
from ij.plugin import ImageCalculator
from ij.process import FloatProcessor,ShortProcessor


class CoExpressGUI(object):

	'''The CoExpressGui Class is the main class used in A3DC. It is used to create the GUI/s to read data, loads images and contains 
	the workflows to process images.
	'''
	def __init__(self):
		self.extensions=['.nd2','.lsm','.zvi','.lms','.tiff', '.tif', '.oib', '.oif',	'.scn',	'.lif','.ics']
		self.segment_GUI()
	
	def segment_GUI(self):
		'''Creates the first GUI shown when running A3DC to read in segmentation parameters and set the processing mode.
    	'''
		#Generate list of channel numbers
		channelList=[str(x) for x in range(1,11)]
		
		#Get ImagePlus: if images are open get active image, otherwise open new
		self.segmentDialog=NonBlockingGenericDialog('Segmentation Settings')
		self.segmentDialog.enableYesNoCancel('Run','Re-analyze')
		self.segmentDialog.setOKLabel('Process images')
		
		#Channel 1 Segmentation input
		self.segmentDialog.setInsets(0,0,0)
   		self.segmentDialog.addMessage('Channel 1:')
		self.segmentDialog.addChoice("Channel number: ", channelList, channelList[0])
		autoThresholdList= ('Manual','Otsu','MaxEntropy','Default','Huang','IsoData','Intermodes','Li','Minimum','Moments','Mean','Triangle','Yen','Percentile','RenyiEntropy','Shanbhag',"MinError(I)")
		self.segmentDialog.addChoice("Thresholding method: ", autoThresholdList, autoThresholdList[0])
		self.segmentDialog.addNumericField("Manual Threshold", 1, 2)

		#Channel 2 Segmentation input 
		self.segmentDialog.setInsets(0,0,0)
   		self.segmentDialog.addMessage('Channel 2:')
		self.segmentDialog.addChoice("Channel number: ", channelList, str(channelList[1]))
		self.segmentDialog.addChoice("Thresholding method: ", autoThresholdList, autoThresholdList[0])
		self.segmentDialog.addNumericField("                                                   Manual Threshold", 1, 2) 
		
		self.segmentDialog.addCheckboxGroup(2,2,["Save intermediate Images?","Manually set output directory?","batch run?",],[True,False,False])

		self.segmentDialog.showDialog()
	
		if self.segmentDialog.wasCanceled():
			pass
		
		elif self.segmentDialog.wasOKed():
			self.mode='image'
			try:
				self.set_segmentation_parameters()
				self.opener()
				self.analyze_GUI()
			except Exception, e:
				IJ.log('')
				IJ.log('##########################################################################################################################') 
				IJ.log(str(Exception)+': '+str(e))
				IJ.log('') 
				IJ.log(str(traceback.format_exc()))
				IJ.log('##########################################################################################################################') 
				IJ.log(CoExpressProcessor.quote())
				IJ.log('##########################################################################################################################')
			
			
		else:
			self.mode='reanalyze'
			self.set_segmentation_parameters()
			self.analyze_GUI()
	
	def set_segmentation_parameters(self):
		'''Reads in the segmentation parameters from the segmentation GUI and puts them in instance data members. 
		In 'Image' mode segmentation parameters ar put in dictionaries to be used later.
    	'''
		if self.mode=='image':
			
			#Channel 1 Segmentation parameters
			channel_ch1=int(self.segmentDialog.getNextChoice())
			autoThreshold_ch1=self.segmentDialog.getNextChoice()
			thresholdVal_ch1=int(self.segmentDialog.getNextNumber())
			self.dictSeg_ch1={'channel':channel_ch1 ,'autothreshold':autoThreshold_ch1 ,'thresholdValue':thresholdVal_ch1}
		
			#Channel 2 Segmentation parameters
			channel_ch2=int(self.segmentDialog.getNextChoice())
			autoThreshold_ch2=self.segmentDialog.getNextChoice()
			thresholdVal_ch2=int(self.segmentDialog.getNextNumber())
			self.dictSeg_ch2={'channel':channel_ch2 ,'autothreshold':autoThreshold_ch2 ,'thresholdValue':thresholdVal_ch2}

		self.saveImageChoice=self.segmentDialog.getNextBoolean()
		self.setOutputChoice=self.segmentDialog.getNextBoolean()
		self.batchChoice=self.segmentDialog.getNextBoolean()
		
		
					
	def analyze_GUI(self):
	
		'''Creates the second GUI shown in A3DC. The function of this gui is to read in the filter settings for the analysis step. 
		The only exception is the size filter that is used in the image processing step (see the manual). The buttons of the 
		self.analyzeDialog general dialog will start the processing step in a mode dependent manner.If the processing mode is 
		‘image’ parameters are read from the GUI and the self.run_image method is started. If the processing mode is ‘reanalyze’
		the self.run_intermediate method is run after reading the input parameters. The analysis parameters are stored in instance variables.
		'''
		self.analyzeDialog=GenericDialog('Analysis Settings')
		
		#super(CoExpressGUI, self).__init__("Ace3D")
		self.analyzeDialog.setInsets(0,0,0)
   		self.analyzeDialog.addMessage('Channel 1:')
   		
		#Channel 1 Filters
		self.analyzeDialog.addStringField("Min Object Volume", 'min', 10) 
		self.analyzeDialog.addStringField("Max Object Volume", 'max', 10)
		
		self.analyzeDialog.addStringField("Min Intensity", 'min', 10) 
		self.analyzeDialog.addStringField("Max Intensity", 'max', 10)

		self.analyzeDialog.addStringField("Min Total Overlapping Ratio", 'min', 10) 
		self.analyzeDialog.addStringField("Max Total Overlapping Ratio", 'max', 10)

		self.analyzeDialog.setInsets(0,100,0)
		self.analyzeDialog.addCheckbox('Remove objects on the edge?', False)
		self.analyzeDialog.addMessage('')

		#Channel 2 Filters
		self.analyzeDialog.setInsets(0,0,0)
   		self.analyzeDialog.addMessage('Channel 2:')
   		
		self.analyzeDialog.addStringField("Min Object Volume", 'min', 10) 
		self.analyzeDialog.addStringField("Max Object Volume", 'max', 10)
		
		self.analyzeDialog.addStringField("Min Intensity", 'min', 10) 
		self.analyzeDialog.addStringField("Max Intensity", 'max', 10)

		self.analyzeDialog.addStringField("Min Total Overlapping Ratio", 'min', 10) 
		self.analyzeDialog.addStringField("Max Total Overlapping Ratio", 'max', 10)

		self.analyzeDialog.setInsets(0,100,0)
		self.analyzeDialog.addCheckbox('Remove objects on the edge?', False)
		self.analyzeDialog.addMessage('')
		
		#Overlapping Filters
		self.analyzeDialog.setInsets(0,0,0)
   		self.analyzeDialog.addMessage('Overlapping Objects:')
   		
		self.analyzeDialog.addStringField("Min Volume", 'min', 10) 
		self.analyzeDialog.addStringField("Max Volume", 'max', 10)

		self.analyzeDialog.addStringField("Min Overlapping Ratio in channel 1", 'min', 10) 
		self.analyzeDialog.addStringField("Max Overlapping Ratio in channel 1", 'max', 10)

		self.analyzeDialog.addStringField("Min Overlapping Ratio in channel 2", 'min', 10) 
		self.analyzeDialog.addStringField("Max Overlapping Ratio in channel 2", 'max', 10)

		self.analyzeDialog.centerDialog(True)
		self.analyzeDialog.showDialog()
	
		if self.analyzeDialog.wasCanceled():
			return
		
		if self.analyzeDialog.wasOKed():
			if self.mode=='image':
				self.set_analyze_parameters()
				self.run_image()
			elif self.mode=='reanalyze':
				self.set_analyze_parameters()
				self.run_intermediate()
			
		

	def set_analyze_parameters(self):
		'''
		Reads in the segmentation parameters from the analysis GUI. All parameters are stored in an instance variables: self.dictChFilter_ch1,
		self.dictChFilter_ch2 and self.dictChFilter_ovl. Each filter will have a separate sub-dictionary 'maxVal' and 'maxVal' key value 
		(if no number is set the largest and smallest possible values are set.
		'''
		#Channel 1 Filters
		minVol_ch1=self.analyzeDialog.getNextString()
		maxVol_ch1=self.analyzeDialog.getNextString()
		minIntensity_ch1=self.analyzeDialog.getNextString()
		maxIntensity_ch1=self.analyzeDialog.getNextString()
		minTotOvlRatio_ch1=self.analyzeDialog.getNextString()
		maxTotOvlRatio_ch1=self.analyzeDialog.getNextString()
		isOnEdge_ch1=self.analyzeDialog.getNextBoolean()
		
		#Channel 1 Filters
		minVol_ch2=self.analyzeDialog.getNextString()
		maxVol_ch2=self.analyzeDialog.getNextString()
		minIntensity_ch2=self.analyzeDialog.getNextString()
		maxIntensity_ch2=self.analyzeDialog.getNextString()
		minTotOvlRatio_ch2=self.analyzeDialog.getNextString()
		maxTotOvlRatio_ch2=self.analyzeDialog.getNextString()
		isOnEdge_ch2=self.analyzeDialog.getNextBoolean()
	

		#Overlapping Object filters
		minVol_ovl=self.analyzeDialog.getNextString()
		maxVol_ovl=self.analyzeDialog.getNextString()
		minOvlRatioCh1_ovl=self.analyzeDialog.getNextString()
		maxOvlRatioCh1_ovl=self.analyzeDialog.getNextString()
		minOvlRatioCh2_ovl=self.analyzeDialog.getNextString()
		maxOvlRatioCh2_ovl=self.analyzeDialog.getNextString()
		
		#Channel 1 Filter dictionary
		self.dictChFilter_ch1={}
		if isOnEdge_ch1==True:
			self.dictChFilter_ch1['isOnEdge']={'maxVal':False, 'minVal':False}
		else:
			self.dictChFilter_ch1['isOnEdge']={'maxVal':True, 'minVal':False}

		if minVol_ch1!='min' or maxVol_ch1!='max':
			minMax={}
			try:
				minMax['minVal']=int(minVol_ch1)
			except ValueError:
				pass	
			try: 
				minMax['maxVal']=int(maxVol_ch1)
			except ValueError:
				pass
			self.dictChFilter_ch1['volume']=minMax
		
		if minIntensity_ch1!='min' or maxIntensity_ch1!='max':
			minMax={}
			try:
				minMax['minVal']=int(minIntensity_ch1)
			except ValueError:
				pass
			try:
				minMax['maxVal']=int(maxIntensity_ch1)
			except ValueError:
				pass
			self.dictChFilter_ch1['meanIntensity']=minMax

		if minTotOvlRatio_ch1!='min' or maxTotOvlRatio_ch1!='max':
			minMax={}
			try:
				minMax['minVal']=float(minTotOvlRatio_ch1)
			except ValueError:
				pass
			try:
				minMax['maxVal']=float(maxTotOvlRatio_ch1)
			except ValueError:
				pass
			self.dictChFilter_ch1['totalOvlRatio']=minMax	
		
		if minTotOvlRatio_ch1=='min' or float(minTotOvlRatio_ch1)<=0 :
			if 'totalOvlRatio' in minTotOvlRatio_ch1:
				self.dictChFilter_ch1['totalOvlRatio']['minVal']=0.000000000000000000000000000000000000000000000000000000000000001
			else:
  				minMax={}
				minMax['minVal']=0.000000000000000000000000000000000000000000000000000000000000001
				self.dictChFilter_ch1['totalOvlRatio']=minMax
				
		#Channel 2 Filter dictionary
		self.dictChFilter_ch2={}
		
		if isOnEdge_ch2==True:
			self.dictChFilter_ch2['isOnEdge']={'maxVal':False, 'minVal':False}
		else:
			self.dictChFilter_ch2['isOnEdge']={'maxVal':True, 'minVal':False}
	
		if minVol_ch2!='min' or maxVol_ch2!='max':
			minMax={}
			try:
				minMax['minVal']=int(minVol_ch2)
			except ValueError:
				pass
			try:
				minMax['maxVal']=int(maxVol_ch2)
			except ValueError:
				pass
			self.dictChFilter_ch2['volume']=minMax

		if minIntensity_ch2!='min' or maxIntensity_ch2!='max':
			minMax={}
			try:
				minMax['minVal']=int(minIntensity_ch2)
			except ValueError:
				pass
			try:
				minMax['maxVal']=int(maxIntensity_ch2)
			except ValueError:
				pass
			self.dictChFilter_ch2['meanIntensity']=minMax

		if minTotOvlRatio_ch2!='min' or maxTotOvlRatio_ch2!='max':
			minMax={}
			try:
				minMax['minVal']=float(minTotOvlRatio_ch2)
			except ValueError:
				pass
			try:
				minMax['maxVal']=float(maxTotOvlRatio_ch2)
			except ValueError:
				pass	
			self.dictChFilter_ch2['totalOvlRatio']=minMax

	
		if minTotOvlRatio_ch2=='min' or float(minTotOvlRatio_ch2)<=0:
			if 'totalOvlRatio' in minTotOvlRatio_ch2:
				self.dictChFilter_ch2['totalOvlRatio']['minVal']=0.000000000000000000000000000000000000000000000000000000000000001
			else:
  				minMax={}
				minMax['minVal']=0.000000000000000000000000000000000000000000000000000000000000001
				self.dictChFilter_ch2['totalOvlRatio']=minMax
				
				
		#Overlapping filter dictionary
		self.dictOvlFilter={}
		if minOvlRatioCh1_ovl!='min' or maxOvlRatioCh1_ovl!='max':
			minMax={}
			try:
				minMax['minVal']=float(minOvlRatioCh1_ovl)
			except ValueError:
				pass
			try:
				minMax['maxVal']=float(maxOvlRatioCh1_ovl)
			except ValueError:
				pass
			self.dictOvlFilter['ovlRatio_ch1']=minMax
			
		if minOvlRatioCh2_ovl!='min' or maxOvlRatioCh2_ovl!='max':
			minMax={}
			try:
				minMax['minVal']=float(minOvlRatioCh2_ovl)
			except ValueError:
				pass
			try:
				minMax['maxVal']=float(maxOvlRatioCh2_ovl)
			except ValueError:
				pass
			self.dictOvlFilter['ovlRatio_ch2']=minMax

		if minVol_ovl!='min' or maxVol_ovl!='max':
			minMax={}
			try:
				minMax['minVal']=int(minVol_ovl)
			except ValueError:
				pass
			try:
				minMax['maxVal']=int(maxVol_ovl)
			except ValueError:
				pass
			self.dictOvlFilter['volume']=minMax	
		

	def run_image(self):

		'''
		Method processes image (or images in batch mode) if the self.mode=’Image’. When not in batch processing mode the first image that has been loaded into 
		self.imp is processed by the self.identification3D method that creates a database of objets. The database created is then processed by the self.analysis3D
		method to get the final filtered database. In batch mode an image list is created from the directory of the first loaded image. 
		'''
		
		try:
			#Set Output directory
			if self.setOutputChoice==True:
				dc = DirectoryChooser("Choose output directory!")
				self.outputDirectory = dc.getDirectory()
			elif self.setOutputChoice==False:
				self.outputDirectory=join(self.inputDirectory, 'Output')

			if not exists(self.outputDirectory):
				makedirs(self.outputDirectory)

			#If
 			if self.batchChoice==False:
				self.identification3D()
				self.analysis3D()
					
			elif self.batchChoice==True:
				print self.inputDirectory
				#Get fileList
 				fileList=CoExpressGUI.get_filelist(self.inputDirectory, self.extensions)
 				
 				#check memory and reoeder filelist
 				#Cycle through fileList
 				for fileName in fileList: 
					
					filePath=join(self.inputDirectory, fileName)
					

					#the [] bracket has to be in place for the plugin to open strings with spaces (otherwise the inputstring is not parsed correctly
					openstr="open=["+join(self.inputDirectory, fileName)+"] color_mode=Default view=Hyperstack stack_order=XYCZT"
					IJ.run("Bio-Formats Importer", openstr)
					self.imp=IJ.getImage()
					self.imp.hide()
					
					self.identification3D()
					self.analysis3D()

					self.imp.close()
			#Print quote		
			IJ.log(CoExpressProcessor.quote())
			IJ.log('##########################################################################################################################')	
				
		except Exception , e:
					self.imp.close()
					IJ.log('') 
					IJ.log('Error occurred while processing images!')
					IJ.log('') 
					IJ.log(str(Exception)+': '+str(e))
					IJ.log('') 
					IJ.log(str(traceback.format_exc()))
					IJ.log('##########################################################################################################################') 

	
	def run_intermediate(self):
	
		'''
		Method processes intermediate files (or files in batch mode) if the self.mode=’reanalyze’. The intermediate files contain a database created previously by the 
		self.identification3D method and saved. When not in batch processing mode the user is prompted with a dialogue to open an image. The database is loaded and 
		processed using the self.analysis3D method to get the final filtered database. In batch mode a file list is created of all the intermediate files from the directory
		of the first file and processed. 
		'''
	
		try:
		
			#Read intermedaite file
			while True:
				op=OpenDialog("Choose raw data file (.imd)...", None)
				filePath=op.getPath()

				fileName=basename(filePath)
				(name, fileExtension) = splitext(filePath)
				self.inputDirectory=dirname(filePath)

				if self.inputDirectory==None:
					break
				elif fileExtension=='.imd':
					break
			
			if self.inputDirectory!=None:
		
				#Set Output directory
				if self.setOutputChoice==True:
					dc = DirectoryChooser("Choose output directory!")
					self.outputDirectory = dc.getDirectory()
				elif self.setOutputChoice==False:
					self.outputDirectory=join(self.inputDirectory, 'Output')

				if not exists(self.outputDirectory):
					makedirs(self.outputDirectory)

				if self.batchChoice==False:
					fileList=[fileName]
					
				elif self.batchChoice==True:
					fileList=CoExpressGUI.get_filelist(self.inputDirectory, extensions=['.imd'])
					
				for fileName in fileList:
					loadedDatabase=CoExpressData.load_from_file(join(self.inputDirectory,fileName))
					self.ch1Data=loadedDatabase[0]
					self.ch2Data=loadedDatabase[1]
					self.ovlData=loadedDatabase[2]
					self.analysis3D()
			#Print quote		
			IJ.log(CoExpressProcessor.quote())
			IJ.log('##########################################################################################################################')
				
		except Exception , e:
			
			IJ.log('') 
			IJ.log('Error occurred while reanalyzing data...')
			IJ.log('') 
			IJ.log(str(Exception)+': '+str(e))
			IJ.log('') 
			IJ.log(str(traceback.format_exc()))
			IJ.log('##########################################################################################################################') 

	
	@staticmethod
	def get_filelist(path, extensions=None):

		'''Get a list of file from a given directory.
		path: path of a directory
		extensions: list of accepted extensions
    	'''
	
		fileList = [f for f in listdir(path) if isfile(join(path, f))]
		
		for i in range(len(fileList)-1,-1,-1):

			if extensions!=None:
				fileName=fileList[i]
				(name, extension) = splitext(fileName)
				if extension not in extensions:
					del fileList[i]

 		return fileList	

 				
	def opener(self, extensionList=None):
	
		'''Gets an imagePlus object. If images are open the active image is returned if it has a path (is saved). If no images are open an open dialog
		is opened to open a file with one of the given extensions.The dialog reopens until the user opens an image with an accepted extension or cancelled.
		path: path of a directory
		extensions: list of accepted extensions
    	'''
    	
		if WindowManager.getImageCount()>0:
			self.imp=IJ.getImage()	
			
			
			#Get image path and check if image has been saved
			win=self.imp.getWindow()
			WindowManager.setCurrentWindow(win)
			self.inputDirectory=IJ.getDirectory("image")

			if self.inputDirectory==None:
				
				raise CoExpressError('Image '+str(self.imp.getTitle())+' has to be saved first!','')
		

		elif WindowManager.getImageCount()==0:
				while True:
					op=OpenDialog("Choose image file...", None)
					path=op.getPath()
					
					if path!=None:
					
						(name, fileExtension) = splitext(path)
						
						if extensionList==None or (extensionList!=None and (fileExtension in extensionlist)):
							#the [] bracket has to be in place for the plugin to open strings with spaces (otherwise the inputstring is not parsed correctly
							openstr="open=["+path+"] color_mode=Default view=Hyperstack stack_order=XYCZT"
							IJ.run("Bio-Formats Importer", openstr)
							self.imp=IJ.getImage()			
							self.inputDirectory=dirname(path)
							break		
					else:
						raise CoExpressError()
						
	@staticmethod
	def segmentation( imp,channelChoice,thresholdChoice,thresholdValue=1):

		impRawChannel=CoExpressProcessor.get_channel(imp, channelChoice, 1)

		if thresholdChoice=='Manual':
				
			impThresholded=CoExpressProcessor.threshold(impRawChannel, thresholdValue)

		else:
			impThresholded=CoExpressProcessor.autothreshold(impRawChannel, thresholdChoice)
					
		return impRawChannel, impThresholded

	def analysis3D(self):
		'''
		This method takes the three objects that are instances CoExpressData class (object databases). The three databases are: self.ch1Data, self.ch2Data
		and self.ovlData. Databases for ch1 and ch2 are augmented with the number of overlapping objects in the other channel, cumulative overlapping ratios
		(if there are multiple overlapping objects the overlapping ratio is summed up) for each object. These pararameters are calculated using self.ovlData
		as it contains the connectivity of objects and using the volumes of the overlapping ratios can be calculated. Databases are filtered according to the
		filter settings stored in self.dictChFilter_ch1, self.dictChFilter_ch2 and self.dictOvlFilter (dictionary created by the self.analyze_GUI). Finally
		the database for the database of ch1 and ch2 objects is saved to a tab delimited text file.

		Input:
			self.ch1Data, self.ch2Data, self.ovlData: database of objects from ch1,ch2 and the overlapping images. Each is an instance of the CoExpressData class.
			self.dictChFilter_ch1/ self.dictChFilter_ch2: dictionaries (created by the self.analyze_GUI method) with filters and the volume filter settings for the 3D object counter plugin.

		Output:
			Final output file with the augmented and filtered database

		'''

		#Check if databases ara available
		if (not hasattr(self, 'ch1Data')): 
			raise CoExpressError('No database for channel 1! Terminating run...','')
			
		if (not hasattr(self, 'ch2Data')):
			raise CoExpressError('No database for channel 2! Terminating run...','')
		
		if (not hasattr(self, 'ovlData')):
			raise CoExpressError('No database for Overlapping objects! Terminating run...','') 
			
		try:

			################################################################################Inizialization#############################################################################################
			#Start measuring time
			globalStartTime = time.time()
			#Inizialize variables
			sourceName=None

			#Check if sources are the same for all input databases
			if self.ch1Data.source==self.ch2Data.source==self.ovlData.source:
				sourceName=self.ch1Data.source
			else:
				raise CoExpressError('Database corrupt: all objects should have the same source: '+self.ch1Data.source+', '+self.ch2Data.source+', '+self.ovlData.source+'!','')		

			#Read variables from database
			channel_ch1=self.ch1Data.channel
			channel_ch2=self.ch2Data.channel

			width=int(self.ch1Data.width)
			height=int(self.ch1Data.height)
			NbSlices=int(self.ch1Data.NbSlices)
			
			NbElements_ch1=int(self.ch1Data.NbElements)
			NbElements_ch2=int(self.ch2Data.NbElements)


			#Print source image properties to log in reprocessing mode
			if self.mode=='reanalyze':
				IJ.log(str(''))
				IJ.log('##########################################################################################################################')
				IJ.log('Reprocessing: '+str(sourceName))
				IJ.log('     Image properties:')
				IJ.log('          Path: '+str(self.ch1Data.path))
				IJ.log('          Width: '+str(width))
				IJ.log('          Height: '+str(height))
				IJ.log('          Number of slices: '+str(self.ch1Data.NbSlices))
				IJ.log('          Number of channels: '+str(self.ch1Data.NbChannels))
				IJ.log('          Number of frames: '+str(self.ch1Data.NbFrames))
				
			
				#Print input parameters to log
				IJ.log(str(''))
				IJ.log('Input parameters: ')
		
				IJ.log('     Channel 1:')
				IJ.log('          Channel No.: '+str(channel_ch1))
				IJ.log('          Thresholding method: '+str(self.ch1Data.autothreshold))
				if self.ch1Data.autothreshold=='Manual':
					IJ.log('          Manual threshold: '+str(self.ch1Data.thresholdValue))	
				for key in self.dictChFilter_ch1.keys():
					text=''
					if 'minVal' in self.dictChFilter_ch1[key].keys():
						text=text+' minimum: '+str(self.dictChFilter_ch1[key]['minVal'])
					if 'maxVal' in self.dictChFilter_ch1[key].keys():
						text=text+' maximum: '+str(self.dictChFilter_ch1[key]['maxVal'])	
					IJ.log('          '+str(key)+': '+text)
		
				IJ.log('     Channel 2:')
				IJ.log('          Channel No.: '+str(channel_ch2))
				IJ.log('          Thresholding method: '+str(self.ch2Data.autothreshold))
				if self.ch2Data.autothreshold=='Manual':
					IJ.log('          Manual threshold: '+str(self.ch2Data.thresholdValue))	
				for key in self.dictChFilter_ch2.keys():
					text=''
					if 'minVal' in self.dictChFilter_ch2[key].keys():
						text=text+' minimum: '+str(self.dictChFilter_ch2[key]['minVal'])
					if 'maxVal' in self.dictChFilter_ch2[key].keys():
						text=text+' maximum: '+str(self.dictChFilter_ch2[key]['maxVal'])
					IJ.log('          '+str(key)+': '+text)
		
				if len(self.dictOvlFilter.keys())>0:
					IJ.log('     Overlapping Objects:')
					for key in self.dictOvlFilter.keys():
						text=''
						if 'minVal' in self.dictOvlFilter[key].keys():
							text=text+' minimum: '+str(self.dictOvlFilter[key]['minVal'])
			
						if 'maxVal' in self.dictOvlFilter[key].keys():
							text=text+' maximum: '+str(self.dictOvlFilter[key]['maxVal'])
			
						IJ.log('          '+str(key)+': '+text)


			#Analyze ovlData (connectivity data)
			IJ.showStatus('Analyzing raw data...')
			IJ.log(str(''))
			IJ.log('Analyzing raw data:')

			#Determine overlapping ratio in the two channels and add to ovlData (connectivity database)
			IJ.showStatus('Measuring overlapping ratio of overlapping objects...')
			IJ.log('           Measuring overlapping ratio of overlapping objects...')			
			overlappingRatio_ch1=[0]*(int(self.ovlData.NbElements))
			overlappingRatio_ch2=[0]*(int(self.ovlData.NbElements))
			
			for i in xrange(0, int(self.ovlData.NbElements)):
		
				volume_ovl=self.ovlData.dataBase['volume'][i]
			
				#Channel1
				tag_ch1=self.ovlData.dataBase['taggs_ch1'][i]
				index_ch1=self.ch1Data.find_value('tag',tag_ch1)
				
				if index_ch1!=None:
					volume_ch1=self.ch1Data.dataBase['volume'][index_ch1]
					overlappingRatio_ch1[i]=float(volume_ovl)/float(volume_ch1)
			
				#Channel2
				tag_ch2=self.ovlData.dataBase['taggs_ch2'][i]
				index_ch2=self.ch2Data.find_value('tag',tag_ch2)

				if index_ch2!=None:
					volume_ch2=self.ch2Data.dataBase['volume'][index_ch2]
					overlappingRatio_ch2[i]=float(volume_ovl)/float(volume_ch2)
		
			self.ovlData.add_dataBase({'ovlRatio_ch1':overlappingRatio_ch1,'ovlRatio_ch2':overlappingRatio_ch2})

			#Filter ovlData (connectivity database)
			IJ.showStatus('Filtering object database...')
			IJ.log('           Filtering object database...')

			for key in self.dictOvlFilter.keys():
				
				if key in self.ovlData.dataBase.keys():
					
					dictFilterValues=self.dictOvlFilter[key]
					
					self.ovlData.filter_dataBase(key,**dictFilterValues)
			
			if ('filtered' in self.ovlData.dataBase.keys() and 'tagList' in self.ovlData.dataBase.keys() ):
				toBeRemoved=[]
				for i in xrange(0,len(self.ovlData.dataBase['filtered'])):
					if self.ovlData.dataBase['filtered'][i]==True:
						toBeRemoved.append(self.ovlData.dataBase['tagList'][i])
				self.ovlData.remove_filtered()
				CoExpressProcessor.remove_tags(objectMap_ovl, toBeRemoved)

			#Filter channel 1 and channel 2 database, based on volume, mean grey intensity
			filterList=['volume','meanIntensity','isOnEdge']
			
			for key in filterList:
				if (key in self.ch1Data.dataBase.keys()) and (key in self.dictChFilter_ch1.keys()):
					
					dictFilterValues=self.dictChFilter_ch1[key]
					self.ch1Data.filter_dataBase(key,**dictFilterValues)
			
			for key in filterList:
				if (key in self.ch2Data.dataBase.keys()) and (key in self.dictChFilter_ch2.keys()):
				
					dictFilterValues=self.dictChFilter_ch2[key]

					self.ch2Data.filter_dataBase(key,**dictFilterValues)
		
			#Analyze ch1Data and ch2Data
			#Determine total overlapping ratio and number of collocalized objects from connectivity data
			totalOverlappingRatio_ch1=[0]*NbElements_ch1
			totalOverlappingRatio_ch2=[0]*NbElements_ch2
		
			overlappingObjectCount_ch1=[0]*NbElements_ch1
			overlappingObjectCount_ch2=[0]*NbElements_ch2
			
			for i in xrange(0, int(self.ovlData.NbElements)):
		
				if ((self.ovlData.dataBase.has_key('filtered')) and (self.ovlData.dataBase['filtered'][i]==False)) or (self.ovlData.dataBase.has_key('filtered')==False) :
					
					tag_ch1=self.ovlData.dataBase['taggs_ch1'][i]
					index_ch1=self.ch1Data.find_value('tag',tag_ch1)
					overlappingRatio_ch1=self.ovlData.dataBase['ovlRatio_ch1'][i]
					
					tag_ch2=self.ovlData.dataBase['taggs_ch2'][i]
					index_ch2=self.ch2Data.find_value('tag',tag_ch2)
					overlappingRatio_ch2=self.ovlData.dataBase['ovlRatio_ch2'][i]
					
					if index_ch1!=None and index_ch2!=None:
						
						if ((self.ch1Data.dataBase.has_key('filtered')) and (self.ch1Data.dataBase['filtered'][index_ch1]==False)) and  ((self.ch2Data.dataBase.has_key('filtered')) and (self.ch2Data.dataBase['filtered'][index_ch2]==False)):
							
							overlappingObjectCount_ch1[index_ch1]+=1
							totalOverlappingRatio_ch1[index_ch1]+=overlappingRatio_ch1
							
							overlappingObjectCount_ch2[index_ch2]+=1
							totalOverlappingRatio_ch2[index_ch2]+=overlappingRatio_ch2
					
							
			self.ch1Data.add_dataBase({'totalOvlRatio':totalOverlappingRatio_ch1,'ovlObjectCount':overlappingObjectCount_ch1})
			self.ch2Data.add_dataBase({'totalOvlRatio':totalOverlappingRatio_ch2,'ovlObjectCount':overlappingObjectCount_ch2})
				
			#Filter remaining keys in channel 1 and channel2 database
			#filterList.append('isOnEdge')
			for key in self.dictChFilter_ch1.keys():
				
				if (key in self.ch1Data.dataBase.keys()) and (key not in filterList):
					
					dictFilterValues=self.dictChFilter_ch1[key]
					self.ch1Data.filter_dataBase(key,**dictFilterValues)
					
			for key in self.dictChFilter_ch2.keys():
			
				if (key in self.ch2Data.dataBase.keys()) and (key not in filterList):
					
					dictFilterValues=self.dictChFilter_ch2[key]
					self.ch2Data.filter_dataBase(key,**dictFilterValues)

		
			
			##################################################################Saving Final data#######################################################################################
			IJ.showStatus('Saving final results...')
			IJ.log('      Saving final results:')	
		
			IJ.log('           Saving final data to result files...')
			chKeyOrder=['tag','volume','intensity','meanIntensity','isOnEdge','totalOvlRatio','ovlObjectCount','centroidX','centroidY','centroidZ','barycenterX','barycenterY','barycenterZ']
			ovlKeyOrder=['tag','volume','taggs_ch1','ovlRatio_ch1','taggs_ch2','ovlRatio_ch2','isOnEdge','centroidX','centroidY','centroidZ']#'outputTaggs_overlapping']
			finalDataPath=join(self.outputDirectory, sourceName+'_ch_'+str(channel_ch1)+'_ch_'+str(channel_ch2)+'_final.res')	
			self.ch1Data.save_To_Text(finalDataPath,chKeyOrder,overwrite=True, descriptors=False, keys=True, types=False,title="channel="+str(channel_ch1))
			self.ch2Data.save_To_Text(finalDataPath,chKeyOrder, descriptors=False, keys=True, types=False,title="channel="+str(channel_ch2))
			self.ovlData.save_To_Text(finalDataPath,ovlKeyOrder, descriptors=False, keys=True, types=False,title="overlapping channel="+str(channel_ch1)+" channel="+str(channel_ch2))


			if self.batchChoice==True:
				
				#Create Summary file
				summaryPath_ch1=join(self.outputDirectory, 'summary_ch_'+str(channel_ch1)+'.txt')
				if not isfile(summaryPath_ch1):
					self.ch1Data.save_To_Text(summaryPath_ch1,chKeyOrder,overwrite=True, descriptors=False, keys=True, types=False,title="channel="+str(channel_ch1))
				else:
					self.ch1Data.save_To_Text(summaryPath_ch1,chKeyOrder,overwrite=False, descriptors=False, keys=False, types=False,title=None)
				
				
				summaryPath_ch2=join(self.outputDirectory, 'summary_ch_'+str(channel_ch2)+'.txt')
				if not isfile(summaryPath_ch2):	
					self.ch2Data.save_To_Text(summaryPath_ch2,chKeyOrder,overwrite=True, descriptors=False, keys=True, types=False,title="channel="+str(channel_ch2))
				else:
					self.ch2Data.save_To_Text(summaryPath_ch2,chKeyOrder,overwrite=False, descriptors=False, keys=False, types=False,title=None)
					
		except Exception , e:
			globalEllapsedTime = time.time() - globalStartTime
			IJ.log('') 
			IJ.log('Error occurred while processing '+str(sourceName)+' after '+str(globalEllapsedTime)+' seconds...')
			IJ.log('') 
			IJ.log(str(Exception)+': '+str(e))
			IJ.log('') 
			IJ.log(str(traceback.format_exc()))
			IJ.log('##########################################################################################################################') 

		##################################################################Finalization#######################################################################################
		finally:
			globalEllapsedTime = time.time() - globalStartTime
			IJ.log('')
			IJ.log('Total time for processing database was '+str(globalEllapsedTime)+' seconds...')
			IJ.log('##########################################################################################################################')
			IJ.log('##########################################################################################################################')
			#Enforce garbage collection, clear objects and images from memory:
			collect()

		
	def identification3D(self):
		
		'''This method takes the image from self.imp that should be a multiple channels and processes it according to the following workflow: 
			(1) two channels are loaded, 
			(2) segmented according to the settings in self.dictSeg_ch1/self.dictSeg_ch2, 
			(3) Objects are found using the 3D object counter that gives the output as an object map,
			(4) a database is created from each object in ch1 and ch2 (self.ch1Data, self.ch2Data), 
			(5) overlapping image is created by multiplying the segmented images from channel 1 and channel2,
			(6) overlapping objects are found using the 3D object counter, 
			(7) overlapping objects are identified and their connectivity and overlapping ratios are saved in a database (self.ovlData) and finally
			the databases are saved in a text file while the ch1,ch2 and overlapping object maps are saved in a three channel image to the output directory.
		Input:
			Method takes attributes from the CoExpressGui Class implicitly (!!!!!):
			self.imp: ImagePlus object (image)
			self.dictSeg_ch1/self.dictSeg_ch2: dictionaries with segmentation parameters
			self.dictChFilter_ch1/ self.dictChFilter_ch2: dictionaries (dictionary created by the self.analyze_GUI) with filters and the volume filter settings for the 3D object counter plugin

		Output:
			self.ch1Data, self.ch2Data, self.ovlData: database of objects from ch1.ch2 and the overlapping image. Each is an instance of the 
			CoExpressData class. Properties are as follows for ch1/ch2: tag (intensity in object map) volume, mean grey intensity. 
			Overlapping database has the following data for each object: tag, volume, object number in ch1, object number in ch2 (connectivity information)
			and the overlapping ratio in the other channel.
		'''
		try:
		################################################################################Inizialization#############################################################################################
			
			#Start measuring time from here
			globalStartTime = time.time()
			
			#Initialize list to store all open images and objects to clear memory (Jython interpreter has issues clearing memory)
			imageMemory=[]
			objectMemory=[]

			#Load segmentation parameters
			channel_ch1=self.dictSeg_ch1['channel']
			autothreshold_ch1=self.dictSeg_ch1['autothreshold']
			if autothreshold_ch1=='Manual':
				thrVal_ch1=self.dictSeg_ch1['thresholdValue']
			else:
				thrVal_ch1=1
			
			channel_ch2=self.dictSeg_ch2['channel']
			autothreshold_ch2=self.dictSeg_ch2['autothreshold']
			if autothreshold_ch2=='Manual':
				thrVal_ch2=self.dictSeg_ch2['thresholdValue']
			else:
				thrVal_ch2=1
		
			#Get image properties from self.imp		
			dictImgProperties=CoExpressProcessor.get_image_properties(self.imp)
			sourceFileName=dictImgProperties["source"]
			path=dictImgProperties["path"]
			width=dictImgProperties["width"]
			height=dictImgProperties["height"]
			NbSlices=dictImgProperties["NbSlices"]
			NbChannels=dictImgProperties["NbChannels"]
			NbFrames=dictImgProperties["NbFrames"]
			bitDepth=dictImgProperties["bitDepth"]
	
			(sourceName, sourceExtension) = splitext(str(sourceFileName))
		

			#Check if image has been saved
			if path==None:	
				raise CoExpressError('Image '+str(sourceFileName)+' has to be saved first!','')

			#Check the chosen channel number exists
			if NbChannels<channel_ch1 or NbChannels<channel_ch2:
				raise CoExpressError('Image '+str(sourceFileName)+' has only '+str(NbChannels)+' channels!','')		
			
			#Check if bitDepth is accepted
			if not (bitDepth==16 or bitDepth==8):
				raise CoExpressError('Image '+str(sourceFileName)+' has to have an 8-bit or 16-bit bithdepth!','')

			#Print image properties to log
			IJ.log(str(''))
			IJ.log('##########################################################################################################################')
			IJ.log('##########################################################################################################################')
			IJ.log('Processing: '+str(sourceFileName))
			IJ.log('     Image properties:')
			IJ.log('          Path: '+str(path))
			IJ.log('          Width: '+str(width))
			IJ.log('          Height: '+str(height))
			IJ.log('          Number of slices: '+str(NbSlices))
			IJ.log('          Number of channels: '+str(NbChannels))
			IJ.log('          Number of frames: '+str(NbFrames))
				
			
			#Print input parameters to log
			IJ.log(str(''))
			IJ.log('Input parameters: ')
		
			IJ.log('     Channel 1:')
			IJ.log('          Channel No.: '+str(channel_ch1))
			IJ.log('          Thresholding method: '+str(autothreshold_ch1))
			if autothreshold_ch1=='Manual':
				IJ.log('          Manual threshold: '+str(thrVal_ch1))	
			for key in self.dictChFilter_ch1.keys():
				text=''
				if 'minVal' in self.dictChFilter_ch1[key].keys():
					text=text+' minimum: '+str(self.dictChFilter_ch1[key]['minVal'])
				if 'maxVal' in self.dictChFilter_ch1[key].keys():
					text=text+' maximum: '+str(self.dictChFilter_ch1[key]['maxVal'])	
				IJ.log('          '+str(key)+': '+text)
		
			IJ.log('     Channel 2:')
			IJ.log('          Channel No.: '+str(channel_ch2))
			IJ.log('          Thresholding method: '+str(autothreshold_ch2))
			if autothreshold_ch2=='Manual':
				IJ.log('          Manual threshold: '+str(thrVal_ch2))	
			for key in self.dictChFilter_ch2.keys():
				text=''
				if 'minVal' in self.dictChFilter_ch2[key].keys():
					text=text+' minimum: '+str(self.dictChFilter_ch2[key]['minVal'])
				if 'maxVal' in self.dictChFilter_ch2[key].keys():
					text=text+' maximum: '+str(self.dictChFilter_ch2[key]['maxVal'])
				IJ.log('          '+str(key)+': '+text)
		
			if len(self.dictOvlFilter.keys())>0:
				IJ.log('     Overlapping Objects:')
				for key in self.dictOvlFilter.keys():
					text=''
					if 'minVal' in self.dictOvlFilter[key].keys():
						text=text+' minimum: '+str(self.dictOvlFilter[key]['minVal'])
			
					if 'maxVal' in self.dictOvlFilter[key].keys():
						text=text+' maximum: '+str(self.dictOvlFilter[key]['maxVal'])
			
					IJ.log('          '+str(key)+': '+text)

			################################################################################Chanel 1#############################################################################################
			#Start measuring time for this substep
			localStartTime=time.time()
	
			#Get channel 1 and segment image
			IJ.showStatus('Processing channel '+str(channel_ch1)+'...')
			IJ.log(str(''))
			IJ.log('Processing channel '+str(channel_ch1)+':')
		
			IJ.showStatus('Extracting channel '+str(channel_ch1)+' ...')
			IJ.log('          Extracting channel '+str(channel_ch1)+' ...')
			if autothreshold_ch1!='Manual':
				IJ.log('          Autothreshold values for channel '+str(channel_ch1)+' are:')	
			ch1SegmentationOutput=CoExpressGUI.segmentation(self.imp,channelChoice=channel_ch1,thresholdChoice=autothreshold_ch1,thresholdValue=thrVal_ch1)
			IJ.showStatus('Segmenting channel '+str(channel_ch1)+' ...')
			IJ.log('          Segmenting channel '+str(channel_ch1)+' ...')
			impChannel_ch1=ch1SegmentationOutput[0]
			arrChannel_ch1=CoExpressProcessor.stack_to_array(impChannel_ch1)
			impSegmentedChannel_ch1=ch1SegmentationOutput[1]

			#add images to  imageMemory
			imageMemory.append(impChannel_ch1)
			imageMemory.append(impSegmentedChannel_ch1)
			
			#Check if image is empty
			emptyFlag=CoExpressProcessor.checkIfEmpty(impSegmentedChannel_ch1)
			if emptyFlag==1:
				raise CoExpressError('Channel 1 Segmented image is empty!','')
			
			#Find objects in channel 1 and determine measured properties
			IJ.showStatus('Finding objects in channel '+str(channel_ch1)+'...')
			IJ.log('          Finding objects in channel '+str(channel_ch1)+'...')
			IJ.log('          Summary for channel '+str(channel_ch1)+' object count:')

			#Set Volume range
			minVol_ch1=1
			maxVol_ch1=None
			if self.dictChFilter_ch1.has_key('volume'):
				if self.dictChFilter_ch1['volume'].has_key('minVal'):
					minVol_ch1=self.dictChFilter_ch1['volume']['minVal']
				else:
					minVol_ch1=1

			if self.dictChFilter_ch1.has_key('volume'):
				if self.dictChFilter_ch1['volume'].has_key('maxVal'):
					maxVol_ch1=self.dictChFilter_ch1['volume']['maxVal']
				else:
					maxVol_ch1=None

			#Find objects
			impObjectMap_ch1=CoExpressProcessor.find_objects3D(impSegmentedChannel_ch1, filterOnEdge=not self.dictChFilter_ch1['isOnEdge']['maxVal'], minVol=minVol_ch1, maxVol=maxVol_ch1,barycenter=True,centroid=True, title='impObjectMap_ch1')
		
			#add images to  imageMemory
			imageMemory.append(impObjectMap_ch1)			

			#Convert channel 1 object map to array
			IJ.showStatus('Converting channel '+str(channel_ch1)+' object map into pixel array...')
			IJ.log('          Converting channel '+str(channel_ch1)+' object map into pixel array...')
			arrTagged_ch1=CoExpressProcessor.stack_to_array(impObjectMap_ch1)

			#add imageArray to  objectMemory
			objectMemory.append(arrTagged_ch1)
					
			#Creating database  of channel 1 objects
			IJ.showStatus('Creating database of objects in channel '+str(channel_ch1)+'...')
			IJ.log('          Creating database of objects in channel '+str(channel_ch1)+'...')
			dictResults_ch1=CoExpressProcessor. analyze_tagged_array(width, height, NbSlices, pixelArray=arrChannel_ch1, taggedArray=arrTagged_ch1 )	

			#Create database object for channel 1
			self.ch1Data=CoExpressData(dictResults_ch1)
			self.ch1Data.add_descriptor(dictImgProperties)
			self.ch1Data.add_descriptor(self.dictSeg_ch1)

			#Calculate ellapsed time
			localElapsedTime = time.time() - localStartTime
			IJ.log('     Processing channel '+str(channel_ch1)+' finished in '+str(localElapsedTime)+' seconds!')

			################################################################################Chanel 2#############################################################################################
			#Start measuring time for this substep
			localStartTime=time.time()
	
			#Get channel 2 and segment image
			IJ.showStatus('Processing channel '+str(channel_ch2)+'...')
			IJ.log(str(''))
			IJ.log('Processing channel '+str(channel_ch2)+':')
		
			IJ.showStatus('Extracting channel '+str(channel_ch2)+' ...')
			IJ.log('          Extracting channel '+str(channel_ch2)+' ...')
			if autothreshold_ch2!='Manual':
				IJ.log('          Autothreshold values for channel '+str(channel_ch2)+' are:')	
			ch2SegmentationOutput=CoExpressGUI.segmentation(self.imp,channelChoice=channel_ch2,thresholdChoice=autothreshold_ch2,thresholdValue=thrVal_ch2)
			IJ.showStatus('Segmenting channel '+str(channel_ch2)+' ...')
			IJ.log('          Segmenting channel '+str(channel_ch2)+' ...')
			impChannel_ch2=ch2SegmentationOutput[0]
			arrChannel_ch2=CoExpressProcessor.stack_to_array(impChannel_ch2)
			impSegmentedChannel_ch2=ch2SegmentationOutput[1]
			
			#add images to  imageMemory
			imageMemory.append(impChannel_ch2)
			imageMemory.append(impSegmentedChannel_ch2)
			
			#Check if image is empty
			emptyFlag=CoExpressProcessor.checkIfEmpty(impSegmentedChannel_ch2)
			if emptyFlag==1:
				raise CoExpressError('Channel 2 Segmented image is empty!','')
			
			#Find objects in channel 2 and determine measured properties
			IJ.showStatus('Finding objects in channel '+str(channel_ch2)+'...')
			IJ.log('          Finding objects in channel '+str(channel_ch2)+'...')
			IJ.log('          Summary for channel '+str(channel_ch2)+' object count:')
			
			#Set Volume range
			minVol_ch2=1
			maxVol_ch2=None
			
			if self.dictChFilter_ch2.has_key('volume'):
				if self.dictChFilter_ch2['volume'].has_key('minVal'):
					minVol_ch2=self.dictChFilter_ch2['volume']['minVal']
				else:
					minVol_ch2=1

			if self.dictChFilter_ch2.has_key('volume'):
				if self.dictChFilter_ch2['volume'].has_key('maxVal'):
					maxVol_ch2=self.dictChFilter_ch2['volume']['maxVal']
				else:
					maxVol_ch2=None
					
			impObjectMap_ch2=CoExpressProcessor.find_objects3D(impSegmentedChannel_ch2,filterOnEdge=not self.dictChFilter_ch2['isOnEdge']['maxVal'], minVol=minVol_ch2, maxVol=maxVol_ch2,barycenter=True,centroid=True,title='impObjectMap_ch2')
			
			#add images to  imageMemory
			imageMemory.append(impObjectMap_ch2)
			
			#Convert channel 2 object map to array
			IJ.showStatus('Converting channel '+str(channel_ch2)+' object map into pixel array...')
			IJ.log('          Converting channel '+str(channel_ch2)+' object map into pixel array...')
			arrTagged_ch2=CoExpressProcessor.stack_to_array(impObjectMap_ch2)
			
			#add imageArray to  objectMemory
			objectMemory.append(arrTagged_ch2)
						
			#Creating database  of channel 2 objects
			IJ.showStatus('Creating database of objects in channel '+str(channel_ch2)+'...')
			IJ.log('          Creating database of objects in channel '+str(channel_ch2)+'...')
			dictResults_ch2=CoExpressProcessor. analyze_tagged_array(width, height, NbSlices, pixelArray=arrChannel_ch2, taggedArray=arrTagged_ch2 )

			#Create database object for channel 2
			self.ch2Data=CoExpressData(dictResults_ch2)
			self.ch2Data.add_descriptor(dictImgProperties)
			self.ch2Data.add_descriptor(self.dictSeg_ch2)
		
			localElapsedTime = time.time() - localStartTime
			IJ.log('     Processing channel '+str(channel_ch2)+' finished in '+str(localElapsedTime)+' seconds!')
			
		
			################################################################################Overlapping#############################################################################################
			localStartTime = time.time()
			
			IJ.showStatus('Starting overlapping analysis...')
			IJ.log(str(''))
			IJ.log('Starting overlapping analysis:')
		
			IJ.showStatus('Generating overlapping image...')
			IJ.log('          Generating overlapping image...')
			impOverlapping=CoExpressProcessor.overlap_images(impObjectMap_ch1, impObjectMap_ch2)
			
			#Check if image is empty
			emptyFlag=CoExpressProcessor.checkIfEmpty(impOverlapping)
			if emptyFlag==1:
				raise CoExpressError('Overlapping image is empty!','')
			
			IJ.showStatus('Looking for overlapping objects...')
			IJ.log('          Looking for overlapping objects...')
			IJ.log('          Summary for overlapping object count:')
			

			impObjectMap_ovl=CoExpressProcessor.find_objects3D(impOverlapping, title='Overlapping')
			
			#Add array to imageMemory
			imageMemory.append(impObjectMap_ovl)

			IJ.showStatus('Converting overlapping object map into pixel array...')
			IJ.log('          Converting overlapping object map into pixel array...')
			arrTagged_ovl=CoExpressProcessor.stack_to_array(impObjectMap_ovl)
			IJ.showStatus('Creating database of  overlapping objects...')
			IJ.log('          Creating database of  overlapping objects...')
			dictResults_ovl=CoExpressProcessor. analyze_tagged_array(width, height, NbSlices, taggedArray=arrTagged_ovl )
			
			#add imageArray to  objectMemory
			objectMemory.append(arrTagged_ovl)
			
			dictConnectivity_ovl=CoExpressProcessor.identify_overlapping_objects(arrTagged_ch1, arrTagged_ch2, arrTagged_ovl )

			#Creating database of overlapping objects
			self.ovlData=CoExpressData(dictResults_ovl)
			self.ovlData.add_dataBase(dictConnectivity_ovl)
			self.ovlData.add_descriptor(dictImgProperties)
			self.ovlData.add_descriptor({'channel_1':channel_ch1 ,'channel_2':channel_ch2})

			#Calculate ellapsed time for this substep
			localElapsedTime = time.time() - localStartTime
			IJ.log('     Processing Overlapping image finished in '+str(localElapsedTime)+' seconds!')

			##################################################################Saving intermediate data##################################################################################
			IJ.showStatus('Saving intermediate data...')
			IJ.log(str(''))
			IJ.log('Saving intermediate data:')
		
			#Save intermediate images
			if self.saveImageChoice==True:

				#Save intermediate images
				IJ.log('     Saving intermediate images...')
				
				#Change LUT of ch1, ch2 and overlapping object map
				#Change bitdepth to ensure bitdepth is the same
				IJ.run(impObjectMap_ch1, "16-bit","")
				IJ.run(impObjectMap_ch1,"Green","")
				
				IJ.run(impObjectMap_ch2, "16-bit","")
				IJ.run(impObjectMap_ch2,"Red","")

				IJ.run(impObjectMap_ovl, "16-bit","");
				IJ.run(impObjectMap_ovl,"Yellow","")

				True==False
				
				#Merge raw ch1, ch2 and overlapping objects
				mergedImg=CoExpressProcessor.merge_images([impObjectMap_ch1,impObjectMap_ch2,impObjectMap_ovl])
				IJ.saveAsTiff(mergedImg, join(self.outputDirectory, sourceName+'_ch_'+str(channel_ch1)+'_ch_'+str(channel_ch2)+'_final'))
				imageMemory.append(mergedImg)


			#Save intermediate data
			IJ.log('     Saving data to result files...')	
			chKeyOrder=['tag','volume','intensity','meanIntensity','isOnEdge','centroidX','centroidY','centroidZ','barycenterX','barycenterY','barycenterZ']
			ovlKeyOrder=['tag','volume','taggs_ch1','taggs_ch2','isOnEdge','centroidX','centroidY','centroidZ']
			intermediatePath=join(self.outputDirectory, sourceName+'_ch_'+str(channel_ch1)+'_ch_'+str(channel_ch2)+'_raw.imd')	
			self.ch1Data.save_To_Text(intermediatePath,chKeyOrder,overwrite=True)
			self.ch2Data.save_To_Text(intermediatePath,chKeyOrder)
			self.ovlData.save_To_Text(intermediatePath,ovlKeyOrder)

			
		except Exception , e:
			globalEllapsedTime = time.time() - globalStartTime
			IJ.log('') 
			IJ.log('Error occurred while processing '+str(sourceFileName)+' after '+str(globalEllapsedTime)+' seconds...')
			IJ.log('') 
			IJ.log(str(Exception)+': '+str(e))
			IJ.log('') 
			IJ.log(str(traceback.format_exc()))
			IJ.log('##########################################################################################################################')


		
		##################################################################Finalization#######################################################################################
		finally:
			globalEllapsedTime = time.time() - globalStartTime
			IJ.log('') 
			IJ.log('Identification of objects in '+str(sourceFileName)+' has finished in '+str(globalEllapsedTime)+' seconds...')
			IJ.log('##########################################################################################################################')
			#Enforce garbage collection, clear objects and images from memory:
			CoExpressProcessor.clear_image_memory(imageMemory)
			CoExpressProcessor.clear_object_memory(objectMemory)
			collect()



class CoExpressProcessor(object):

	'''
	This class is a collection of methods most of which are static methods used to process images and image arrays.
	'''
							
	@staticmethod			
	def get_image_properties(imp):

		"""Get image properties form imagePlus object. Return value is a dictionarry that contains the width,height, number of slices and source title.
		imp: imagePlus object
    	"""
    	
		#Get image properties
		imgSource=str(imp.getTitle())
		imgWidth = imp.getWidth()
		imgHeight= imp.getHeight()
		imgNbSlices=imp.getNSlices()
		imgBitDepth=imp.getBitDepth()
		imgNbChannels=imp.getNChannels()
		imgNbFrames=imp.getNFrames()

		#get path
		win=imp.getWindow()
		WindowManager.setCurrentWindow(win)
		imgPath=IJ.getDirectory("image")
		
		#Create output dictionary and put in data
		outputDict = {"path":imgPath, "width" : imgWidth,"height" : imgHeight,"NbSlices":imgNbSlices, "source":imgSource,"bitDepth":imgBitDepth,"NbChannels":imgNbChannels,"NbFrames":imgNbFrames};
	
		return outputDict
		
	
	@staticmethod
	def get_channel(imp, channel, frame):
	
		"""Extract channel from given frame in an imagePlus object and returns a reference to extracted image (imagePlus object).
		imp: imagePlus object
		channel: int channe l number
		frame:frame int number
    	"""
		NbChannels=imp.getNChannels()
		if channel>NbChannels:
			raise CoExpressError('Invalid channel '+str(channel)+'!','')
    	
		outputName=imp.getTitle()+'_Ch'+str(channel)
		#Load Stack
		stack = imp.getImageStack() 
		#Create output stack with the same size as image
		stackOut = ImageStack(imp.width, imp.height)
	
		#Cycle through slices and append slices of the given channel/frame to output
		for i in xrange(1, imp.getNSlices()+1):
			#Get the corresponding processor and threshold
			current=imp.getStackIndex(channel, i, frame)
			stackOut.addSlice(str(i), stack.getProcessor(current))
				
		return ImagePlus(str(outputName), stackOut)

	@staticmethod
	def rotate_stack(imp):
	
		"""Rotate hyperstack by 90 degrees clockwise:
		imp: imagePlus object
		angel: angle of rotation
		"""
    	#Get image properties
		NbChannels=imp.getNChannels()
		NbFrames=imp.getNFrames()
    	
		#Load Stack
		stack = imp.getImageStack()
		size = stack.getSize()
		
		#Create output stack with the same size as image
		stackOut = ImageStack(imp.height, imp.width)
	
		#Cycle through stack,rotate and add to output stack
		for i in xrange(1,size+1):
			ip = stack.getProcessor(i)
			#ip.rotate(angel)
			
			stackOut.addSlice(str(i),ip.rotateRight())
	
		outputName=imp.getTitle()+'_rotated'	
		return ImagePlus(str(outputName), stackOut)
	

	@staticmethod
	def threshold( imp, threshold):

		"""Threshold an imagePlus object and returns a reference to the thresholded image (imagePlus object).
		imp: image
		outputName: name of the outputfile (has to be string, if it is a number it is converted to string)
    	threshold: if it is a string then the appropriate autothreshold method is chosen, if it is a number then it is taken as the lower threshold value.
   
    	"""

    	
		#Create output stack with the same size as image
		impTitle=imp.getTitle()
		impOut = imp.duplicate()
		impOut.setTitle(str(impTitle)+"_thresholded")
		
		#Apply threshold
		#Cycle through slices
		for i in xrange(1, imp.getNSlices()+1):
			#Get the corresponding processor and threshold
			impOut.stack.getProcessor(i).threshold(threshold)
		
		impOut.updateAndDraw()
	
		return impOut

	@staticmethod
	def autothreshold(imp, threshold):

		"""Threshold an imagePlus object and returns a reference to the thresholded image (imagePlus object)..
		imp: image
		outputName: name of the outputfile (has to be string, if it is a number it is converted to string)
    	threshold: if it is a string then the appropriate autothreshold method is chosen, if it is a number then it is taken as the lower threshold value.
    	"""
		#get image properties
		impBitDepth=imp.getBitDepth()
		impTitle=imp.getTitle()

		if (impBitDepth==8 or impBitDepth==16):
			
			#Create output stack with the same size as image
			impOut = imp.duplicate()
			impOut.setTitle(str(impTitle)+"_thresholded")

			#Autothreshold if possible (can be invalid input etc.)
			try:
				IJ.run(impOut, "Auto Threshold", "method="+str(threshold)+" white show stack")
				impOut.updateAndDraw()
			except:
				raise CoExpressError('Could not autothreshold using the '+str(threshold)+' method!','')
		else:
			raise CoExpressError('Only 8-bit and 16-bit images can be autothresholded using the '+str(threshold)+' method!','')
			
		return impOut
		

	@staticmethod
	def checkIfEmpty(imp):

		"""Check if image contains non-zero values and returns 1 if the image is empty.
		imp: imagePlus
    	"""
    	
		#Load Original Stack
		stack = imp.getImageStack() 
	
		#Cycle through slices in stack to to check for non zero elements
		maxInt=0
		for i in xrange(1, imp.getNSlices()+1):
			#Get the corresponding processor
			stat=imp.stack.getProcessor(i).getStatistics() 
  			if stat.max>maxInt:
  				maxInt=stat.max
		
  		if maxInt>0:
			output=0
		else:
			output=1
		
  		return output
  		
	@staticmethod
	def find_objects3D(imp, minVol=1, maxVol=None, filterOnEdge=False, barycenter=False, centroid=False, title=None):

		'''
		Finds object in a 3D hyperstack (imagePlus object) using the 3d Object Counter plugin and returns an object map (the object pixels are given their tag
		as an intensity value) and a results table is created if wanted.

		minVol/ maxVol: volume range for the 3D object counter
		filterOnEdge: Boolean. If true objects touching the edge are not taken into account.
		Barycenter/centroid: Boolean flags to set the contents of the results table
		'''
	

		#Set title if title=None
		if title==None:
			#Get input imagePlus title
			rawTitle=str(imp.getTitle())
			(title , extension) = splitext(rawTitle)
	
		if maxVol==None:
			imgWidth = imp.getWidth()
			imgHeight= imp.getHeight()
			imgNbSlices=imp.getNSlices()
			maxVol=imgWidth*imgHeight*imgNbSlices
	
		#Setup 3D OC settings
		settings="volume nb_of_obj._voxels mean_gray_value"
		if centroid==True:
			settings=settings+" centroid "
		if barycenter==True:
			settings=settings+" centre_of_mass "
		settings=settings+" dots_size=5 font_size=10 redirect_to=none"
		
		inputText="threshold=1 slice=10 min.="+str(int(minVol))+" max.="+str(int(maxVol))
		if filterOnEdge==True:
			inputText=inputText+" exclude_objects_on_edges "
		inputText=inputText+" objects summary"
		
		#Run 3D OC
		IJ.run("3D OC Options", settings)
		IJ.run(imp, "3D Objects Counter", inputText)

		#Create output ImagePlus
		impOut=IJ.getImage()
		impOut.hide()
		impOut.setTitle(title+"_objects")
	
		return impOut
		
	@staticmethod
	def overlap_images(imp,imp2):

		'''		Takes two images (imagePlus object) and creates an image with the overlapping areas by multiplying the tho images. 
		Returns a reference to the overlapping imagePlus.
	
		Imp,im2: imagePlus objects
		'''
		ic=ImageCalculator()
		ic.calculate("Multiply create stack", imp, imp2)
		impOut=IJ.getImage()
		impOut.hide()
		
		return impOut
		
	
	@staticmethod
	def get_results(barycenter=False,centroid=False):
	
		'''Loads the results table from the output of the 3D Object counter.
		
		Barycenter,centroid: Boolean flags True if one wants the barycenter/centroid coordinates.
		'''	
		
		#check if results table exists
		resultsExists=WindowManager.getWindow('Results')
		if resultsExists==None:
			raise CoExpressError('There is no results table to be processed!','')

		#get results table
		table = ResultsTable.getResultsTable()

		#Get number of objects and create a list containing taggs
		NbObjects=table.getCounter()
		tag = [x for x in range(1, NbObjects+1)]
		
		#Read Data
		#Read volume
		index=table.getColumnIndex('Nb of obj. voxels')
		volume=table.getColumn(index)
		outputVolumeList=volume.tolist()

		#Read mean grey intensity
		index=table.getColumnIndex('Mean')
		intensity=table.getColumn(index)
		outputMeanIntensityList=intensity.tolist()

		#Create Output Dictionary
		outputDict = {"tag": tag, "volume":outputVolumeList, "intensity":outputMeanIntensityList}
	
		if centroid==True:
			#Read centroid
			index=table.getColumnIndex('X')
			centroidX=table.getColumn(index)
			outputCentroidXList=centroidX.tolist()

			index=table.getColumnIndex('Y')
			centroidY=table.getColumn(index)
			outputCentroidYList=centroidY.tolist()

			index=table.getColumnIndex('Z')
			centroidZ=table.getColumn(index)
			outputCentroidZList=centroidZ.tolist()

			outputDict["centroidX"]=outputCentroidXList
			outputDict["centroidY"]=outputCentroidYList
			outputDict["centroidZ"]=outputCentroidZList

		
		if barycenter==True:
			#Read barycenter
			index=table.getColumnIndex('XM')
			barycenterX=table.getColumn(index)
			outputBarycenterXList=barycenterX.tolist()

			index=table.getColumnIndex('YM')
			barycenterY=table.getColumn(index)
			outputBarycenterYList=barycenterY.tolist()

			index=table.getColumnIndex('ZM')
			barycenterZ=table.getColumn(index)
			outputBarycenterZList=barycenterZ.tolist()
		
			outputDict["barycenterX"]=outputBarycenterXList
			outputDict["barycenterY"]=outputBarycenterYList
			outputDict["barycenterZ"]=outputBarycenterZList
	
		#Clear results table
		IJ.run("Clear Results")
	
		return outputDict

	@staticmethod
	def stack_to_array(imp):
	
		"""Convert stack from image stack to a pixel array. The array is stored in a Java array and is the return value.
		imp: imagePlus object
   		"""
		#Get image properties
		Width = imp.getWidth()
		Height= imp.getHeight()
		NbSlices=imp.getNSlices()
	
		#Create a list of pixels
		stack = imp.getImageStack() 
	
		imagePixels=[]  
		for i in xrange(1, imp.getNSlices()+1):
			fp = stack.getProcessor(i).convertToFloat()		
			pix=fp.getPixels()
			for i in xrange(0,len(pix)):
				imagePixels.append(int(pix[i]))
		
		imgArray=array(imagePixels, 'i')
		del imagePixels[:]
		
		return imgArray
	
	@staticmethod
	def identify_overlapping_objects(taggedObjArray1, taggedObjArray2, taggedOverlappingArray ):

		"""Identifies overlapping objects in two tagged image pixel arrays (taggedObjArray1 and taggedObjArray2) and returns using
		a pixel array of their overlapping image (taggedOverlappingArray ). These arrays
		are Java arrays to speed up processing times. Returns a dictionarry containing the connectivity information
		
		taggedObjArray1, taggedObjArray2, taggedOverlappingArray: Java arrays
    	"""
    	#Check if image arrays at input have the same length
		try:
			len(taggedObjArray1)==len(taggedObjArray2)==len(taggedOverlappingArray)

		except:
			raise Ace3DError('Input image arrays have to have the same length!','')
			
		NbPixels=len(taggedObjArray1)
		
		#Generate Java array for output: overlappingTaggs1(2) contain the taggs in taggedObjArray1(2) that overlap with
		#taggedOverlappingArray.
		NbOverlappingObj=max(taggedOverlappingArray)
		taggs_ch1=[0]*NbOverlappingObj
		taggs_ch2=[0]*NbOverlappingObj
		
		#Check which objects overlap: if
		for i in xrange(0, NbPixels):
			currOvlID=taggedOverlappingArray[i]-1
			if (taggedOverlappingArray[i]>0 and (taggs_ch1[currOvlID]==0 or taggs_ch2[currOvlID]==0)):
				
				taggs_ch1[currOvlID]=taggedObjArray1[i]
				taggs_ch2[currOvlID]=taggedObjArray2[i]
		
		outputDict = {"taggs_ch1":taggs_ch1, "taggs_ch2":taggs_ch2}
	
		return outputDict
		
	@staticmethod
	def analyze_tagged_array(width, height, NbSlices, pixelArray=None, taggedArray=None ):
	
		"""Analyze tagged image array. Measures the number of objects, volume, cumulative intensity and mean intensity of each object in 'taggedImgArray' using pixel values in 
		'imgArray'(the original image). Return value is a dictionary containing the number of objects and lists of the tags of each object with their respective volumes and intensities
		
		imgArray: pixel array with the original image
		taggedImgArray: tagged pixel array
		Widht: Widht of the image 
		Height: height of the image  
		NbSlices: number of slices in the stack 
    	"""
		
		simpleFlag=False
		if pixelArray==None:
			simpleFlag=True
		
		length=width*height*NbSlices
		maxTag=taggedArray[0]
		for i in xrange(1,length):
			if (taggedArray[i]>maxTag):
				maxTag=int(taggedArray[i])
		
		#######################################################Inizialize output arrays##############################################################
		#Initialize Java arrays to store volume,intensity and isOnEdge state
		volumeList=zeros(maxTag,'i')
		isOnEdgeList=zeros(maxTag,'z')
		centroidXList=zeros(maxTag,'d')
		centroidYList=zeros(maxTag,'d')
		centroidZList=zeros(maxTag,'d')

		if simpleFlag==False:
			intensityList=zeros(maxTag,'d')
			barycenterXList=zeros(maxTag,'d')
			barycenterYList=zeros(maxTag,'d')
			barycenterZList=zeros(maxTag,'d')
		
		#################################Mesaure volume and intensity and determine if object is on edge using tagged image###########################
		arrayIndex=0
		for z in xrange(1, NbSlices+1):
			for y in xrange(0, width):
				for x in xrange(0, height):
					
					#print taggedArray[arrayIndex]
					if (taggedArray[arrayIndex]!=0):
						currentID=int(taggedArray[arrayIndex])-1

						volumeList[currentID]=volumeList[currentID]+1

						centroidXList[currentID]=centroidXList[currentID]+x
						centroidYList[currentID]=centroidYList[currentID]+y
						centroidZList[currentID]=centroidZList[currentID]+z

						if simpleFlag==False:
							intensityList[currentID]=intensityList[currentID]+pixelArray[arrayIndex]
							barycenterXList[currentID]=barycenterXList[currentID]+pixelArray[arrayIndex]*x
							barycenterYList[currentID]=barycenterYList[currentID]+pixelArray[arrayIndex]*y
							barycenterZList[currentID]=barycenterZList[currentID]+pixelArray[arrayIndex]*z

  						#Check if the current particle is touching an edge
						if(x==0 or y==0 or x==width-1 or y==height-1 or (NbSlices!=1 and (z==1 or z==NbSlices))):
							isOnEdgeList[currentID]=True
					arrayIndex+=1
					
		##########################Finalize output data:calculate barycenters, centroids and remove zero elements, sort based on tags##########################################
		#Calculate barycenters/centroidsCreate output lists that contains tuples of output parameters that can be easily sorted
		dataMatrix=[]
		NbObjects=0
		for i in xrange(0, len(volumeList)):
			if volumeList[i]>0:
				NbObjects+=1

				#Calculate centroid coordinates
				centroidX=centroidXList[i]/float(volumeList[i])
				centroidY=centroidYList[i]/float(volumeList[i])
				centroidZ=centroidZList[i]/float(volumeList[i])

				if simpleFlag==False:
					#Calculate MeanGrey intensity
					meanGreyintensity=float(intensityList[i])/float(volumeList[i])

					#Calculate baricenter coordinates if intensity is larger than zero (eg. if the imgArray is randomized or from an other chanel!!!)
					if intensityList[i]>0:
						barycenterX=barycenterXList[i]/float(intensityList[i])
						barycenterY=barycenterYList[i]/float(intensityList[i])
						barycenterZ=barycenterZList[i]/float(intensityList[i])
					else:
						barycenterX='N/A'
						barycenterY='N/A'
						barycenterZ='N/A'
					
					dataMatrix.append((i+1,volumeList[i],isOnEdgeList[i],centroidX,centroidY,centroidZ, intensityList[i],meanGreyintensity,barycenterX,barycenterY,barycenterZ))
				if simpleFlag==True:
					#dataMatrix.append((i,volumeList[i],intensityList[i],meanGreyintensity,isOnEdgeList[i],barycenterX,barycenterY,barycenterZ,centroidX,centroidY,centroidZ))
					dataMatrix.append((i+1,volumeList[i],isOnEdgeList[i],centroidX,centroidY,centroidZ))
		
		#sort output based on tag number in ascending order, Sorting taggs is important when looking for tag values
		dataMatrix.sort(reverse=False, key=lambda x: x[0])

		#Create list for each output parameter
		outputVolumeList=[]
		outputIsOnEdgeList=[]
		outputMeanIntensityList=[]
		outputTagList=[]
		outputCentroidXList=[]
		outputCentroidYList=[]
		outputCentroidZList=[]

		if simpleFlag==False:
			outputIntensityList=[]
			outputBarycenterXList=[]
			outputBarycenterYList=[]
			outputBarycenterZList=[]

		for i in range(0, len(dataMatrix)):
			outputTagList.append(dataMatrix[i][0])
			outputVolumeList.append(dataMatrix[i][1])
			outputIsOnEdgeList.append(dataMatrix[i][2])
			outputCentroidXList.append(dataMatrix[i][3])
			outputCentroidYList.append(dataMatrix[i][4])
			outputCentroidZList.append(dataMatrix[i][5])

			if simpleFlag==False:
				outputIntensityList.append(dataMatrix[i][6])
				outputMeanIntensityList.append(dataMatrix[i][7])
				outputBarycenterXList.append(dataMatrix[i][8])
				outputBarycenterYList.append(dataMatrix[i][9])
				outputBarycenterZList.append(dataMatrix[i][10])

		outputDict = {"volume":outputVolumeList, "isOnEdge":outputIsOnEdgeList, 'tag':outputTagList, "centroidX":outputCentroidXList,"centroidY":outputCentroidYList,"centroidZ":outputCentroidZList }
		
		if simpleFlag==False:
			outputDict["intensity"]=outputIntensityList
			outputDict["meanIntensity"]=outputMeanIntensityList
			outputDict["barycenterX"]=outputBarycenterXList
			outputDict["barycenterY"]=outputBarycenterYList
			outputDict["barycenterZ"]=outputBarycenterZList
	
		return outputDict
	
	@staticmethod
	def merge_images(imageList, name=None, composite=True, keep=True):

		"""Merge images in list.
		imageList:lsit of imagePlus objects
		composit:if True the output image is a composit
		keep:if True keep images
		name: title of th eoutput image
		"""
		
		#Create input for "Merge Channel plugin"
		inputText=""
		visibleImages=[True]*len(imageList)
		
		#check if image is Visible. if not show (otherwise the merge channels does not work)
		for i in range(0,len(imageList)):
			
			if imageList[i].isVisible()==False:
				visibleImages[i]=False
				imageList[i].show()
				
			inputText=inputText+" "+"c"+str(i+1)+"="+imageList[i].title
	
		if composite==True:
			inputText=inputText+" create"

		if keep==True:
			inputText=inputText+" keep"
		
		#Merge channels
		IJ.run("Merge Channels...", inputText)

		#If image is not visible before running macro hide them
		for i in range(0,len(imageList)):
			if visibleImages[i]==False:
				imageList[i].hide()
				
		#Get output image and rename if name!=None
		outImp=IJ.getImage()
		if name!=None:
			outImp.setTitle(str(name))	
		#Hide outImp
		outImp.hide()
		
		#Return output image
		return outImp
		
	@staticmethod
	def clear_image_memory(imageList):
		
		'''
		Forces ImageJ to clear the memory from the imagePlus objects in the imageList.
		
		imageList: list of imageJ objects
		'''

		for image in imageList:
			image.close() 
			image.flush()

	@staticmethod
	def clear_object_memory(objectList):
		
		'''Forces the Java Virtual Machine to clear objects in objectList  from memory 
		'''

		for objects in objectList:
			del objects
	
	@staticmethod
	def get_open_images():

		'''Gets a list of open images and a list of paths from the ImageJ WindowManager.
		'''
	
		fileNameList=[]
		pathList=[]
		#Create list of open images
		if WindowManager.getIDList()!=None:
			
			imageList = [f for f in map(WindowManager.getImage, WindowManager.getIDList())if f.getWindow()!=None]
			#Cycle through image list
			for img in imageList:
				#get title
				title=img.getTitle()
				
				#Select image window
				win=img.getWindow()
				WindowManager.setCurrentWindow(win)
				
				#append only images that have been saved (have a path)
				if win!=None:
		
					#get path
					path=IJ.getDirectory("image")
				
			
					if path!=None:
						fileNameList.append(title)
						pathList.append(path)

		
		return fileNameList, pathList
			
	@staticmethod
	def quote():
	
		'''Generates a random quote (most of which are from Gaussian03).
		'''
	
		quoteList=["You know you're a teacher when you say 2, write 3, and mean 4.\n     -Ronald Anstrom, high school teacher, Underwood, N.D. 1974",
		"Research is what I am doing when I don't know what I am doing.\n     -Werner von Braun",
		"Learn from yesterday, live for today, look to tomorrow, rest this afternoon.\n     -Snoopy",
		"The great thing about being imperfect is the joy it brings others.\n     -Sign outside Lake Agassiz Jr. High School, Fargo, N.D.",
		"It is unworthy of excellent men to lose hours like slaves in the labor of calculation which could be safely relegated to anyone else if a machine were used.\n     -G. W. von Leibniz",
		"Achievement - the man who rows the boat generally doesn't have time to rock it.\n     -From the back of a sugar packet",
		"Nothing will be attempted if all possible objections must first be overcome.\n     -The golden principle, Paul Dickson's ""The Official Rules""",
		"The reason man's best friend is a dog is because he wags his tail instead of his tongue.\nIf you perceive that there are four possible ways in which a procedure can go wrong and circumvent these, then a fifth way will develop.\n     -Murphy's twelfth law",
		"Standing in the middle of the road is very dangerous; you get knocked down by the traffic from both sides.\n     -Margaret Thatcher",
		"You will never ""find"" time for anything. If you want time, you must make it.\n     -Charles Bixton)",
		"Be not the first by whom the new are tried, nor yet the the last to lay the old aside.\n     -Alexander Pope",
		"If it happens, it must be possible.\n     -The unnamed law from Paul Dickson's ""The Official Rules""",
		"Life can only be understood backward, but must be lived forward.\n     -Kirkegaard",
		"If you don't have the law - argue the facts. If you don't have the facts - argue the law. If you don't have either - pound the table.\n     -Gaussian QC",
		"You know you've spoken too long when the audience stops looking at their watches and starts shaking them.\n     -Gaussian QC",
		"The best way to pay for a lovely moment is to enjoy it.\n     -Richard Bach",
		"Happiness is not having what you want - happiness is wanting what you have!\n     -From Mrs. Severn's desk",
		"Ashes to ashes\nDust to dust\nOil those brains\nBefore they rust.\n     -J. Prelutsky",
		"The human brain, then, is the most complicated organization of matter that we know.\n     -Isaac Asimov",
		"The brain struggling to understand the brain is society trying to explain itself.\n     -Colin Blakemore",
		"Whatever happens in the mind of man is represented in the actions and interactions of brain cells.\n     -Geschwind and A.M. Galaburda",
		"Brain: an apparatus with which we think that we think.\n     -Ambrose Bierce)",
		"Mind: A mysterious form of matter secreted by the brain.\n     -Ambrose Bierce",
		"Ah! My poor brain is racked and crazed,\nMy spirit and senses amazed!\n     -Johann Wolfgang Von Goethe:Faust, 1808",
		"I am a brain, Watson. The rest of me is a mere appendix.\n     -Arthur Conan Doyle:Sherlock Holmes",
		"I like nonsense; it wakes up the brain cells.\n     -Dr. Seuss",
		"Money spent on brain is never spent in vain.\n     -English Proverb", 
		"Ask for advice, and then use your brain.\n     -Norwegian Proverb",
		"If you prick me, do I not leak\n     -Data from Star Trek",
		"Data: Apologies, Captain. I seem to have reached an odd functional impasse. I am stuck.\nPicard: Then get unstuck and continue with the briefing.\nData: Yes, sir. That is what I am trying to do, sir, but the solution eludes me.\n     -Star Trek:The Last Outpost)â,
		"Your focus determines your reality.\n     -Qui-Gon Jinnâ
		"Do. Or do not. There is no try.\n     -Yodaâ
		"Your eyes can deceive you. Donât trust them.\n     -Obi-Wan Kenobi)â
		"A life is like a garden. Perfect moments can be had, but not preserved, except in memory.\n     -Leonard Nimoy, not Spock",
		"Scissors cuts paper, paper covers rock, rock crushes lizard, lizard poisons Spock,\nSpock smashes scissors, scissors decapitates lizard, lizard eats paper,\npaper disproves Spock, Spock vaporises rock, and as it always has, rock crushes scissors.\n     -Sheldon:The Big Bang Theory)",
		"Physics is like sex: sure, it may give some practical results, but that''s not why we do it\n     -Richard P. Feynman",
		"You will have a long and wonderfull life\n     -Chinese fortune cookie",
		"If we knew what it was we were doing, it would not be called research, would it?\n     -Albert Einstein",
		"I have not failed. I''ve just found 10,000 ways that won''t work.\n     -Thomas Alva Edison",
		"Science is organized common sense where many a beautiful theory was killed by an ugly fact.\n     -Thomas Huxley",
		"The most exciting phrase to hear in science, the one that heralds the most discoveries, is not ''Eureka!'' but ''That''s funny...\n     -Isaac Asimov",
		"If you try and take a cat apart to see how it works, the first thing you have on your hands is a non-working cat.\n     -Douglas Adams",
		"Philosophy of science is about as useful to scientists as ornithology is to birds.\n     -Richard P. Feynman",
		"I have never let my schooling interfere with my education.â\n     -Mark Twain
		"If your result needs a statistician then you should design a better experiment.\n     -Ernest Rutherford",
		"Every generation of humans believed it had all the answers it needed, except for a few mysteries they assumed would be solved at any moment.\nAnd they all believed their ancestors were simplistic and deluded. What are the odds that you are the first generation of humans who will understand reality?\n     -Scott Adams (Dilbert)",
		"In the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.\n     -Douglas Adams",
		"The function of science fiction is not always to predict the future but sometimes to prevent it.\n     -Frank Herbert",
		"There ain't no surer way to find out whether you like people or hate them than to travel with them.\n     -Mark Twain",
		"A true friend is someone who is there for you when he'd rather be anywhere else.\n     -Len Wein",
		"Asking dumb questions is easier than corecting dumb mistakes.\n     -Gaussian QC",
		"Sacred cows make the best hamburger.\n     -Mark Twain",
		"The solution to a problem changes the problem.\n     -John Peers: Paul Dickson's ""The Official Rules""",
		"Education without common sense is a load of books on the back of an ass.\n     -Gaussian QC",
		"Everything's got a moral, if only you can find it.\n -Lewis Carrol, Alice in Wonderlan",
		"Experience is what you get when you don't get what you want.\n     -Dan Stanford",
		"We have left undone those things which we ought to have done, and we have done those things which we ought not to have done.\n     -Book of common prayer",
		"A fool can ask more questions than a wise man can answer.\n     -Gaussian QC",
		"Mondays are the potholes in the road of life.\n     -Tom Wilson",
		"Actors are so fortunate. They can choose whether they will appear in a tragedy or in comedy, whether they will suffer of make merry, laugh or shed tears. but in real life it is different.\nMost men and women are forced to perform parts for which they have no qualifications. The world is a stage, but the play is badly cast.\n     -Oscar Wilde",
		"In the universe the difficult things are done as if they were easy.\n     -Lao-Tsu",
		"A bird in the hand is safer than one overhead.\n     Newton's seventh law",
		"Be like a postage stamp. Stick to one thing until you get there.\n     -Josh Billings",
		"Steinbach's guidelines for systems programming: never test for an error condition you don't know how to handle.\n     -Gaussian QC",
		"Chinese fortune cookie of Jan 1 1967 say.... all things are difficult before they are easy.\nWe learn so little and forget so much. You will overcome obstacles to achieve success. Ah so.",
		"Getting a simple answer from a professor is like getting a thimble of water from a fire hydrant.\n     -Prof. Len Shapiro, NDSU",
		"Models are to be used, not believed.\n     -Paraphrased by H. Thiel, Principles of Econometrics",
		"If god had meant man to see the sun rise, he would have scheduled it for a later hour.\n     -Gaussian QC",
		"Science at its best provides us with better questions, not absolute answers.\n     -Norman Cousins, 1976",
		"We are reaching the stage where the problems we must solve are going to become insoluble without computers.\nI do not fear computers. I fear the lack of them.\n     -Issac Asimov",
		"Be careful not to become too good of a songbird or they''ll throw you into a cage.\n     -Snoopy to Woodstock",
		"The world is made up of the wills, the won''ts, and the cant''s: the wills do everything, the won''ts do nothing, the can'ts can't do anything.\n     From Walt Disney's ""Black Hole""",
		"The meek shall inherit the earth. (The rest of us will escape to the stars)\n     -Gaussian QC
		"All papers that you save will never be needed until such time as they are disposed of, when they become essential.\n     -John Corcoran in Paul Dickson's ""The Official Rules""",
		"There is no subject, however complex, which, if studied with patience and intelligience will not become more complex.\n     -Quoted by D. Gordon Rohman",
		"If you get confused, logic out your dilemma.\n     -Picker X-ray corp. digital printer manual CA. 1964
		"Fatherhood is pretending the present you love most is soap-on-a-rope.\n     -Bill Cosby",
		"To err is human - and to blame it on a computer is even more so.\n     -Gaussian QC",
		"If you want to learn from the theoretical physicists about the methods which they use, I advise you to follow this principle very strictly:/ndon''t listen to their words; pay attention, instead, to their actions.\n     -A. Einstein, 1934",
		"Those with the gold make the rules.\n     -Peter''s golden rule",
		"To suspect your own mortality is to know the beginning of terror. To learn irrefutably that you are mortal is to know the end of terror.\n     -Jessica: Children of Dune by Frank Herbert",
		"A politician is a person who can make waves and then make you think he''s the only one who can save the ship.\n     -Ivern Ball",
		"Wisdom is knowing what to do, skill is knowing how to do it, and virtue is not doing it.\n     -Gaussian QC",
		"All science is either physics, or stamp collecting.\n     -Ernest Rutherford, 1871—1937",
		"Theory: supposition which has scientific basis, but not experimentally proven. Fact: a theory which has been proven by enough money to pay for the experiments.\n     -The wizard of ID",
		"It is a capital mistake to theorize before one has data. Insensibly one begins to twist facts to suit theories rather than theories to suit facts.\n     -Sherlock Holmes",
		"Common sense is not so common.\n     -Voltaire",
		"Everybody is ignorant, only on different subjects.\n     -Will Rogers",
		"Unless we change directions, we will wind up where we are headed.\n     -Confucius",
		"The whole of science is nothing more than a refinement of everyday thinking.\n     -A. Einstein",
		"My opinions may have changed, but not the fact that I am right.\n     -Ashleigh Brilliant",
		"Never teach a pig to sing. It wastes your time and annoys the pig.\n     -Seen on a greeting card",
		"Michael Faraday, asked by a politician what good his electrical discoveries were, replied ""at present I do not know, but one day you will be able to tax them.""",
		"A chemical physicist makes precise measurements on impure compounds. A theoretical physical chemist makes imprecise measurements on pure compounds. An experimental physical chemist makes imprecise measurements on impure compounds.\n     -Gaussian QC",
		"KNOWING is a barrier which prevents learning.\n     -Teaching of the Bene Gesserit",
		]
		
		randomNumber=randint(0, len(quoteList)-1)

		return quoteList[randomNumber]

  		
############################################################################################################################################
############################################################################################################################################
############################################################################################################################################
############################################################################################################################################
############################################################################################################################################	
class CoExpressData(object):

	'''Data container that contains a database stored in a dictionary and descriptors for additional metadata like sorce filename, source path, number of elements etc.
	dataBase: dictionary containing database. Values are lists with each index rapresenting one object (length of each list has to be the same
	NbElements: number of objects in the database (length of the list)
	dataBaseKeys: A list of keys in the dataBase dictionary
	descriptors: list of descriptor keys'''
	
	def __init__(self, dictionary):
	
		'''Initialization method. Adds the input dictionary to the dataBase attribute and updates default attributes (NbElements,dataBaseKeys,descriptors)
		dictionary: input dictionary that is added to the databvase'''
		
		self.dataBase=dictionary
		self.dataBaseKeys=self.dataBase.keys()
		self.NbElements=self.__getNbElements__()				
		self.descriptors=self.__dict__.keys()
		self.descriptors.remove("dataBase")
		
	def __getitem__(self, key):
	
		'''Method that allows one to access database dictionary elements using their keys using the instanceName[key] sintax.'''
		
		if key not in self.__dict__.keys():
			raise AttributeError('Invalid attribute!')		
		return self.__dict__[key]

	def __str__(self):

		'''Creates a string representation of the class so it can be printed using the str(instanceName) syntax.'''
		
		#outputstring for descriptors:
		strDescriptors='{'
		for key in self.descriptors:
			strDescriptors=strDescriptors+key+':'+str(self.__dict__[key])+','
		strDescriptors=strDescriptors.strip(',')
		strDescriptors=strDescriptors+'}'
		#Output string for dataBase elements
		strDataBase=str(self.dataBase)
		
		return "descriptors:\n"+strDescriptors+"\ndataBase:\n"+strDataBase
		
	def __update_dataBase__(self):
	
		'''Method to update the list of  "descriptors" and "databaseKeys" anddescriptor data members.'''
		
		self.dataBaseKeys=self.dataBase.keys()
		self.descriptors=self.__dict__.keys()
		self.descriptors.remove("dataBase")		
		
	def __getNbElements__(self):
	
		'''Method returns number of entries in the database and checks if the database fields are lists and that their lenght is the same and returns with the length. '''
		
		if len(self.dataBaseKeys)>0:
			
			length=len(self.dataBase[self.dataBaseKeys[0]])
			for i in range(1, len(self.dataBaseKeys)-1):
				key=self.dataBaseKeys[i]
				element=self.dataBase[key]
													
				if len(element)!=length:
					raise CoExpressError('All data attributes have to be lists of the same length!','')
					
		
		return length
		
	def find_value(self, key, value):
	
		'''Finds value in the dataBase for a given key and returns the index if value exists or returns None if value is not found.
		key: key in the dataBase dictionary
		value: value to be found'''
	
		if key in self.dataBase.keys():
			valueList=self.dataBase[key]
			index=None
			for i in range(0, len(valueList)):
				if valueList[i]==value:
					index=i
					break
			return index
		
	def add_dataBase(self, dictionary, overwrite=False):
	
		'''Add new keys/values to the dataBase dictionary.
		dictionary: input dictionary containing the new keys and value list
		overwrite: if False only new keys are accepted and if True if the key already exists the value list is overwritten
		'''
		
		for key in dictionary.keys():
		
			if self.dataBase.has_key(key) and overwrite==False:
				raise CoExpressError('Dictionary already has a key: '+key,'')
			if len(dictionary[key])==int(self.NbElements):
				self.dataBase[key]=dictionary[key]
			else:
				raise CoExpressError('Length of entry '+key+' is not equal to NbElements!','')
		self.__update_dataBase__()

	def add_descriptor(self, dictionary, overwrite=False):
	
		'''Add new descriptors to the class instance.
		dictionary: input dictionary containing the new keys and value list
		overwrite: if False only new keys are accepted and if True if the key already exists the value list is overwritten
		'''
		
		for key in dictionary.keys():
			if self.dataBase.has_key(key) and overwrite==False:
				raise CoExpressError('Dictionary already has a key: '+key,'')
			else:
				self.__dict__[key]=dictionary[key]
		self.__update_dataBase__()
		
	def filter_dataBase(self, key, overwrite=False, **kwargs):
	
		'''Method to filter the entries of a key value in the dataBase dictionary. Minimum and maximum values are given as keyword arguments as the minVal and maxVal variable. 
		If it does not exist a new key "Filtered" is added to the dictionary that will contain a list of booleans. Entries will be true if filter is true and false otherwise. 
		If overwrite=True and the key "Filtered exists" it will be cleared (all entries set to False) and filtered with the curren filter settings. If overwrite=False results
		of previous filters will not be	cleared.
		key: dictionary key for dataBase data element
		overwrite: flag that determins if "Filtered" key in dataBase will be overwritten
		keywargs: keyword arguments to set minVal and maxVal'''
		
		keys=kwargs.keys()

		#Set minVal 
		if 'minVal' in keys:
			minimum=kwargs['minVal']
		else:
			minimum=min(self.dataBase[key])

		#Set maxVal 
		if 'maxVal' in keys:
			maximum=kwargs['maxVal']		
		else:
			maximum=max(self.dataBase[key])

		#If minimum is larger tha maximum exchange values
		if minimum>maximum:
			minimum, maximum = maximum, minimum

		#If database does not have a 'filtered' key or overwrite=True, add new 'filtered' key
		length=int(self.NbElements)
		if ('filtered' not in self.dataBase.keys()) or overwrite==True:
			self.dataBase['filtered'] = [False] * length

		self.__update_dataBase__()

		#Filter
		for i in range(0, length):
			
			if (self.dataBase[key][i]>maximum or self.dataBase[key][i]<minimum):
				self.dataBase['filtered'][i]=True
		
	
				
	def remove_database_element(self, index):
	
		'''Removes entries with a given index from the dataBase.'''

		if (index<-self.NbElements or index>=self.NbElements):# -self counter to allow negative indexes (remove from end)
			raise IndexError('Data field index out of range')
			break
		else:
			for key in self.dataBaseKeys:
				del self.dataBase[key][index]
			#If index is in range and is remove decrement data counter
			self.NbElements-=1
	
	def remove_filtered(self):
	
		'''Removes filtered entries is dataBase if it has a "Filtered" key.'''
		
		if self.dataBase.has_key('filtered'):
			
			for i in xrange(self.NbElements-1,-1,-1):
				
				if self.dataBase['filtered'][i]==True:
					for key in self.dataBase.keys():
						del self.dataBase[key][i]
					self.NbElements-=1	
		else:
			raise CoExpressError('Database has to be filtered first!','')
			
	def save_To_Text(self,filePath, keyOrderList=[], overwrite=False, descriptors=True, keys=True, types=True, title=None):
		
		'''Append CoExpressData object to a tab delimited text file, with the possibility to determine printing order.
		filePath: path to the file
		keyOrderList: list to determine printing order
		overwrite: if False and file exists appends CoExpressData instance , otherwise deletes previous file and creates new one.
		title: if value is NOT None the first line appended to the file
		descriptors: flag that determines if the descriptors wil be print or not
		keys: flag that determins if the keys of the dataBase dictionary will be printed before the data
		types: flag that determines if the types of the dataBase entries of each key should be printed before data (usefull if data will be later loaded and reused)
		'''
		
		#Delete file if it exists
		if isfile(filePath) and overwrite==True:
			remove(filePath)

		#Create a list containing the keys that have to be printed in order, followed by the other keys
		keyList=keyOrderList+list(set(self.dataBaseKeys).difference(set(keyOrderList)))

		#update data counter and database
		self.__getNbElements__()
		self.__update_dataBase__()
		
		#Should be using "with", however the jython interpreter of ImageJ does not support futures
		# with control-flow structure to ensure that clean-up code is executed ('from __future__ import with_statement' needed 
		#in the beginning of lines for python 2.5 and earlier)
		f=open(filePath,'a')

		#Print title line
		if title!=None:
			f.write('#title:\n')
			f.write(str(title)+'\n')
			
		#Print descriptors with values
		if descriptors==True:
			f.write('#descriptors:\n')
			for key in self.descriptors:
				if  key!='descriptors' and key!='dataBaseKeys':
					f.write(key+'='+str(self.__dict__[key])+'\t')
			f.write('\n')
			
		#Save the dataBase keys in a tab delimited table
		if  keys==True:
			f.write('#dataBase:\n')
			for key in keyList:
				if  key in self.dataBaseKeys:
					f.write(key+'\t')
			f.write('\n')

		#Save the dataBase types in a tab delimited table
		if  types==True:
			for key in keyList:
				if key in self.dataBaseKeys:
					f.write(str(type(self.dataBase[key][0]))+'\t')
			f.write('\n')

		#Save the dataBase data in a tab delimited table
		for i in range(0,int(self.NbElements)):
			for key in keyList:
				if key in self.dataBaseKeys:
					f.write(str(self.dataBase[key][i])+'\t')
			f.write('\n')

		#Close file
		f.close()


	@staticmethod
	def load_from_file(filePath):
		'''Loads all CoExpressData objects from textfile. Descriptors to str, dataBase values to int, float, boolean or string. The loaded objects are returned as a list.
		filePath: Filepath of the text file
		'''
		
		#Should be using "with", however the jython interpreter of ImageJ does not support futures
		inputFile=open(filePath,'r')

		#Create a list of lines from file
		lines = inputFile.read().strip("\n").splitlines()

		outputList=[]
		index=0
		while index!=len(lines):
						

			if lines[index]=='#descriptors:':
			
				#Create dictionary to store descriptors and data
				dataBaseDict={}
				descriptorDict={}
				
				#Increment index so descriptors are read from the next line
				index+=1
				splitLine=lines[index].strip("\n").split("\t")
				
				#Fill dictonary containing descriptors
				for i in xrange(0,len(splitLine)-1):#-1 because of the last tab
					elements=splitLine[i].split("=")
					
					if elements[0] not in descriptorDict.keys():
						descriptorDict[elements[0]]=elements[1]
					else:
						raise CoExpressError('File was corrupt: duplicate descriptor: '+elements[0]+' !','')

				#Read in dataBase keys,datatypes data
				index+=1
				if lines[index]=='#dataBase:':

					#Read dataBase keys and setup dictionary for data
					index+=1
					dataBaseKeys=lines[index].strip("\n").split("\t")
					for i in xrange(0,len(dataBaseKeys)-1):#-1 because of the last tab
						dataBaseDict[dataBaseKeys[i]]=[]

					#Read dataBase keys and setup dictionary for data
					index+=1
					dataBaseTypes=lines[index].strip("\n").split("\t")
					
					#Read data until line containing'#descriptors:' (next Ace3D object) is or index reaches last value
					index+=1
					while True:

					
						if lines[index]=='#descriptors:':
							break

						#Get data values
						valueList=lines[index].strip('\n').split("\t")

						#Convert values to designated types. Supported: str, int, float,bool
						for i in xrange(0,len(dataBaseKeys)-1):#-1 because of the last tab
							val=valueList[i]
							typ=dataBaseTypes[i]
							if typ=="<type 'int'>":
								val=int(val)
							elif typ=="<type 'float'>" :
								val=float(val)
							elif typ=="<type 'bool'>":
								val=CoExpressData.str_to_bool(val)
							elif typ=="<type 'str'>" :
								pass
							else:
								raise CoExpressError('Unrecognized type read from file: '+filePath+' !','')
							dataBaseDict[dataBaseKeys[i]].append(val)

						if index==len(lines)-1:
							break
						index+=1

					#Create CoExpressData object
					buff=CoExpressData(dataBaseDict)
					buff.add_descriptor(descriptorDict)
					buff.__update_dataBase__()
					#Decrement index
					index-=1
			
			index+=1
			#Append	CoExpressData to outputlist	
			if index==len(lines):
				break
			else:
				outputList.append(buff)
				
		#Close file
		inputFile.close()
			
		return outputList
	
	
	@staticmethod
	def str_to_bool(string):
		'''Converts string to boolean. Function is not case sensitive! True if string is "yes", "true", "t" or "1" in lower case.
		False is all other cases, so beware.
		string: string to be converted
		'''
  		return string.lower() in ("yes", "true", "t", "1")


#Class for error handling
class CoExpressError(Exception):
	def __init__(self, message, errors):
			
		super(CoExpressError, self).__init__(message)
		self.errors = errors
			
	def __str__(self):
		return repr(self.message)



		
if __name__ == '__main__':
	
	
	a=CoExpressGUI()



	
	
	