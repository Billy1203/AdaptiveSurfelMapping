import numpy as np
import cv2
import os


def image_filter(img, sem, target):
    assert img.shape == sem.shape, "image and sem in different shape."
    assert target.shape[0] == 3, "target sem should in 3 channel RGB format."

    mask_list = []
    for i in (sem == target):
        for j in i:
            if j[0] and j[1] and j[2]:  # all is true, remove the pixels
                mask_list.append(0)
            else:
                mask_list.append(1)
    mask_img = np.array(mask_list, dtype=np.uint8).reshape(img.shape[0], img.shape[1])
    mask_img2 = cv2.cvtColor(mask_img * 255, cv2.COLOR_GRAY2BGR)
    img2 = cv2.bitwise_and(img, mask_img2)
    return img2

if __name__=='__main__':
    carla_dataset_path = "/home/yiheng/dataset/carla/carla_scene2_highway/"
    for image_dic in ["image_2", "image_3", "image_val_0", "image_val_1"]:
        sem_dic = image_dic + "_semantic"

        target_color = np.array([180, 130, 70])  # sky

        filtered_dic = image_dic + "_filtered"
        if not os.path.exists(carla_dataset_path+filtered_dic):
            os.mkdir(carla_dataset_path+filtered_dic)
            print("filtered dic built.")
        else:
            print("filtered dic exist.")

        image_list = os.listdir(carla_dataset_path+image_dic)

        for _image_name in image_list:
            _image_path = carla_dataset_path + image_dic + '/' + _image_name
            _sem_path = carla_dataset_path + sem_dic + '/' + _image_name
            _filtered_path = carla_dataset_path + filtered_dic + '/' + _image_name

            img = cv2.imread(_image_path)
            sem = cv2.imread(_sem_path)

            filtered_img = image_filter(img, sem, target_color)

            cv2.imwrite(_filtered_path, filtered_img)

        print("%s finished." % image_dic)






