import json
import cv2
import numpy as np
from keras.models import load_model
from keras.preprocessing import image

# Carga el modelo pre-entrenado desde un archivo .h5
model = load_model('/home/charlen/Dropbox/COMPARTIR_LINUX/PROGRAMACION/inferencia_tensorflow/modelo_v1/keras_model.h5')

# Función para preprocesar la imagen antes de la inferencia
def preprocess_image(img_path):
    image = cv2.imread(img_path)

    # Preprocesar la imagen
    image = cv2.resize(image, (224, 224))
    image = image.astype('float32') / 255.0
    image = np.expand_dims(image, axis=0)
    return image

# Función para realizar la inferencia de clasificación de la imagen
def classify_image(img_path):
    img = preprocess_image(img_path)
    prediction = model.predict(img)
    class_index = np.argmax(prediction)
    class_label = 'lata'  # Reemplaza esto con las etiquetas de clase correspondientes a tu modelo
    confidence = float(prediction[0, class_index])
    result = {'class_label': class_label, 'confidence': confidence}
    return result

# Ruta de la imagen de entrada
image_path = '/home/charlen/Dropbox/COMPARTIR_LINUX/PROGRAMACION/seminario_chatgpt/lata.jpg'  # Reemplaza esto con la ruta de tu imagen

# Realiza la clasificación de la imagen
classification_result = classify_image(image_path)

# Convierte el resultado a formato JSON
json_result = json.dumps(classification_result)

# Imprime el resultado en formato JSON
print(json_result)
