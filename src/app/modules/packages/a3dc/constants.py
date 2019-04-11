
SHAPE_DESCRIPTORS = {'volume': 'The size of the object in physical units. It is equal to the NumberOfPixels multiplied by the physical pixel size eg. um\u00B3',
                                 'voxelCount': 'The size of object in voxels (number of object voxels)',
                                 'centroid': 'The position of the center of the shape in physical coordinates (order:Z,Y,X). Calculated as the average of the pixel coordinates in the 3 dimensions. It is not constrained to be in the object, and thus can be outside if the object is not convex.',
                                 'ellipsoidDiameter': 'The diameter of the ellipsoid of the same size and the same ratio on all the axes than the label object. Measured in physical units.',
                                 'ellipsoidPerimeter': 'The physical size of the perimiter of an ellipsoid  of the same size and the same ratio on all the axes than the label object. In 2D, it is a distance, in 3D, a surface etc.',
                                 'boundingBox': 'The parameters of the bounding box of the object. THe bounding box is a cuboid that contains the objest.',
                                 'pixelsOnBorder': 'The number of pixels in the objects which are on the border of the image.',
                                 'elongation': 'Elongation is the  ratio of the largest principal moment to the second largest principal moment.',
                                 'equivalentSphericalRadius': 'The equivalent radius in physical units of the hypersphere of the same size than the label object.',
                                 'flatness': 'The ratio between the largest and smallest principal moments of the object.',
                                 'principalAxes': 'The principal axes of the object.',
                                 'principalMoments': 'The principal moments of the object.',
                                 'roundness': 'Measure of the sphericity of object. Value is between 0 and 1 (1 if the object is a perfect sphere).',
                                 'feretDiameter': 'The diameter in physical units of the sphere which contains the object.',
                                 'perimeter': 'The physical size of the perimiter of the objects of the image. In 2D, it is a distance, in 3D, a surface etc.',
                                 'perimeterOnBorder': 'The physical size of the perimiter of the objects which are on the border of the image. In 2D, it is a distance, in 3D, a surface etc.',
                                 'perimeterOnBorderRatio': 'Ratio of the perimiter of an object on the border and the perimeter of an object .',
                                 'equivalentSphericalPerimeter': 'The equivalent perimeter in physical units of the hypersphere of the same size than the label object. In 2D, it is a distance, in 3D, a surface etc.',
                                 'tag': 'The tag of the immage. Output images contain the object map where object pixel intensity is set to the tag value.'}

INTENSITY_DESCRIPTORS = {'meanIntensity': 'Mean grey scale intensity within objects.',
                               'medianIntensity': 'Median of the grey scale intensity values within objects.',
                               'skewness': 'Skewness of the grey scale intensity values within objects.',
                               'kurtosis': 'Kurtosis of the grey scale intensity values within objects.',
                               'variance': 'Variance of the grey scale intensity values within objects.',
                               'maximumPixel': 'Position of the pixel with the highest intensity within objects.',
                               'maximumValue': 'Maximum of the grey scale intensity values within objects.',
                               'minimumValue': 'Minimum of the grey scale intensity values within objects.',
                               'minimumPixel': 'Median of the grey scale intensity values within objects.',
                               'centerOfMass': 'The position of the center of mass of the shape in physical coordinates (order:Z,Y,X). Calculated as the wighted average (intensity as weight factor) of the pixel coordinates in the 3 dimensions. It is not constrained to be in the object, and thus can be outside if the object is not convex.',
                               'standardDeviation': 'Standard deviation of the grey scale intensity values within objects.',
                               'cumulativeIntensity':'Sum of the grey scale intensity values within objects.',
                               'getWeightedElongation': 'Weighted elongation is the  ratio of the largest principal moment to the second largest principal moment.',
                               'getWeightedFlatness': 'The ratio between the largest and smallest principal moments of the object using image intensity as weights.',
                               'getWeightedPrincipalAxes': 'The principal axes of the object using image intensity as weights.',
                               'getWeightedPrincipalMoments': 'The principal moments of the object using image intensity as weights.'}


OTHER_DESCRIPTORS={'filter':'False if object has been filtered out and True if object is accepted.',
                   'object':'List of object taggs that ovrelapp in the given channel.',
                   'overlappingRatio':'The ratio of the volume of the overlapping region to the total volume of the object',
                   'totalOverlappingRatio':'The ratio of the sum of the volumes that overlapping regions to the total volume of the object',
                   'colocalizationCount':'Number of objects colocalizing with the given channel.'}

NUMERIC_DTYPES=['int', 'float', 'bool', 'complex', 'int_','intc', 'intp', 'int8' ,'int16' ,'int32' ,'int64',
                'uint8' ,'uint16' ,'uint32' ,'uint64' ,'float_' ,'float16' ,'float32' ,'float64' ,'complex_' ,'complex64' ,'complex128' ]


    
