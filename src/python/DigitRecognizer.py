import cv2
from keras.models import load_model
import numpy as np

model = load_model("Resource/test_model.h5")

def check_digit():
    image = cv2.imread("Digit.png")
    img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    img = np.vstack(img).reshape(-1, 28, 28, 1)
    img = img.astype('float32')
    img /= 255
    result = model.predict(img)
    print(result[0][result.argmax()])
    if result[0][result.argmax()] > 0.7:
        return result.argmax()
    else:
        return 0
