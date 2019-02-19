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
    


    def test_threshold_auto(self):
         
        
        for idx, cs in enumerate(assets_core.threshold_auto_case_list):
            
            case_text="Case "+str(idx+1)+" with method="+str(cs['method'])+" mode="+str(cs['mode'])
            
            output=segmentation.threshold_auto(cs['input'], method=cs['method'], mode=cs['mode'])
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output[0].dtype,  cs['result_array'].dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            

            with self.subTest():
                np.testing.assert_array_equal(output[0], cs['result_array'], verbose=True, err_msg=case_text)
            #Test if thresholds are equal
            with self.subTest():
                self.assertEqual(output[1], cs['result_threshold'], msg='Threshold values do not match '+case_text)                


    def test_threshold_adaptive(self):
        

  
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
        
        
    
    

if __name__ == '__main__':
    
    #Ansi Escap characers:
    ANSI_DICT={'close':'\u001b[0m', 'open':u'', 'underlined':'\u001b[4m', 'red':'\u001b[31m', 'green':'\u001b[32m'}
    
    
    
    keys_1={'PhysicalSizeX':0.1,'PhysicalSizeY':0.2,
                       'PhysicalSizeZ':0.3, 'TimeIncrement':0.4, 
                       'PhysicalSizeXUnit':'test unit x',
                       'PhysicalSizeYUnit':'test unit y',
                       'PhysicalSizeZUnit':'test unit z',
                       'TimeIncrementUnit':'test unit t'}
    
    image_1=VividImage.from_ndarray(assets_core.input_array_1, key_dict=keys_1)

    
    keys_2={'PhysicalSizeX':0.1,'PhysicalSizeY':0.2,
                       'PhysicalSizeZ':0.3, 'TimeIncrement':0.4, 
                       'PhysicalSizeXUnit':'test unit x',
                       'PhysicalSizeYUnit':'test unit y',
                       'PhysicalSizeZUnit':'test unit z',
                       'TimeIncrementUnit':'test unit t'}
    
    image_2=VividImage.from_ndarray(assets_core.input_array_2, key_dict=keys_2)


    keys_3={'PhysicalSizeX':0.1,'PhysicalSizeY':0.2,
                       'PhysicalSizeZ':0.3, 'TimeIncrement':0.4, 
                       'PhysicalSizeXUnit':'test unit x',
                       'PhysicalSizeYUnit':'test unit y',
                       'PhysicalSizeZUnit':'test unit z',
                       'TimeIncrementUnit':'test unit t'}
    
    image_3=VividImage.from_ndarray(assets_core.input_array_3, key_dict=keys_3)

    
   
    image_4=VividImage.from_ndarray(assets_core.input_array_4, key_dict=keys_3) 
    
    image_5=VividImage.from_ndarray(assets_core.input_array_5, key_dict=keys_3) 

    print(image_4)
    #print(image_1.get_dimension(0, dimension='C'))
    image_tagged=segmentation.tag_image(image_5.get_3d_array())
    
    vividimage_tagged=VividImage(image_tagged, copy.deepcopy(image_5.metadata ))
    print(image_tagged)
    print(vividimage_tagged)
    print(vividimage_tagged.image.shape)
    
    analyzed_image =core.analyze(vividimage_tagged, meas_list=assets_core.shape_descriptors+assets_core.intensity_descriptors)
    
    print(analyzed_image.database)
    print(analyzed_image.image)
    print(analyzed_image.metadata)
    print(analyzed_image.database)
    print(analyzed_image.image.shape)
    '''
    print(list([segmentation.overlap_image([input_array_1,input_array_2])]))
    
         

    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nRunning Tests:\n'+ANSI_DICT["close"])

    
    #Redirest stderr to dummy stream
    #Some SimpleITK functions output warnings to stderr
    sys.stderr = io.StringIO()


    #Run cProfiles without saving to files
    #Using cProfile.run('fun()') convention prints full report
    pr=cProfile.Profile()#
    pr.enable()
    
    
    #test_suite = unittest.TestLoader().loadTestsFromTestCase(SegmentationTest)


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
    
    
'''    
    