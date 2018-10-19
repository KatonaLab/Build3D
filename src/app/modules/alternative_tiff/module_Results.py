import a3dc_module_interface as a3
from modules.packages.a3dc.utils import error
from modules.packages.a3dc.core import VividImage
 

def module_main(ctx):
    
    try: 
        
        a3.outputs['ChA Image']=VividImage(a3.inputs['ChA Image'].image, a3.inputs['ChA Image'].metadata).to_multidimimage()
        a3.outputs['ChB Image']=VividImage(a3.inputs['ChB Image'].image, a3.inputs['ChB Image'].metadata).to_multidimimage()
        #a3.outputs['ChA Thresholded']=VividImage(a3.inputs['ChA Thresholded'].image, a3.inputs['ChA Thresholded'].metadata).to_multidimimage()
        #a3.outputs['ChB Thresholded']=VividImage(a3.inputs['ChB Thresholded'].image, a3.inputs['ChB Thresholded'].metadata).to_multidimimage()
        a3.outputs['ChA Analyzed']=VividImage(a3.inputs['ChA Analyzed'].image, a3.inputs['ChA Analyzed'].metadata).to_multidimimage()
        a3.outputs['ChB Analyzed']=VividImage(a3.inputs['ChB Analyzed'].image, a3.inputs['ChB Analyzed'].metadata).to_multidimimage()
        a3.outputs['Overlapping Image']=VividImage(a3.inputs['Overlapping Image'].image, a3.inputs['Overlapping Image'].metadata).to_multidimimage()

    except Exception as e:
        error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !", exception=e)

 
def generate_config():

    #Set Outputs and inputs
    config=[a3.Input('ChA Image', a3.types.GeneralPyType), 
        a3.Input('ChB Image', a3.types.GeneralPyType), 
        a3.Input('ChA Thresholded', a3.types.GeneralPyType),
        a3.Input('ChB Thresholded', a3.types.GeneralPyType),
        a3.Input('ChA Analyzed', a3.types.GeneralPyType),
        a3.Input('ChB Analyzed', a3.types.GeneralPyType),
        a3.Input('Overlapping Image', a3.types.GeneralPyType),
        a3.Output('ChA Image', a3.types.ImageFloat), 
        a3.Output('ChB Image', a3.types.ImageFloat), 
       # a3.Output('ChA Thresholded', a3.types.ImageFloat),
        #a3.Output('ChB Thresholded', a3.types.ImageFloat),
        a3.Output('ChA Analyzed', a3.types.ImageFloat),
        a3.Output('ChB Analyzed', a3.types.ImageFloat),
        a3.Output('Overlapping Image', a3.types.ImageFloat)] 
    
    
    return config

a3.def_process_module(generate_config(), module_main)
