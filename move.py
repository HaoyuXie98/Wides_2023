"""
: Modifier: Yuning Zhang
: Modified time: 2023/05
: Modifier Organization: University of Southern California
: Modification Log
:    - Change the rotation angle of the XYtable as constant number, 0 - no need to modify it during the indoor experiment
:    - Hardcode the XYtable index as xytable 2
:    - Update the system abspath layers
"""

# Import Libraries
import os
import sys
import argparse
import numpy as np
import matplotlib
import configparser
import threading
import time
import logging

# Set up logging
log_file_path = 'log.txt'
logging.basicConfig(filename=log_file_path, level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
matplotlib.use('TkAgg')
from matplotlib import pyplot as plt

# path = os.path.abspath('../../')
path = os.path.abspath('../')
if not path in sys.path:
    sys.path.append(path)
import xytable_packages


def main():
    """

    :return:
    :rtype:
    """

    # Create an argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("-X", "--x", type=int, default=0, help="x coordinate on XY table")
    parser.add_argument("-Y", "--y", type=int, default=0, help="y coordinate on XY table")
    parser.add_argument("-C", "--Corner", dest='corner', type=str, default='2', help="the corner of XY table")
    # parser.add_argument("-R", "--r", type=int, default=0, help="rotation angle of the array on XY table")
    args = parser.parse_args()

    # Create an XY-table object
    xytable = xytable_packages.object.XYTable('xytable'+args.corner)

    # Set up the movement parameters
    x = args.x
    y = args.y
    # r = args.r
             
    # Move
    # xytable.move(x, y, r)
    xytable.move(x, y, 0)
    
    xy_status = "Run"
    while xy_status == "Run":
        time.sleep(1)  # Adjust the sleep duration as needed
        xy_status, rotator_status, current_position = xytable.check()
        if xy_status == "Idle":
            logging.info("The current position of 'xytable{}': {}".format(args.corner, current_position))
    


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
