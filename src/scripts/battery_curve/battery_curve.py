from pprint import pprint
import matplotlib.pyplot as plt

DATA_PATH = './adc_val.csv'
MAX_CAPACITY = 200.0 # maH
MAX_VOLTAGE = 4100
MIN_VOLTAGE = 3200
LOOKUP_TABLE_SIZE = MAX_VOLTAGE - MIN_VOLTAGE

def plot_battery(x, y, x_label, y_label, label):
    _, ax = plt.subplots(figsize=(5, 2.7), layout='constrained')
    ax.plot(x, y, label=label)
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)
    ax.set_title("Battery data")
    ax.legend()
    plt.savefig(label + '.png')

def generate_lookup_table():
    '''
    Use the data from output.csv to compute this coefficients using
    curve.fit
    '''
    a = 1.552e-04
    b = -9.850e-01
    c = 1.564e+03
  
    voltages = [x for x in range(MAX_VOLTAGE, MIN_VOLTAGE, -1)]
    func = lambda x : a * pow(x, 2) + b * x + c
    percentages = list(map(lambda x : 100 if x > 100.0 else int(round(x, 0)), list(map(func, voltages))))
   
    # Copy paste this into battery.h
    # In the future, it would be nice if this could be done automatically 
    with open('battery_lookup_table', 'w') as f:
        type = 'const uint8_t lookup_table[LOOKUP_TABLE_SIZE] = '
        type += '{'
        f.write(type)
        for i in range(0, LOOKUP_TABLE_SIZE):
            f.write(str(percentages[i]))
            if i != LOOKUP_TABLE_SIZE - 1:
                f.write(", ")
        f.write("};\n")

if __name__ == "__main__":
  
    voltage_list = [] 
    with open(DATA_PATH, "r") as f:
        for line in f:
            line_tuple = line.strip('\n').split(',')
            timestamp = int(line_tuple[0])
            voltage = int(line_tuple[1].strip())
            
            # Drop readings with timestamp delta < 30s
            if voltage >= 4030:
                continue
            
            voltage_list.append((timestamp, voltage))
       
    voltage_list = sorted(voltage_list)
   
    initial_timestamp = voltage_list[0][0]
    timestamps = [(x - initial_timestamp) / 1000.0 for x, _ in voltage_list]
    voltages = [y for _, y in voltage_list]
    capacity_percent = []
   
    current_between_samples = MAX_CAPACITY / len(timestamps) 
    for i in range(len(timestamps)):
        percent = (MAX_CAPACITY - current_between_samples * i) / MAX_CAPACITY
        capacity_percent.append(round(percent * 100, 3))
       
    plot_battery(voltages, capacity_percent, "mV", "battery%", "Voltage to capacity")
    plot_battery(timestamps, voltages, "time(s)", "mV", "Voltage over time")

    with open('output.csv', 'w') as f:
        for v, p in zip(voltages, capacity_percent):
            f.write(f'{v}, {p}\n')
            
    generate_lookup_table()