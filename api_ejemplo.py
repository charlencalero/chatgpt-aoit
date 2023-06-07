from flask import Flask, jsonify

app = Flask(__name__)

@app.route('/')
def hello_world():
    return jsonify({'message': '¡Hola, mundo!'})

@app.route('/api/data')
def get_data():
    data = {'name': 'John Doe', 'age': 30, 'city': 'Example City'}
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)
