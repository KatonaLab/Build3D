import os
import sys
import subprocess
import copy
import random
import traceback
import numpy as np
from modules.packages.PythImage.ImageClass import ImageClass as PythImage
import SimpleITK as sitk
from ast import literal_eval


class VividImage(PythImage):
    '''
    
    
    
    ITK image: Is an image type used by the SimpleITK package.
    
    MultiDimImageFloat: Is an image type used by the Visualization framwork of A3DC
    .This image type can contain only 3D data for a single channel and time steps! 
    The image type has a meta attribute that contains the metadata in string form.
    If the image is from an ics module the image metadata are converted to Ome-Tiff
    compatibble key-value pairs. The ics loading module reads ics metadata and 
    gives it a name based on the function that reads the metadata with subelements 
    divided by ':'. ICS files have required keys (see Dean P, Mascio L, Ow D, Sudar
     D, Mullikin J.,Proposed standard for image cytometry data files., Cytometry. 
     1990;11(5):561-9.) if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is 
    among the metadata keys the metadata is taken asics metadata!!!!!. Currently 
    only one channel and first time point is loaded as an image. The channel number
     is added as the 'channel' key along with data type as the 'type key', the 
     source file path as the 'path' key, the probe emission wavelength as 
     'wavelength'. The 'normalized' key is True if the image has been normalized 
    between 0 and 1  and False otherwise. These later keys are not ics 
    compatible metadata keys!!! Dimension order of the reader is XYZ. Samples per 
    pixel daa not read from ics header so it is set to 1 default.
    '''
    
    __REQUIRED_ICS_KEYS=['IcsGetCoordinateSystem','IcsGetSignificantBits']
    __DIM_TRANSLATE=PythImage._ImageClass__DIM_TRANSLATE   
    
    def __init__(self, image, metadata, database=None):
     
        #Check if compulsory keys are missing
        key_list=['Type']# 'Name', 'SizeC', 'SizeT', 'SizeX', 'SizeY', 'SizeZ', 'DimensionOrder']
        missing_keys=[]
        for key in key_list:
            if key not in metadata:
                missing_keys.append(key)
        if len(missing_keys)>0:
            raise Exception('Invalid Metadata! Missing the following keys: '+str(missing_keys))
        
        
        #Check if metadata 'Type' field matches the type of the image
        if metadata['Type']!=image.dtype:
             #raise Warning('Image array type is '+str(array.dtype)+' while metadata is '+str( metadata['Type'])+' ! Metadata is modified acordingly!')
             image=image.astype(metadata['Type'])

        #Call parent __init__
        super(VividImage, self).__init__(image, metadata)
        
        #Set database if supplied
        if database!=None:
            self.database=database

    def is_3d(self):
        
        if self.metadata['SizeT']>1:
            flag=False
        elif self.metadata['SizeC']>1:
            flag=False
        else:
            flag=True
            
        return flag
    ################
    def get_dimension(self, index, dimension='C'):

        #Get channel from image. 
        if dimension not in self.__DIM_TRANSLATE.keys():
            raise Exception('Invalid dimension %s! Value must be in %s' % (str(dimension), str(self.__DIM_TRANSLATE.keys())))
            
        if index>=self.metadata[self.__DIM_TRANSLATE[dimension]] or 0>index:
            raise Exception('Image dimension %s has a length of %s ! Index %s is invalid!' % (str(dimension) ,str(self.metadata[self.__DIM_TRANSLATE[dimension]]),str(index)))
        
        #Create metadata
        metadata=copy.deepcopy(self.metadata)
        metadata[self.__DIM_TRANSLATE[dimension]]=1
        if dimension=='C':
            
            if isinstance(metadata['SamplesPerPixel'], list):
                metadata['SamplesPerPixel']=metadata['SamplesPerPixel'][index]
                metadata['Name']=metadata['Name'][index] 
            elif index==0:
                metadata['SamplesPerPixel']=metadata['SamplesPerPixel']
                metadata['Name']=metadata['Name']
            else:
                raise IndexError('Invalid Index {} ! the "Name" and "SamplesPerPixel" metadata keys are lists for multichannel images.'.format(str(index)))
                
             
        #Extract axis from image array from image array
        order=self.metadata['DimensionOrder']

        array=np.take(self.image, index, len(order)-order.index(dimension)-1)

        return VividImage(array, metadata)
    
    def get_3d_array(self, T=None, C=None):
        
        output=self

        if T!=None:
            if self.metadata['SizeT']>T+1 or self.metadata['SizeT']<0:
                raise Exception('Invalid time index {}. Image has {} time points!'.format( T, self.metadata['SizeT']))
            else:
                output=output.get_dimension( T, dimension='T')
        
        if C!=None:
            if self.metadata['SizeC']>C+1 or self.metadata['SizeC']<0:
                raise Exception('Invalid channel index {}. Image has {} time points! '.format( C, self.metadata['SizeC']))
            else:
                output=output.get_dimension( C, dimension='C')
        
        if output.metadata['SizeT']==1 and output.metadata['SizeC']==1 :
            self.reorder('ZYXCT')
            array=output.image[0][0]
            
        else:
            raise Exception('Image has to be 3D only (must contain one time point or one channel)!')


        return array

    def to_itk(self):

        
        itk_img = sitk.GetImageFromArray(self.get_3d_array())
        
        #Check if physical size metadata is available if any is missing raise Exeption
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        missing_size=[s for s in size_list if s not in self.metadata.keys()]
        if len(missing_size)!=0:
            print('Missing :'+str(missing_size)+'! Unable to carry out analysis!', file=sys.stderr)
        else:
            itk_img.SetSpacing((self.metadata['PhysicalSizeX'],self.metadata['PhysicalSizeY'],self.metadata['PhysicalSizeZ']))
        
        #Check if unit metadata is available
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in self.metadata.keys()]
        if len(missing_size)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)
    
        return itk_img
    
    @classmethod
    def from_multidimimage(cls, multidimimage, database=None):
        
        import a3dc_module_interface as md
        
        def is_ics(multidimimage):
            '''Check if image has been loaded from ics image. ICS files have required keys 
            (see Dean P, Mascio L, Ow D, Sudar D, Mullikin J.,Proposed standard for 
            image cytometry data files., Cytometry. 1990;11(5):561-9.) Function checks 
            if 'IcsGetCoordinateSystem' OR 'IcsGetSignificantBits' is among the 
            dictionary keys the dictionary is taken as ics.
            '''
            return (multidimimage.meta.has(VividImage.__REQUIRED_ICS_KEYS[0]) or multidimimage.meta.has(VividImage.__REQUIRED_ICS_KEYS[1]))   
        
        def ics_to_metadata(array, ics_metadata):
        
            #Get Shape information
            ome_metadata={'SizeT': 1, 'SizeC':1, 'SizeZ':array.shape[-1], 'SizeX':array.shape[0], 'SizeY':array.shape[1]}
            ome_metadata['DimensionOrder']='ZYXCT'
            
            channel=int(ics_metadata['channel'])
            
            #Add Type and path
            ome_metadata['Type']=ics_metadata['type']
            ome_metadata['Path']=ics_metadata['path']
            #!!!!!!!!!Not read from header by ics loder module!!!!!!!
            ome_metadata['SamplesPerPixel']=1
            
            #Generate channel name
            if str('IcsGetSensorExcitationWavelength:'+str(channel)) in ics_metadata.keys(): 
                ome_metadata['Name']='Probe_Ex_{}nm'.format(str(int(float(ics_metadata['IcsGetSensorExcitationWavelength:'+str(channel)]))))
            elif str('IcsGetSensorEmissionWavelength:'+str(channel)) in ics_metadata.keys():
                ome_metadata['Name']='Probe_Em_{}nm'.format(str(int(float(ics_metadata['IcsGetSensorEmissionWavelength:'+str(channel)]))))
            else:
                ome_metadata['Name']= 'Ch'+str(channel)
            
            #Get scale information in ome compatible format
            scale_dict={'IcsGetPosition:scale:x':'PhysicalSizeX','IcsGetPosition:scale:y':'PhysicalSizeY', 'IcsGetPosition:scale:z':'PhysicalSizeZ'}
            for key in scale_dict.keys():
                if key in ics_metadata.keys():
                    ome_metadata[scale_dict[key]]=ics_metadata[key]
            
            #Get scale unit information in ome compatible format
            unit_dict={'IcsGetPosition:units:x':'PhysicalSizeXUnit','IcsGetPosition:units:y':'PhysicalSizeYUnit', 'IcsGetPosition:units:z':'PhysicalSizeZUnit'}
            for key in unit_dict.keys():
                if key in ics_metadata.keys():
                    ome_metadata[unit_dict[key]]=ics_metadata[key]
    
            return ome_metadata
    
        
        def metadata_to_dict( multidimimage):
        
            metadata={}
            for idx, line in enumerate(str(multidimimage.meta).split('\n')[1:-1]):
                    
                    line_list=line.split(':')
                    
                    #Get key and value. Ics metadata keys have : as separator
                    #for the 'path' key the path is separated as well.
                    if line_list[0].lstrip().lower()=='path':
                        key=line_list[0].lstrip()
                        value=':'.join(line_list[1:])
                    else:
                        key=':'.join(line_list[:-1])
                        value=multidimimage.meta.get(key)
                    
                    #ad metadata key value to outpit dictionary
                    try:
                        metadata[key]=literal_eval(value)
        
                    except:
                        metadata[key]=value
            
            return metadata
    
    
        #get image array
        array=md.MultiDimImageFloat_to_ndarray(multidimimage)
    
        #Get image metadata and convert database if the metadata is ICS style
        metadata=metadata_to_dict(multidimimage)
    
        if is_ics(multidimimage):
            metadata=ics_to_metadata(array, metadata)
        #else:
           #array=array[::,::-1,::] 
        
        #Create output image    
        output=cls(array, metadata)
    
        #Add database if available
        if database!=None and isinstance(database, dict):
            output.database=database
    
        return output
    


    def to_multidimimage(self):
        
        import a3dc_module_interface as md
        
        #Check if image is time series
        if self.metadata['SizeT']>1:
            warning("Image is a time series! Only the first time step will be extracted!")
            self.metadata['SizeT']=1
            
        #Check if image has multiple channels
        if self.metadata['SizeC']>1:
            warning("Image is a multichannel image! Only the first channel will be extracted!")
            self.metadata['SizeC']=1
        
        #Create output MultiDimImageFloat
        self.reorder('ZYXCT')
        multidimimage=md.MultiDimImageFloat_from_ndarray(self.image[0][0].astype(np.float))
    
        
        #Clear metadata
        multidimimage.meta.clear()
        
        for key in self.metadata.keys():
            multidimimage.meta.add(key, str(self.metadata[key]))        
            
        return multidimimage 
 
    
SEPARATOR='#####################################################################################################################'
    
QUOTE_LIST=["You know you're a teacher when you say 2, write 3, and mean 4.\n     -Ronald Anstrom, high school teacher, Underwood, N.D. 1974",
		"Research is what I am doing when I don't know what I am doing.\n     -Werner von Braun",
		"Learn from yesterday, live for today, look to tomorrow, rest this afternoon.\n     -Snoopy",
		"The great thing about being imperfect is the joy it brings others.\n     -Sign outside Lake Agassiz Jr. High School, Fargo, N.D.",
		"It is unworthy of excellent men to lose hours like slaves in the labor of calculation which could be safely relegated to anyone else if a machine were used.\n     -G. W. von Leibniz",
		"Achievement - the man who rows the boat generally doesn't have time to rock it.\n     -From the back of a sugar packet",
		"Nothing will be attempted if all possible objections must first be overcome.\n     -The golden principle, Paul Dickson's ""The Official Rules""",
		"If you perceive that there are four possible ways in which a procedure can go wrong and circumvent these, then a fifth way will develop.\n     -Murphy's twelfth law",
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
		"Brain: an apparatus with which we think that we think.\n     -Ambrose Bierce",
		"Mind: A mysterious form of matter secreted by the brain.\n     -Ambrose Bierce",
		"Ah! My poor brain is racked and crazed,\nMy spirit and senses amazed!\n     -Johann Wolfgang Von Goethe:Faust, 1808",
		"I am a brain, Watson. The rest of me is a mere appendix.\n     -Arthur Conan Doyle:Sherlock Holmes",
		"I like nonsense; it wakes up the brain cells.\n     -Dr. Seuss",
		"Money spent on brain is never spent in vain.\n     -English Proverb", 
		"Ask for advice, and then use your brain.\n     -Norwegian Proverb",
		"If you prick me, do I not leak\n     -Data from Star Trek",
       "Data: Apologies, Captain. I seem to have reached an odd functional impasse. I am stuck.\nPicard: Then get unstuck and continue with the briefing.\nData: Yes, sir. That is what I am trying to do, sir, but the solution eludes me.\n     -Star Trek:The Last Outpost",
		"Your focus determines your reality.\n     -Qui-Gon Jinn",
		"Do. Or do not. There is no try.\n     -Yoda",
		"Your eyes can deceive you. Don't trust them.\n     -Obi-Wan Kenobi",
		"A life is like a garden. Perfect moments can be had, but not preserved, except in memory.\n     -Leonard Nimoy, not Spock",
		"Scissors cuts paper, paper covers rock, rock crushes lizard, lizard poisons Spock,\nSpock smashes scissors, scissors decapitates lizard, lizard eats paper,\npaper disproves Spock, Spock vaporises rock, and as it always has, rock crushes scissors.\n     -Sheldon:The Big Bang Theory)",
		"Physics is like sex: sure, it may give some practical results, but thats not why we do it\n     -Richard P. Feynman",
		"You will have a long and wonderfull life\n     -Chinese fortune cookie",
		"If we knew what it was we were doing, it would not be called research, would it?\n     -Albert Einstein",
		"I have not failed. Ive just found 10,000 ways that wont work.\n     -Thomas Alva Edison",
		"Science is organized common sense where many a beautiful theory was killed by an ugly fact.\n     -Thomas Huxley",
		"The most exciting phrase to hear in science, the one that heralds the most discoveries, is not Eureka! but Thats funny...\n     -Isaac Asimov",
		"If you try and take a cat apart to see how it works, the first thing you have on your hands is a non-working cat.\n     -Douglas Adams",
		"Philosophy of science is about as useful to scientists as ornithology is to birds.\n     -Richard P. Feynman",
		"I have never let my schooling interfere with my education.\n     -Mark Twain",
		"If your result needs a statistician then you should design a better experiment.\n     -Ernest Rutherford",
		"Every generation of humans believed it had all the answers it needed, except for a few mysteries they assumed would be solved at any moment.\nAnd they all believed their ancestors were simplistic and deluded. What are the odds that you are the first generation of humans who will understand reality?\n     -Scott Adams (Dilbert)",
		"In the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.\n     -Douglas Adams",
       "The function of science fiction is not always to predict the future but sometimes to prevent it.\n     -Frank Herbert",
		"There ain't no surer way to find out whether you like people or hate them than to travel with them.\n     -Mark Twain",
		"A true friend is someone who is there for you when he'd rather be anywhere else.\n     -Len Wein",
		"Asking dumb questions is easier than corecting dumb mistakes.\n     -Gaussian QC",
		"Sacred cows make the best hamburger.\n     -Mark Twain",
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
		"Be careful not to become too good of a songbird or theyll throw you into a cage.\n     -Snoopy to Woodstock",
		"The world is made up of the wills, the wonts, and the cants: the wills do everything, the wonts do nothing, the can'ts can't do anything.\n     From Walt Disney's ""Black Hole""",
		"The meek shall inherit the earth. (The rest of us will escape to the stars)\n     -Gaussian QC",
		"All papers that you save will never be needed until such time as they are disposed of, when they become essential.\n     -John Corcoran in Paul Dickson's 'The Official Rules'",
		"There is no subject, however complex, which, if studied with patience and intelligience will not become more complex.\n     -Quoted by D. Gordon Rohman",
		"If you get confused, logic out your dilemma.\n     -Picker X-ray corp. digital printer manual CA. 1964", 
		"To err is human - and to blame it on a computer is even more so.\n     -Gaussian QC",
		"If you want to learn from the theoretical physicists about the methods which they use, I advise you to follow this principle very strictly:/ndont listen to their words; pay attention, instead, to their actions.\n     -A. Einstein, 1934",
		"Those with the gold make the rules.\n     -Peter's golden rule",
		"To suspect your own mortality is to know the beginning of terror. To learn irrefutably that you are mortal is to know the end of terror.\n     -Jessica: Children of Dune by Frank Herbert",
		"A politician is a person who can make waves and then make you think hes the only one who can save the ship.\n     -Ivern Ball",
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
       "You shall know the truth, and the truth shall set you free.\n     -John 8:32",
       "Scientific progress is the discovery of a more and more comprehensive simplicity... The previous successes give us confidence in the future of science: we become more and more conscious of the fact that the universe is cognizable.\n     -Monsignor Georges Lemaître",
       "The miracle of the appropriateness of the language of mathematics for the formulation of the laws of physics is a wonderful gift which we neither understand nor deserve.\n     -Wigner Eugene",
       "The eternal mystery of the world is its comprehensibility…The fact that it is comprehensible is a miracle.\n     -Albert Einstein",
       "Better to illuminate than merely to shine, to deliver to others contemplated truths than merely to contemplate.\n     -Thomas Aquinas",
       "The things that we love tell us what we are.\n     -Thomas Aquinas",
       "Nonsense is nonsense even when spoken by world-famous scientists.\n     -John Lennox",
       "Begin with the beautiful, which leads you to the good, which leads you to the truth.\n     -Robert Barron",
       "Start by doing what's necessary; then do what's possible; and suddenly you are doing the impossible.\n    -Francis of Assisi",
       "The task of the modern educator is not to cut down jungles, but to irrigate deserts.\n     -C.S. Lewis",
       "The future is something which everyone reaches at the rate of 60 minutes an hour, whatever he does, whoever he is.\n     -C.S. Lewis",
       "Failures, repeated failures, are finger posts on the road to achievement. One fails forward toward success.\n     -C.S. Lewis",
       "Yet trees are not 'trees', until so named and seen\n and never were so named, till those had been\n who speech's involuted breath unfurled,\n faint echo and dim picture of the world\n     -J.R.R. Tolkien",
       "The heart of Man is not compound of lies, but draws some wisdom from the only Wise.\n     -J.R.R. Tolkien",
       "For surely “nothing” is every bit as physical as “something,” especially if it is to be defined as the “absence of something.\n     -Lorentz Krauss",
       "Imagine a kettle boiling on a stove. The scientist can tell you much about it. What temperature the water will boil at, the interaction of atoms at different temperatures, the change in the nature of matter and many other interesting and important things.\n But what science can never discover is that that the kettle is on the stove so that you can have a cup of tea with a friend. That is the real reason the you are boiling the kettle and science can't, nor was it ever designed to, tell you that.\n     -Sir John Polkinghorne",
       "It is important to realize that in physics today, we have no knowledge of what energy “is”.\n     -Richard Feynman",
       "THE WORLD IS TOO COMPLICATED IN ALL ITS PARTS AND INTERCONNECTIONS TO BE DUE TO CHANCE ALONE.\n     -Alan Sandage",
       "Logic is the most useful tool of all the arts. Without it no science can be fully known.\n     -William of Ockham",
       "It is pointless to do with more what can be done with fewer.\n     -William of Ockham",
       "The reason man's best friend is a dog is because he wags his tail instead of his tongue.\n     -Anonymus",
       "Wonder is the desire of knowledge.\n    -Thomas Aquinas" ,
       "Characteristics which define beauty are wholeness, harmony and radiance.\n    -Thomas Aquinas"
		]
def reorder_list(lst, val_list):

    for element in reversed(val_list):
        if element in lst:
            lst.remove(element)
            lst.insert(0, element)

    return lst


def round_up_to_odd(f):
    #Round to add as blocksize has to be odd
    return int(np.ceil(f) // 2 * 2 + 1)


def os_open(path):
    '''Open file using its associated program in an os (MacOS, Linux, Windows) 
    dependent manner. Exceptions are raised as warnings using a try statement.
    
    path(str): The path of the file to be opened
    '''
    try:
        #Windows
        if sys.platform == "win32":
            os.startfile(path)
        #MacOS
        elif sys.platform == "darwin":
           subprocess.call(["open", path]) 
        #Linux/Unix
        else:
            subprocess.call(["xdg-open", path])
    
    except Exception as e:
        raise Warning(str(e))


def quote(verbose=False):
	
    '''Generates a random quote (most of which are from Gaussian03).
    '''
	 #Generate random index
    index=random.randint(0,len(QUOTE_LIST)-1)
    
    #Get quote
    quote=QUOTE_LIST[index]
    
    #Print if verbose is set to true
    if verbose:
        print_line_by_line(quote)
    
    return quote


def print_line_by_line(string, file=sys.stdout):
    
    string_list=string.split("\n")
    for i in string_list:
        print(i, file)


def warning(string):
    
    print(string, file=sys.stderr)
        

def error(message, exception=None, verbose=True):

    if verbose==False:
        len=1
    else:
        len=10
    
    print(SEPARATOR, file=sys.stderr)
    
    print("Traceback:",file=sys.stderr)
    print(traceback.format_exc(len), file=sys.stderr)
    
    print(SEPARATOR, file=sys.stderr)
    print(message, file=sys.stderr) 
    print(SEPARATOR, file=sys.stderr) 

    raise Exception(message, exception)


def value_to_key(dictionary, val):
    
    #Get the ocurrences of val among dictionary values
    count=sum(value == val for value in dictionary.values())
    #version 2: count=sum(map((val).__eq__, dictionary.values()))
    
    #If value is not in dictionary.values raise exception
    if count==0:
        raise LookupError('Value %s is not in dictionary'.forma(str(val)))
    if count>1:
        raise LookupError('More than one key have value %s!'.forma(str(val)))
    
    #get value
    #version 2: list(dictionary.keys())[list(dictionary.values()).index(val)]
    for key, value in dictionary.items():
        if value == val:
            return key 


def dictinary_equal(dict_1,dict_2):
    '''Compares two dictionaries and returns true is dictionaries have the same
    keys and values are equal (key and value objects have to be comparable)
    '''
    flag=True
    for key, value in dict_1.items():
        if key in dict_2.keys():
            if dict_2[key]!=value:
                flag=False
        else:
            flag=False
            
    return flag

def rename_duplicates(string_list):
    '''Processes a list of strings. If list has duplicate elements an index is added to it.
    '''
    if isinstance(string_list, str) or not isinstance(string_list, list):
        raise Exception('Object must be list of strings!')
    
    output = []
    for idx, val in enumerate(string_list):
        totalcount = string_list.count(val)
        count = string_list[:idx].count(val)
        output.append(val +'_'+ str(count + 1) if totalcount > 1 else val)
    
    return output

#Class for error handling
class VividException(Exception):
    def __init__(self, message, errors):
			
        super(VividException, self).__init__(message)
        self.errors = errors
    
        print(traceback.format_exc(10), file=sys.stderr)	
        
    def __str__(self):
        
        return repr(self.errors)
    