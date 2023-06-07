import datetime
from flask import Flask, request, jsonify
import cv2
from keras.models import load_model
import numpy as np
import base64
import os

app = Flask(__name__)

# Cargar el modelo
model = load_model('/home/charlen/Dropbox/COMPARTIR_LINUX/PROGRAMACION/inferencia_tensorflow/modelo_v1/keras_model.h5')

# Cargar las etiquetas
with open('/home/charlen/Dropbox/COMPARTIR_LINUX/PROGRAMACION/inferencia_tensorflow/modelo_v1/labels.txt', 'r') as f:
    class_names = f.read().splitlines()

@app.route('/predict', methods=['POST'])
def predict():
    # Obtener los datos de la solicitud POST
    data = request.json
    image_base64 = data['image']
    name = data['name']

    
    # Decodificar la imagen desde base64
    image_data = base64.b64decode(image_base64)

    # solo lectura temporal
    # Guardar la imagen decodificada en un archivo temporal
    file_name = '/tmp/temp_image.jpg'
    #with open(temp_image_path, 'wb') as f:
    #    f.write(image_data)

    # Obtener la fecha y hora actual
    #current_datetime = datetime.datetime.now().strftime("%Y%m%d%H%M%S")

    # Generar el nombre del archivo con la fecha y hora
    #file_name = f"/home/charlen/inferencia/image_{current_datetime}.jpg"

    # Guardar la imagen decodificada en el archivo correspondiente
    with open(file_name, 'wb') as f:
       f.write(image_data)

    # Cargar la imagen
    image = cv2.imread(file_name)

    # Preprocesar la imagen
    image = cv2.resize(image, (224, 224))
    image = image.astype('float32') / 255.0
    image = np.expand_dims(image, axis=0)

    # Realizar la inferencia
    predictions = model.predict(image)

    # Obtener la clase con mayor probabilidad
    class_index = np.argmax(predictions[0])

    # Obtener el nombre de la clase
    class_name = class_names[class_index]

    # Obtener la probabilidad de la clase predicha
    confidence = predictions[0][class_index]

    # Eliminar el archivo temporal
   # os.remove(temp_image_path)

    # Preparar la respuesta JSON
    response = {
        'name': name,
        'class': class_name,
        'confidence': float(confidence)
    }

    return jsonify(response)

if __name__ == '__main__':
    app.run(host='192.168.2.24',debug=True,port=5700)