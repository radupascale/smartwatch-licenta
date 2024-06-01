import json
import sys
from pprint import pprint

import matplotlib.pyplot as plt

COEFFICIENTS_LOW_0_HZ = {
    'alpha': [1, -1.979133761292768, 0.979521463540373],
    'beta':  [0.000086384997973502, 0.000172769995947004, 0.000086384997973502]
}


def filter(data: list, coefficients: dict):
    '''
    Applies an IIR filter over a time series stream (note the 0 initialization)

    '''
    filtered_data = [0, 0]
    alpha = coefficients['alpha']
    beta = coefficients['beta']
    for i in range(2, len(data)):
        output = alpha[0] * (data[i] * beta[0] + 
                             data[i-1] * beta[1] + 
                             data[i-2] * beta[2] -
                             filtered_data[i-1] * alpha[1] -
                             filtered_data[i-2] * alpha[2])
        filtered_data.append(output)
        
    return filtered_data


def parse_accel_data(data: list):
    '''
    Parses raw accelerometer data to a standard format and applies a 
    0Hz low pass filter to the data to separate user acceleration
    from gravitational acceleration.
    
    Parameters:
    - data: list of tuples of (x, y, z)
    
    Returns:
    List of tuples of tuples of (x, y, z). The first item is
    user acceleration and the second item is gravitational acceleration.
    '''
    x_series, y_series, z_series = map(list, zip(*data))
    x_filtered = filter(x_series, COEFFICIENTS_LOW_0_HZ)
    y_filtered = filter(y_series, COEFFICIENTS_LOW_0_HZ)
    z_filtered = filter(z_series, COEFFICIENTS_LOW_0_HZ)
 
    gravity_accel = list(zip(x_filtered, y_filtered, z_filtered))
    user_accel_x = map(lambda x, y: x - y, x_series, x_filtered)
    user_accel_y = map(lambda x, y: x - y, y_series, y_filtered)
    user_accel_z = map(lambda x, y: x - y, z_series, z_filtered)
    user_accel = list(zip(user_accel_x, user_accel_y, user_accel_z))
    
    return (gravity_accel, user_accel)
    


def plot_accel_data(data: dict):
    '''
    Opens an interactive plot of the data

    Parameters:
    - data: A dictionary with timestamps and x, y, z time series

    Returns:
    None
    '''
    timestamps = data['timestamps']
    x_series = data['x']
    y_series = data['y']
    z_series = data['z']

    fig, ax = plt.subplots(figsize=(5, 2.7), layout='constrained')
    ax.plot(timestamps, x_series, label='x_acceleration')
    ax.plot(timestamps, y_series, label='y_acceleration')
    ax.plot(timestamps, z_series, label='z_acceleration')
    ax.set_xlabel('time(s)')
    ax.set_ylabel('acceleration m/s^2')
    ax.set_title("Accelerometer data")  # Add a title to the Axes.
    ax.legend()  # Add a legend.
    plt.show()


def read_accel_from_file(file_name: str):
    '''
    Parameters:
    - file_name: Path to the JSON with accelerometer data to be read

    Returns: 
    '''
    with open(file_name, "r") as f:
        data = f.read()
        lines = data.split("\n")

        # Create a new plot
        timestamps = []
        x_series = []
        y_series = []
        z_series = []
        first_timestamp = None
        for line in lines:
            try:
                line = json.loads(line)
                # convert to seconds from microseconds
                timestamp = float(line['timestamp']) * 1e-6
                
                if timestamp == 0:
                    continue
                
                if first_timestamp is None:
                    first_timestamp = timestamp

                # truncate to two decimals
                x = round(line['x'], 2)
                y = round(line['y'], 2)
                z = round(line['z'], 2)

                timestamps.append((round(timestamp - first_timestamp, 5)))
                x_series.append(x)
                y_series.append(y)
                z_series.append(z)
            except json.JSONDecodeError:
                continue
    return {"timestamps": timestamps, "x": x_series, "y": y_series, "z": z_series}

def create_data_dict(x, y, z, timestamps):
    '''
    Parameters:
    - x: list of x_acceleration
    - y: list of y_acceleration
    - z: list of z_acceleration
    - timestamps: list of timestamps

    Returns:
    A dictionary with the data
    '''
    return {"timestamps": timestamps, "x": x, "y": y, "z": z}

if __name__ == "__main__":
    if (len(sys.argv) != 2):
        print("Usage: python ./graph_data.py $PATH_TO_ACCEL_JSON")
        exit(-1)

    raw_accel_data = read_accel_from_file(sys.argv[1])
    raw_accel_data_list = [(x, y, z) for x, y, z in zip(raw_accel_data['x'], raw_accel_data['y'], raw_accel_data['z'])]
    gravity_accel, user_accel = parse_accel_data(raw_accel_data_list)

    x_series, y_series, z_series = map(list, zip(*gravity_accel))
    plot_accel_data(create_data_dict(x_series, y_series, z_series, raw_accel_data['timestamps']))
    x_series, y_series, z_series = map(list, zip(*user_accel))
    plot_accel_data(create_data_dict(x_series, y_series, z_series, raw_accel_data['timestamps']))
    plot_accel_data(raw_accel_data)
