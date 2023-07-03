"""
: Author: Yuning Zhang
: Created time: 2023/05
: Organization: University of Southern California
: Modification Log
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

matplotlib.use('TkAgg')
from matplotlib import pyplot as plt

# path = os.path.abspath('../../')
path = os.path.abspath('../')
if not path in sys.path:
    sys.path.append(path)
import paam_packages
import time
import logging

# Set up logging
log_file_path = 'log.txt'
logging.basicConfig(filename=log_file_path, level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')


def main():
    """

    :return:
    :rtype:
    """
    # Create an argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--node", type=str, default='rfdev4-in2', help="the domain of the PAAM that you want to control")
    parser.add_argument("-t", "--theta", type=int, default=0, help="the direction of azimuth steering")
    parser.add_argument("-p", "--phi", type=int, default=0, help="the direction of elevation steering")
    args = parser.parse_args()

    # Create a PAAM object
    PAAM0 = paam_packages.object.PAAM(args.node)

    # Set up the RF configuration for the PAAM
    theta = args.theta
    phi = args.phi
                 
    # Execute
    print(" ")
    print("**************************************************")
    print("Steering the PAAM board to azimuth: {}, elevation: {}...".format(theta, phi))
    print("**************************************************")

    step = PAAM0.steer(theta, phi)
    log_message = "The step(s) been executed: {}".format(", ".join(step_idx['@name'] for step_idx in step))
    logging.info(log_message)



if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
