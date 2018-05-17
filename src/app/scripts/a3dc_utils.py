import a3dc
import numpy as np


def multi_dim_image_to_numpy_array(image):
    """
    Returns a numpy array with the copied data of the a3dc.MultiDimImage
    argument 'image'.
    """
    out = np.zeros(image.dims())
    for i, plane in enumerate(multi_dim_image_plane_iterator(image)):
        np.copyto(out[:, :, i], plane)

    return out


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
