import os
import sys

#Import from module files one directory level over tests directory. Insert dir
#into system path temporarily
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
import segmentation
import core
from ImageClass import ImageClass
import utils
