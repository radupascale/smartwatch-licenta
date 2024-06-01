import json
import random
import time
from datetime import datetime

from flask import (Flask, Response, render_template, request,
                   stream_with_context)

application = Flask(__name__)
random.seed()  # Initialize the random number generator
IP = '192.168.0.102'
PORT = 5000

OUTPUT_FILE = ""
OUTPUT_DIR = "accel_data"

@application.route('/')
def index():
    return render_template('index.html')

# client sends data
bmi_data = []
@application.route('/send_data', methods=['POST'])
def send_data():
    recv_data = request.json
    bmi_data.append(recv_data) 
    outFile = open(OUTPUT_FILE, '+a')
    outFile.write(json.dumps(recv_data) + "\n")
    outFile.close()
    return 'OK', 200

def read_bmi_data():
    while True:
        if bmi_data:
            json_data = json.dumps(bmi_data.pop(0))
            yield f"data:{json_data}\n\n"

@application.route('/chart-data')
def chart_data():

    response = Response(read_bmi_data(), mimetype="text/event-stream")
    response.headers["Cache-Control"] = "no-cache"
    response.headers["X-Accel-Buffering"] = "no"
    return response



if __name__ == '__main__':
    # Unique name accel_stream + date_and_time
    OUTPUT_FILE = OUTPUT_DIR + "/" + datetime.now().strftime("%H%M%S%d%m%Y") + ".json"
    
    application.run(host=IP, port=PORT, debug=True)
