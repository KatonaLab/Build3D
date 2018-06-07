import a3dc_module_interface as a3
import numpy as np


def multi_dim_image_plane_iterator(image):
    """
    Returns a generator object can iterate through the planes of the 'image'.
    """
    for i in range(image.dims()[2]):
        non_copied_slice = np.array(image.plane([i]), copy=False)
        yield non_copied_slice


def multi_dim_image_apply(image, plane_func):
    """
    Applies 'plane_func' to all the planes of the 'image', treating one plane
    at a time. 'plane_func' should be a function with the signature
    plane_func(plane) -> None and should manupulate the given plane in-place.
    No copy of the 'image' data is made. This function has no return object,
    manipulates 'image' in-place.
    """
    for plane in multi_dim_image_plane_iterator(image):
        plane_func(plane)
