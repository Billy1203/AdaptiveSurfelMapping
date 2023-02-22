import load_map
import sys


if __name__=='__main__':
    dataset_path = sys.argv[1]
    model_path = sys.argv[2]
    param1 = float(int(sys.argv[3])/100)  # d, 0.05-0.45
    param2 = float(int(sys.argv[4])/10)  # r, 0-5.0
    print("++++param1 is %.2f++++param2 is %.1f" % (param1, param2))

    load_map.loading_map(dataset_path, model_path, param1, param2)
