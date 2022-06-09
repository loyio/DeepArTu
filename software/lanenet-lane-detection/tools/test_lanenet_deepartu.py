#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author  : Loyio
# @Site    : https://github.com/loyio/DeepArTu
# @File    : test_lanenet.py
# @IDE: Neovim
"""
Evaluate lanenet model on deepartu lane dataset
"""
import argparse
import os
import os.path as ops
import time

import cv2
import numpy as np
import tensorflow as tf
import tqdm
import sys

BASE_DIR = ops.dirname(ops.dirname(ops.abspath("gitRepo/lanenet-lane-detection")))
sys.path.append(BASE_DIR)

from lanenet_model import lanenet
from lanenet_model import lanenet_postprocess
from local_utils.config_utils import parse_config_utils
from local_utils.log_util import init_logger

CFG = parse_config_utils.lanenet_cfg
LOG = init_logger.get_logger(log_file_name_prefix='lanenet_eval')


def init_args():
    """

    :return:
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--src_video', type=str, help='The source deepartu lane test data')
    parser.add_argument('--weights_path', type=str, help='The model weights path')
    parser.add_argument('--with_lane_fit', type=args_str2bool, help='If need to do lane fit', default=True)

    return parser.parse_args()


def args_str2bool(arg_value):
    """

    :param arg_value:
    :return:
    """
    if arg_value.lower() in ('yes', 'true', 't', 'y', '1'):
        return True

    elif arg_value.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Unsupported value encountered.')


def minmax_scale(input_arr):
    """

    :param input_arr:
    :return:
    """
    min_val = np.min(input_arr)
    max_val = np.max(input_arr)

    output_arr = (input_arr - min_val) * 255.0 / (max_val - min_val)

    return output_arr



def eval_lanenet(src_video, weights_path, with_lane_fit=True):
    """

    :param src_dir:
    :param weights_path:
    :param with_lane_fit:
    :param save_dir:
    :return:
    """
    assert ops.exists(src_video), '{:s} not exist'.format(src_video)

    input_tensor = tf.compat.v1.placeholder(dtype=tf.float32, shape=[1, 256, 512, 3], name='input_tensor')

    net = lanenet.LaneNet(phase='test', cfg=CFG)
    binary_seg_ret, instance_seg_ret = net.inference(input_tensor=input_tensor, name='LaneNet')

    postprocessor = lanenet_postprocess.LaneNetPostProcessor(cfg=CFG)

    cap = cv2.VideoCapture(src_video)
    fourcc = cv2.VideoWriter_fourcc(*'X264')
    # out = cv2.VideoWriter('output-src.mp4', fourcc, 29.97, (1280, 720))
    out = cv2.VideoWriter('output-mask.mp4', fourcc, 29.97, (512, 256))

    saver = tf.compat.v1.train.Saver()

    sess = tf.compat.v1.Session()

    with sess.as_default():

        saver.restore(sess=sess, save_path=weights_path)

        
        while(cap.isOpened()):

            ret, image = cap.read()
            image_vis = image
            image_res = cv2.resize(image, (512, 256), interpolation=cv2.INTER_LINEAR)
            image = image_res / 127.5 - 1.0



            binary_seg_image, instance_seg_image = sess.run(
                [binary_seg_ret, instance_seg_ret],
                feed_dict={input_tensor: [image]}
            )

            postprocess_result = postprocessor.postprocess(
                binary_seg_result=binary_seg_image[0],
                instance_seg_result=instance_seg_image[0],
                source_image=image_vis,
                with_lane_fit=with_lane_fit,
                data_source='tusimple'
            )
            mask_image = postprocess_result['mask_image']
            if with_lane_fit:
                lane_params = postprocess_result['fit_params']
                LOG.info('Model have fitted {:d} lanes'.format(len(lane_params)))
                for i in range(len(lane_params)):
                    LOG.info('Fitted 2-order lane {:d} curve param: {}'.format(i+1, lane_params[i]))

            for i in range(CFG.MODEL.EMBEDDING_FEATS_DIMS):
                instance_seg_image[0][:, :, i] = minmax_scale(instance_seg_image[0][:, :, i])
            embedding_image = np.array(instance_seg_image[0], np.uint8)
            binary_image = np.array(binary_seg_image[0], np.uint8)

            # print("binary_image: ",binary_image.shape)
            # print("image: ",image.shape)
            # print("embedding_image: ",embedding_image.shape)
            # print("embedding_image[:, :, (2, 1, 0)]: ",embedding_image[:, :, (2, 1, 0)].shape)

            binary_image_rgb = np.empty([256, 512, 3])
            black_image = np.zeros([256, 512, 3], np.uint8)
            for i in range(binary_image.shape[0]):
                for j in range(binary_image.shape[1]):
                    bint = binary_image[i][j]
                    binary_image_rgb[i][j] = [bint, bint, bint]

            binary_image_rgb = np.array(binary_image_rgb, np.uint8)


            # print("binary_image_rgb: ", binary_image_rgb.shape)
            # print("mask_image: ", mask_image.shape)

            # h_frame = np.vstack((image, embedding_image[:, :, (2, 1, 0)]))
            # hh_frame = np.hstack((h_frame, binary_image*255))
            # hh_frame = np.array(h_frame, np.uint8)

            # cv2.imshow("frame", hh_frame)

            if mask_image is None : 
                img_show = black_image
            else :
                img_show = mask_image[:, :, (2, 1, 0)]

            out.write(img_show)


            # cv2.imshow("Frame", image)
            # cv2.imshow("frame", embedding_image[:, :, (2, 1, 0)])
            # cv2.imshow("frame", binary_image * 255)
            # cv2.imshow("frame", mask_image[:, :, (2, 1, 0)])
            # cv2.imshow("frame", image_vis[:, :, (2, 1, 0)])
            
            
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        # out.release()
        cv2.destroyAllWindows()

    return


if __name__ == '__main__':
    """
    test code
    """
    # init args
    args = init_args()

    eval_lanenet(
        src_video=args.src_video,
        weights_path=args.weights_path,
        with_lane_fit=args.with_lane_fit
    )
