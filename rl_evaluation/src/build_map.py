import build_map_py
import sys


if __name__=='__main__':
    dataset_path = sys.argv[1]
    model_path = sys.argv[2]
    # param1 = float(int(sys.argv[3])/100)  # d, 0.05-0.45
    param1 = float(sys.argv[3])  # d, 0.05-0.45
    param2 = float(sys.argv[4])  # r, 0-5.0
    depth_dir = sys.argv[5]
    file_name_width = int(sys.argv[6])
    print("++++param1 is %.2f++++param2 is %.1f" % (param1, param2))

    build_map_py.building_map(dataset_path, model_path, param1, param2, depth_dir, file_name_width)

# can run for around 120 frames
