# -*- coding: utf-8 -*-
"""
Created on Fri Jul  6 10:13:24 2018

@author: pongor.csaba
"""

from matplotlib.colors import is_color_like
import warnings
from xml.etree import cElementTree as etree 



class ShapeGroup(object):
    
    __units={'µm': 'micrometer SI unit:default', 
             'Ym':'yottameter SI unit.',
             'Zm':'zettameter SI unit.',
             'Em':'exameter SI unit.',
             'Pm':'petameter SI unit.',
             'Tm':'terameter SI unit.',
             'Gm':'gigameter SI unit.',
             'Mm':'megameter SI unit.',
             'km':'kilometer SI unit.',
             'hm':'hectometer SI unit.',
             'dam':'decameter SI unit.',
             'm':'meter SI unit.',
             'dm':'decimeter SI unit.',
             'cm':'centimeter SI unit.',
             'mm':'millimeter SI unit.',
             'nm':'nanometer SI unit.',
             'pm':'picometer SI unit.',
             'fm':'femtometer SI unit.',
             'am':'attometer SI unit' ,
             'zm':'zeptometer SI unit.',
             'ym':'yoctometer SI unit.',
             'Å':'ångström SI-derived unit.',
             'thou':'thou Imperial unit (or mil, 1/1000 inch).',
             'li':'line Imperial unit (1/12 inch).',
             'in':'inch Imperial unit.',
             'ft':'foot Imperial unit.',
             'yd':'yard Imperial unit.',
             'mi':'terrestrial mile Imperial unit.',
             'ua':'astronomical unit SI-derived unit. The official term is ua '\
                 'as the SI standard assigned AU to absorbance unit.',
             'ly':'light year.',
             'pc':'parsec.',
             'pt':'typography point Imperial-derived unit (1/72 inch). Use of '\
                 'this unit should be limited to font sizes.',
             'pixel':'pixel abstract unit.  This is not convertible to any '\
                 'other length unit without a calibrated scaling factor. Its '\
                 'use should should be limited to ROI objects, and converted '\
                 'to an appropriate length units using the PhysicalSize units '\
                 'of the Image the ROI is attached to reference frame',	
             'reference frame':'abstract unit.  This is not convertible to any '\
                 'other length unit without a scaling factor.  Its use should be '\
                 'limited to uncalibrated stage positions, and converted to an '\
                 'appropriate length unit using a calibrated scaling factor.'}
    
    
    
    __attr__required={'ID','Union'}
    __attr__optional={'FillColor','FillRule','StrokeColor','StrokeWidth','StrokeWidthUnit','StrokeDashArray','Text','FontFamily','FontSize','FontSizeUnit','FontStyle','Locked',' TheT',' TheC',' TheZ'}
    __attr__unsupported={'Transform','AnnotationRef'}  
            
    __shapes_supported={'Rectangle':['Height','Width', 'X','Y'],'Point':['X','Y'],'Ellipse':['RadiusX', 'RadiusY', 'X', 'Y'],'Line':['X1', 'X2','Y1','Y2'],'Polyline':['Points'],'Polygon':['Points']}       
    __shapes_unsupported={'Mask':['Height','Width', 'X','Y'],'Label':['X','Y']}      
            
            
    def __init__(self, input_dict):
        
        self.__attributes={}
        missing=(set(self.__attr__required)-set(input_dict.keys()))
        if len(missing)==0:
            for key in input_dict:
              
                if isinstance(getattr(self.__class__, key, None), property):
                    self.__attributes[key]=input_dict[key]
        
        else:
          raise RuntimeError('ShapeGroup object is missing the following attributes: '+str(missing))  
                
        
            
            
        
    def __setattr__(self, attribute, value):
        
        property_obj = getattr(self.__class__, attribute, None)
        #If attribute is property use setter, otherwise use standard method
        if isinstance(property_obj, property):
            if property_obj.fset is None:
                raise AttributeError("can't set attribute")
            property_obj.fset(self, value)
        else:
            super(ShapeGroup, self).__setattr__(attribute, value)    
   
    ##########################################################################   
    @property
    def Union(self):
        '''
        The shape element contains a single specific ROI shape and links
        that to any channels, and a timepoint and a z-section.
        '''
        return self.__attributes['Union']

    @Union.setter
    def Union(self, value):
       
        if isinstance(value, dict):
 ####################################################################################### 
 #######################################################################################
            '''    
            #Check if Shappe is unsupported
            if len(set(value.keys()) or set(self.__shapes_unsupported.keys()))>0:
                print(value)
                print(set(self.__shapes_unsupported.keys())-set(value.keys()) )
                raise Exception('Shape has to be a dictionary!')
            '''
            #Check if Shape is supported and contains only one element
            common=set(value.keys()) & set(self.__shapes_supported.keys())
           
            #Get sole element from the intersection of value.keys() & self.__shapes.keys()
            element = next(iter(common))
            if list(value[element].keys())==self.__shapes_supported[element]:
                self.__attributes['Union']={'Shape':value}    
        
 
        else:
            raise TypeError('Shape has to be a dictionary!')
                    
            
    def AddShape(self, value):
        if isinstance(value, dict):
            if 'Shape' in self.__attributes['Union'].keys():
            
                if isinstance(self.__attributes['Union']['Shape'], list):
                    self.__attributes['Union']['Shape'].append(value)
                else:
                    self.__attributes['Union']['Shape']=[self.__attributes['Union']['Shape'], value]
            
        else:
            self.__attributes['Union']['Shape']=value
    
    def RemoveShape(self, position=0):
        
        if 'Shape' in self.__attributes['Union'].keys() :
                
            if isinstance(self.__attributes['Union']['Shape'], list):
                if len(self.__attributes['Union']['Shape'])!=1:
                    del self.__attributes['Union']['Shape'][position]
                
                elif len(self.__attributes['Union']['Shape'])==1:
                    self.__attributes['Union']['Shape']=self.__attributes['Shape'][0]
                    raise  Exception('Shape group must contain at least one element!')
                    
            else:
                raise  Exception('Shape group must contain at least one element!')
        
                
          
    #########################################################################
    ##########################################################################   
    @property
    def Transform(self):
        '''
        This is a matrix used to transform the shape. The element has 6 
        float attributes. If the element is present then all 6 values must be
        included.
        '''
        warnings.warn('Transform is currently unsupported!')
        

    @Transform.setter
    def Transform(self, value):

        
        warnings.warn('Transform is currently unsupported!')
            
    ##########################################################################   
    @property
    def AnnotationRef(self):
        '''
        The AnnotationRef element is a reference to an element derived
        from the CommonAnnotation element.
        '''
        warnings.warn('AnnotationRef is currently unsupported!')
        

    @AnnotationRef.setter
    def AnnotationRef(self, value):

        
        warnings.warn('AnnotationRef is currently unsupported!')
    
    
    
    
    ##########################################################################   
    @property
    def FillColor(self):
        '''
        The color of the fill - encoded as RGBA
        The value "-1" is #FFFFFFFF so solid white (it is a signed 32 bit value)
        NOTE: Prior to the 2012-06 schema the default value was incorrect and produced a transparent red not solid white.
        '''
        return self.__attributes['FillColor']

    @FillColor.setter
    def FillColor(self, value):

        if is_color_like(value):
            warnings.warn('Invalid value for attribute FillColor! Has to be RGBA value.....')
            
        self.__attributes['FillColor'] = value
        
    #########################################################################    
    @property
    def FillRule(self):
        """
        The rule used to decide which parts of the shape to fill. 
        """
        return self.__attributes['FillColor']

    @FillRule.setter
    def FillRule(self, value):
        
        values=["EvenOdd","NonZero"]
        
        if (value not in values) or (not isinstance(value, str)):
            warnings.warn('Invalid value for attribute FillRule! Value has to be one of the following: '+str(values))
            
        self.__attributes['FillColor'] = value
        
    #########################################################################       
    @property
    def StrokeColor(self):
        """
        The color of the stroke  - encoded as RGBA
        The value "-1" is #FFFFFFFF so solid white (it is a signed 32 bit value)
        NOTE: Prior to the 2012-06 schema the default value was incorrect and produced a transparent red not solid white.
        """
        return self.__attributes['StrokeColor']

    @StrokeColor.setter
    def StrokeColor(self, value):
                
        if is_color_like(value):
            warnings.warn('Invalid value for attribute StrokeColor! Has to be RGBA value.....')
            
        self.__attributes['StrokeColor'] = value
        
    #########################################################################     
    @property
    def StrokeWidth(self):
        """
        The width of the stroke. Units are set by StrokeWidthUnit.
        """
        return self.__attributes['StrokeWidth']

    @StrokeWidth.setter
    def StrokeWidth(self, value):
        
        if not isinstance(value, (int,float)):
            warnings.warn('Invalid value for attribute StrokeColor! Value has to be int or float...')
            
        self.__attributes['StrokeWidth'] = value
        
    #########################################################################        
    @property
    def StrokeWidthUnit(self):
        """
        The units used for the stroke width.
        """
        return self.__attributes['StrokeWidthUnit']

    @StrokeWidthUnit.setter
    def StrokeWidthUnit(self, value):

        if value not in self.__units.keys():
            warnings.warn('Invalid value for attribute StrokeWidthUnit! Value has to be one of the following:\n'+str(self.__units))
            
        self.__attributes['StrokeWidthUnit'] = value
        
    #########################################################################    
    @property
    def StrokeDashArray(self):
        """
        StrokeDashArray string e.g. "none", "10 20 30 10"
        """
        return self._self.__attributes['StrokeDashArray']

    @StrokeDashArray.setter
    def StrokeDashArray(self, value):
        
        if not isinstance(value, str):
            warnings.warn('Invalid value for attribute StrokeDashArray! Value has to be string e.g. "none", "10 20 30 10"')
            
        self.__attributes['StrokeDashArray'] = value
        
    #########################################################################     
    @property
    def LineCap(self):
        """
        The shape of the end of the line.
        """
        return self._self.__attributes['LineCap']

    @LineCap.setter
    def LineCap(self, value):
        
        values=["Butt","Line","Square"]
        
        if (value not in values) or (not isinstance(value, str)):
            warnings.warn('Invalid value for attribute LineCap! Value has to be a string and one of the following: '+str(values))
            
        self.__attributes['LineCap'] = value
        
    #########################################################################    
    @property
    def Text(self):

        return self._self.__attributes['Text']

    @Text.setter
    def Text(self, value):
        
        if not isinstance(value, str):
            warnings.warn('Invalid value for attribute Text! Value has to be a string...')
            
        self.__attributes['Text'] = value
        
    #########################################################################    
    @property
    def FontFamily(self):
        """
        The font family used to draw the text. Note: these values
        are all lower case so they match the standard HTML/CSS values. 
        "fantasy" has been included for completeness we do not recommended its 
        regular use.
        """
        return self.__attributes['FillColor']

    @FontFamily.setter
    def FontFamily(self, value):
        
        values=["serif","sans-serif","cursive","fantasy","monospace"]
        
        if (value not in values) or (not isinstance(value, str)):
            warnings.warn('Invalid value for attribute FillRule! Value has to be one of the following: '+str(values))
            
        self.__attributes['FillColor'] = value
        
    #########################################################################   
    @property
    def FontSize(self):
        """
        Size of the font. Units are set by FontSizeUnit.
        """
        return self.__attributes['FontSize']

    @FontSize.setter
    def FontSize(self, value):
                
        if (not isinstance(value, int)) or (not value>0):
            warnings.warn('Invalid value for attribute FontSize! Value has to be int and non/negative...')
            
        self.__attributes['FontSize'] = value
        
    ######################################################################### 
    @property
    def FontSizeUnit(self):
        """
        The units used for the font size.
        """
        return self.__attributes['FontSizeUnit']

    @FontSizeUnit.setter
    def FontSizeUnit(self, value):
                
        if value not in self.__units.keys():
            warnings.warn('Invalid value for attribute FontSizeUnit! Value has to be one of the following:\n'+str(self.__units))
            
        self.__attributes['FontSizeUnit'] = value
        
    #########################################################################   
    @property
    def FontStyle(self):
        """
        The style and weight applied to the text. This is a simplified 
        combination of the HTML/CSS attributes font-style AND font-weight.
        """
        return self.__attributes['FontStyle']

    @FontStyle.setter
    def FontStyle(self, value):
                
        values=["Bold","BoldItalic","Italic","Normal"]
        
        if (value not in values) or (not isinstance(value, str)):
            warnings.warn('Invalid value for attribute FontStyle! Value has to be a string and one of the following: '+str(values))
            
        self.__attributes['FontStyle'] = value
        
    #########################################################################    
    @property
    def Visible(self):
        """
        Controls whether the shape is currently visible, true is visible, false is hidden.
        """
        return self._self.__attributes['Visible']

    @Visible.setter
    def Visible(self, value):
        
        if not isinstance(value, bool):
            warnings.warn('Invalid value for attribute Visible! Value has to be boolean')
            
        self._self.__attributes['Visible'] = value
        
    #########################################################################     
    @property
    def Locked(self):
        """
        Controls whether the shape is currently visible, true is visible, false is hidden.
        """
        return self._self.__attributes['Locked']

    @Locked.setter
    def Locked(self, value):
        
        if not isinstance(value, bool):
            warnings.warn('Invalid value for attribute Locked! Value has to be boolean')
            
        self._self.__attributes['Locked'] = value
        
    ######################################################################### 
    @property
    def ID(self):
        """
        Shape ID
        """
        return self._self.__attributes['ID']

    @ID.setter
    def ID(self, value):
        
        #Check is ID is appropriate (eg. "Shape:0:0")
        valid=True
        if isinstance(value, str):
            lines=str(value, 'utf-8').split(":")
            for index, element in enumerate(lines):
                if index==0:
                    if not element=="Shape":
                        valid=False
                else:
                    if not self.__represents_type(element, int):
                        valid=False
        if not valid:
            warnings.warn('Invalid value for attribute ID!')
            
        self._self.__attributes['ID'] = value
        
    #########################################################################     
    @property
    def TheZ(self):
        """
        The z-section the ROI applies to. If not specified then
        the ROI applies to all the z-sections of the image. [units:none]
        This is numbered from 0.
        """
        return self.__attributes['TheZ']

    @TheZ.setter
    def TheZ(self, value):
        
        if (not isinstance(value, int)) or (not value>0):
            warnings.warn('Invalid value for attribute TheZ! Value has to be int and non/negative......')
            
        self.__attributes['TheZ'] = value
        
    #########################################################################       
    @property
    def TheZ(self):
        """
        The z-section the ROI applies to. If not specified then
        the ROI applies to all the z-sections of the image.
        This is numbered from 0.
        """
        return self.__attributes['TheZ']

    @TheZ.setter
    def TheZ(self, value):
        
        if (not isinstance(value, int)) or (not value>0):
            warnings.warn('Invalid value for attribute TheZ! Value has to be int and non/negative......')
            
        self.__attributes['TheZ'] = value
        
    #########################################################################      
    @property
    def TheT(self):
        """
        The timepoint the ROI applies to. If not specified then
        the ROI applies to all the timepoints of the image. 
        This is numbered from 0.
        """
        return self.__attributes['TheZ']

    @TheT.setter
    def TheT(self, value):
        
        if (not isinstance(value, int)) or (not value>0):
            warnings.warn('Invalid value for attribute TheT! Value has to be int and non/negative......')
            
        self.__attributes['TheT'] = value
        
    #########################################################################      
    @property
    def TheC(self):
        """
        The channel the ROI applies to. If not specified then
        the ROI applies to all the channels of the image. 
        This is numbered from 0.
        """
        return self.__attributes['TheC']

    @TheC.setter
    def TheC(self, value):
        
        if (not isinstance(value, int)) or (not value>0):
            warnings.warn('Invalid value for attribute TheC! Value has to be int and non/negative......')
            
        self.__attributes['TheZ'] = value
        
    #########################################################################  
    

    def __represents_type(self, s, atype):
        '''
        Check if string represents type given through atype!
        '''
        try: 
            atype(s)
            return True
        except ValueError:
            return False

    def Xml(self):
            print(self.Union)
           #Create list of shapes
            if isinstance(self.Union['Shape'], list):
                shape_list=self.Union['Shape']
            else:
                shape_list=[self.Union['Shape']]
            
            #Generate Union element
            union_element=etree.Element('Union')
            
            #Cycle through shape list. Create shape elements
            for shape in shape_list:
                
                shape_attrib={str(key):str(shape[key]) for key in shape.keys()}
                shape_element=etree.Element('Shape', shape_attrib)
            
                #Create shape_type element. Shape can only contain one shape type
                shape_type_key=[key for key in shape.keys()][0]
                shape_type_attrib={str(key):str(shape[shape_type_key][key]) for key in shape[shape_type_key].keys()}
                shape_type_element=etree.Element(shape_type_key, shape_type_attrib)
                shape_element.append(shape_type_element)
            
                union_element.append(shape_element)
            
            return union_element
        
if __name__ == "__main__":
    
    #ome_dict={'Shape': {'Ellipse': {'RadiusX': 11.5, 'RadiusY': 15.5, 'X': 20.5, 'Y': 25.5}, 'ID': 'Shape:4:0', 'Text': '0001-0025-0020', 'TheT': 0, 'TheZ': 0}}
    ome_dict={'Union': {'Shape': {'Polygon': {'Points': '34,7 35,7 36,7 36,8 37,8 37,10 38,10 38,11 39,13 39,14 41,17 41,19 42,21 43,23 43,25 43,26 43,27 43,28 43,29 43,31 43,32 43,33 43,34 43,35 43,36 43,37 42,37 42,38 41,38 41,39 40,39 39,39 39,40 39,41 38,41 37,41 37,42 36,42 34,42 32,42 31,42 28,42 25,41 24,41 22,40 20,40 18,39 17,39 16,37 15,37 15,36 14,35 13,34 13,32 13,30 13,29 13,26 13,25 13,24 13,23 13,22 13,21 13,20 14,19 14,18 14,17 15,15 16,15 16,14 17,14 21,13 22,13 24,12 25,12 26,11 27,11 28,11 29,11 29,10 30,10 31,10 32,10 32,9 33,9 33,8 34,8 35,8'}, 'ID': 'Shape:5:0', 'Text': '0001-0024-0028', 'TheT': 0, 'TheZ': 0}}, 'ID': 'ROI:5:0'}
    a=ShapeGroup(ome_dict)
    a.AddShape({'Ellipse': {'RadiusX': 11.5, 'RadiusY': 15.5, 'X': 20.5, 'Y': 25.5}})
    strint=etree.tostring(a.Xml())
    print(str(string))