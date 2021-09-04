import engineio
from flask import Flask, render_template
from flask_socketio import SocketIO
from flask_serial import Serial
from datetime import datetime

import json
import socketio
import os

app = Flask(__name__)

## Configuration Serial GW
serial_port = '/dev/ttyUSB0'                    # Modificar aquí el serial del Arduino, lo dice el IDE
app.config['SERIAL_PORT'] = serial_port
app.config['SERIAL_TIMEOUT'] = 0.1
app.config['SERIAL_BAUDRATE'] = 115200
app.config['SERIAL_BYTESIZE'] = 8
app.config['SERIAL_PARITY'] = 'N'
app.config['SERIAL_STOPBITS'] = 1

socketio = SocketIO(app, async_mode="gevent", ping_timeout=5, logger=False, engineio_logger=False)
ser = Serial(app)

#
#   Enpoints
#

# Endpoint home page
@app.route("/")
def index():
    # Comprobamos que el serial existe
    if os.path.exists(serial_port):
        serial_exists = True
    else:
        serial_exists = False

    return render_template("index.html", serial_port=serial_port, serial_exists=serial_exists)

# Endpoint documentation page
@app.route("/doc")
def doc():
    return render_template("doc.html")

#
#   SocketIO
#

# Handler SocketIO [messages of web]
@socketio.on('send')
def handle_send(json_str):
    data = json.loads(json_str)["message"]
    print("Boton pulsado: {}".format(data))

    # Enviamos por serial
    try:
        ser.on_send(data)
    except Exception as ex:
        print("Error on serial: {}".format(ex))

    # Enviamos en el socket 'receive_message' [hacer en recepcion serial]
    now = datetime.now()
    msg = "{} >> Comando a gateway: {}".format(now.strftime("%H:%M:%S"), data)
    socketio.emit("receive_message", data={"message":str(msg)})

#
#   Flask-Serial
#

# Handler incoming serial messages
@ser.on_message()
def handle_message(msg):
    try:
        print("receive message: {}".format(msg))
        now = datetime.now()
        msg = "{} >> Respuesta: {}".format(now.strftime("%H:%M:%S"), msg.decode("utf-8").strip('\n'))
        print("Send socket msg: {}".format(msg))
        socketio.emit("receive_message", data={"message":str(msg)})
    except Exception as ex:
        print(ex)

# Log thread serial
@ser.on_log()
def handle_logging(level, info):
    print(level, info)

if __name__ == '__main__':
    socketio.run(app, debug=False, log_output=True)