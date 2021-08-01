from flask import Flask, render_template
from flask_socketio import SocketIO
from datetime import datetime

import json
import socketio


app = Flask(__name__)
socketio = SocketIO(app)

## Configuration Serial GW



## ENDPOINTS
# Endpoint home page
@app.route("/")
def index():

    return render_template("index.html", gw=True)

# Endpoint documentation page
@app.route("/doc")
def doc():

    return render_template("doc.html")

# handler SocketIO
@socketio.on('send')
def handle_send(json_str):
    data = json.loads(json_str)["message"]
    print("boton: {}".format(data))

    # Enviamos en el socket 'receive_message' [hacer en recepcion serial]
    now = datetime.now()
    msg = "{} >> Enviado: {}".format(now.strftime("%H:%M:%S"),data)
    socketio.emit("receive_message", data={"message":str(msg)})

if __name__ == '__main__':
    socketio.run(app, debug=False)