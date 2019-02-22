import unittest
import numpy as np
import time
import cProfile
import pstats 
import io
import sys
import copy

from context import core
from context import segmentation
from context import VividImage
from assets import assets_core


class CoreTest(unittest.TestCase):
        
    def setUp(self):
        self.__start = time.time()
        print('Running: {}'.format(self.id()))
        #print('{} ({}s)'.format(self.id(), round(elapsed, 2))) 
        
    def tearDown(self):
        elapsed = time.time() - self.__start
        print('Finished: {} ({}s)\n'.format(self.id(), round(elapsed, 10)))    
    


    def test_analyze(self):
         
        
        for idx, cs in enumerate(assets_core.analyze_case_list):
            
            
            
            case_text="Case "+str(idx+1)+" with "
            
            #Settings
            if cs['settings']==None:
                case_text=case_text+'Default settings'
            else:
                for key in cs['settings']:
                    case_text+=' '+str(key)+' = '+str(cs['settings'][key])
            
            if cs['settings']!=None:
                output =core.analyze(cs['input'], **cs['settings'] )
            
            else:
                output =core.analyze(cs['input'])
            
            #Test if image data types are equal
            with self.subTest():
                self.assertEqual(output.image.dtype,  cs['result'].image.dtype, msg='Data type of image arrays do not match in '+case_text)
            
                
            #Test if image arrays are the same
            with self.subTest():
                np.testing.assert_array_equal(output.image,  cs['result'].image, verbose=True,err_msg='Image arrays do not match in '+case_text)
           
            #Test if image metadata are the same
            with self.subTest():
                self.assertDictEqual(output.metadata,  cs['result'].metadata, msg='Image metadata do not match in '+case_text)
            
            #Test if image database are the same
            with self.subTest():
                self.assertDictEqual(output.database,  cs['result'].database, msg='Image databases do not match in '+case_text) 


    '''
    def test_analyze_error(self):
        

  
        for idx, cs in enumerate(assets_core.threshold_adaptive_case_list):
            
            output_array=segmentation.threshold_adaptive(cs['input'], method=cs['method'], blocksize=cs['blocksize'], offset=cs['offset'])
            result_array=cs['result']
            

            case_text="Case "+str(idx)+" with method="+str(cs['method'])+" blocksize="+str(cs['blocksize'])+" offset="+str(cs['offset'])
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output_array.dtype,  result_array.dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            with self.subTest():
                np.testing.assert_array_equal(output_array,result_array, verbose=True,err_msg=case_text)
        
    '''        
    
    

if __name__ == '__main__':
    
    #Ansi Escap characers:
    ANSI_DICT={'close':'\u001b[0m', 'open':u'', 'underlined':'\u001b[4m', 'red':'\u001b[31m', 'green':'\u001b[32m'}
    

    
  
    def tag_image(img):
        
        tagged_array=segmentation.tag_image(img.get_3d_array())
    
        return VividImage(tagged_array, copy.deepcopy(img.metadata ))


    

    
    #analyzed_image =core.analyze(assets_core.analyze_input_5, meas_list=assets_core.intensity_descriptors)
    
    analyzed_image =core.analyze(assets_core.analyze_input_7, meas_list=assets_core.shape_descriptors+assets_core.intensity_descriptors)
    print('FuckThisshit', analyzed_image.database)
    
    #assets_core.image_1.metadata['Name']=['Image 1']
    #assets_core.image_2.metadata['Name']=['Image 2']
    #analyzed_image =core.analyze(tag_image(assets_core.image_1), img_list=[assets_core.image_1, assets_core.image_2])
    
    #assets_core.image_1.metadata['Name']=['Image 1']
    #assets_core.image_2.metadata['Name']=['Image 2']
    #analyzed_image =core.analyze(tag_image(assets_core.image_1), img_list=[assets_core.image_1, assets_core.image_2], meas_list=assets_core.shape_descriptors+assets_core.intensity_descriptors)
    
    #assets_core.image_1.metadata['Name']=['Image 1']
    #assets_core.image_2.metadata['Name']=['Image 2']
    #analyzed_image =core.analyze(tag_image(assets_core.image_1), img_list=[assets_core.image_1], meas_list=assets_core.shape_descriptors+assets_core.intensity_descriptors)
    

    print((analyzed_image.image, None))
    print('')
    print(analyzed_image.metadata)
    print('')
    print(analyzed_image.database)
    print('')
    print(analyzed_image.image.shape)
    
    
    
    
         

    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nRunning Tests:\n'+ANSI_DICT["close"])

    
    #Redirest stderr to dummy stream
    #Some SimpleITK functions output warnings to stderr
    sys.stderr = io.StringIO()


    #Run cProfiles without saving to files
    #Using cProfile.run('fun()') convention prints full report
    pr=cProfile.Profile()#
    pr.enable()
    
    
    test_suite = unittest.TestLoader().loadTestsFromTestCase(CoreTest)


    unittest.TestResult()
    test_stream=io.StringIO()
    test_result=unittest.TextTestRunner(stream=test_stream, verbosity=4).run(test_suite)
    
    pr.disable()

    #Create stream and generate profile stats report form profile
    s=io.StringIO()
    ps = pstats.Stats(pr, stream=s)
    
    #Format and sort stats report
    #Sort by cumulative time and only show results for functions containing 'tests'
    ps.strip_dirs().sort_stats('cumulative').print_stats('test')
    
    #Print results
    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nTest Results:\n'+ANSI_DICT["close"])
    
    print(test_stream.getvalue())
        
    #Print formatted stats report
    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nProfile Report:\n'+ANSI_DICT["close"])
    print(s.getvalue())


    if test_result.wasSuccessful():
        print(ANSI_DICT["open"]+ANSI_DICT["green"]+ANSI_DICT["underlined"]+'RUN WAS SUCCESSFUL!'+ANSI_DICT["close"])
    else:
        print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'RUN WAS UNSUCCESSFUL!'+ANSI_DICT["close"])
    
    

    