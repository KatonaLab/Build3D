import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.utils import error
from modules.a3dc_modules.a3dc.multidimimage import to_multidimimage
 
def module_main(ctx):
    
    try: 
        
        a3.outputs['ChA Image']=to_multidimimage(a3.inputs['ChA Image'])
        a3.outputs['ChB Image']=to_multidimimage(a3.inputs['ChB Image'])
        a3.outputs['ChA Thresholded']=to_multidimimage(a3.inputs['ChA Thresholded'])
        a3.outputs['ChB Thresholded']=to_multidimimage(a3.inputs['ChB Thresholded'])
        a3.outputs['ChA Analyzed']=to_multidimimage(a3.inputs['ChA Analyzed'])
        a3.outputs['ChB Analyzed']=to_multidimimage(a3.inputs['ChB Analyzed'])
        a3.outputs['Overlapping Image']=to_multidimimage(a3.inputs['Overlapping Image'])


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
        a3.Output('ChA Thresholded', a3.types.ImageFloat),
        a3.Output('ChB Thresholded', a3.types.ImageFloat),
        a3.Output('ChA Analyzed', a3.types.ImageFloat),
        a3.Output('ChB Analyzed', a3.types.ImageFloat),
        a3.Output('Overlapping Image', a3.types.ImageFloat)] 
    
    
    return config

a3.def_process_module(generate_config(), module_main)
