# python prepare_data.py -f

import json
import os
from pathlib import Path
import sys
import argparse
import cv2
import numpy as np
import csv
import RotationTransform as rt
import math
import pandas as pd


parser = argparse.ArgumentParser()
parser.add_argument('-f', '--fake', action='store_true', 
                    help='if set, only print operations but not really implement')
parser.add_argument('-s', '--show', action='store_true', 
                    help='if set, show the depth in color map')
args = parser.parse_args()

load_f = open('preparation.json', 'r')
setting = json.load(load_f)
load_f.close()


frames = setting['frames']
if len(frames) % 2 != 0:
    print('[Error] frames should be even')

raw_poses = None
f_pose = None

raw_cali = None


for dir in setting['sensors']:
    in_dir = os.path.join(setting['raw_data_dir'], dir)
    out_dir = os.path.join(setting['dest_dir'], dir)
    if not os.path.exists(out_dir):
        Path(out_dir).mkdir(parents=True, exist_ok=True)
    cnt = 0
    for i in range(len(frames) // 2):
        for j in range(frames[2*i], frames[2*i+1]):
            in_file_name = '%010d.png' % j
            out_file_name = '%010d.png' % cnt
            cnt += 1
            in_file = os.path.join(in_dir, in_file_name)
            out_file = os.path.join(out_dir, out_file_name)
            if j in frames:
                print('cp {} {}'.format(in_file, out_file))
            if args.fake:
                continue

            # poses
            if dir == 'image_2':
                if raw_cali is None:
                    in_cali_file = os.path.join(in_dir, 'camera_info.csv')
                    df = pd.read_csv(in_cali_file)
                    data = df.loc[0]
                    out_cali_file = os.path.join(setting['dest_dir'], 'calibration.txt')
                    with open(out_cali_file, 'w') as f_cali:
                        f_cali.write('{:.2f} {:.2f} {:.2f} {:.2f}\n{:.0f} {:.0f}'.format(data['fx'],
                                                                 data['cx'],
                                                                 data['fy'],
                                                                 data['cy'],
                                                                 int(data['width']),
                                                                 data['height']))
                if raw_poses is None:
                    in_pose_file = os.path.join(in_dir, 'poses.csv')
                    pose_f = open(in_pose_file, 'r')
                    csv_reader = csv.reader(pose_f)
                    raw_poses = list(csv_reader)
                    pose_f.close()
                    out_pose_file = os.path.join(setting['dest_dir'], 'pose.txt')
                    f_pose = open(out_pose_file, 'w')
                    
                for x in raw_poses:
                    if x[0] != 'frame' and int(x[0]) == j:
                        t = [float(x[2]), float(x[3]), float(x[4])]
                        r = rt.eularangle_to_rotmate([math.radians(float(x[5])), math.radians(float(x[6])), math.radians(float(x[7]))])
                        f_pose.write('{} {} {} {} '.format(r[0][0], r[0][1], r[0][2], t[0]))
                        f_pose.write('{} {} {} {} '.format(r[1][0], r[1][1], r[1][2], t[1]))
                        f_pose.write('{} {} {} {}\n'.format(r[2][0], r[2][1], r[2][2], t[2]))
            

            # images
            if "depth" not in dir:
                os.system('cp {} {}'.format(in_file, out_file))
            else:
                depth_bgr = cv2.imread(in_file)
                print(in_file)
                depth_bgr = depth_bgr.astype(np.float64)
                depth = depth_bgr[:,:,2] + depth_bgr[:,:,1]*256 + depth_bgr[:,:,0]*256*256
                depth = depth / (256 * 256 * 256 - 1)
                depth = depth * 1000                        # meter
                depth = np.clip(depth, 0, 60)
                if args.show:
                    depth_show = (depth / 60) * 256
                    depth_show = depth_show.astype(np.uint8)
                    depth_show = cv2.applyColorMap(depth_show, cv2.COLORMAP_AUTUMN)
                    cv2.imshow('depth', depth_show)
                    cv2.waitKey(0)
                    cv2.destroyAllWindows()
                depth = depth * 1000                        # milimeter
                depth = depth.astype(np.uint16)
                cv2.imwrite(out_file, depth)
