import random
import requests
import time
from datetime import datetime

while True:
    # post data with timestamp
    data = {'time': datetime.now().strftime('%H:%M:%S'),'x': random.random() * 100, 'y': random.random() * 100, 'z': random.random() * 100}
    response = requests.post('http://192.168.0.102:5000/send_data', json=data)
    time.sleep(1) 