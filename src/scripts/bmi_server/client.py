import random
import time
from datetime import datetime

import requests

from bmi_graph import IP, PORT

while True:
    # post data with timestamp
    data = {'timestamp': round(time.time_ns()), 'x': random.random(
    ) * 100, 'y': random.random() * 100, 'z': random.random() * 100}
    response = requests.post(f'http://{IP}:{PORT}/send_data', json=data)
    # sleep for 1/50s
    time.sleep(1.0/50.0)
