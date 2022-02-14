from serial.tools import list_ports
import serial
import yaml
import argparse

device_info = {}


def init_device_info(ser):
    ser.write(b'i')
    result = ser.read(13)
    if len(result) != 13:
        print(f"Device info read filed, data size = {len(result)}, it should be 13")
        exit(-1)
    device_info['fw_ver_major'] = result[0]
    device_info['fw_ver_minor'] = result[1]
    device_info["ch_number"] = result[2]
    device_info["ch_types_mask"] = result[3]
    sens_number = result[4]
    device_info["sens_number"] = sens_number
    device_info["ch_ids"] = result[5:(5 + sens_number)]


def print_ids():
    ids_string = "Sensor IDs:  "
    for n in range(device_info['sens_number']):
        ids_string += f"0x{(device_info['ch_ids'][n]):02x} "
    print(ids_string)


def print_device_info():
    print(f"Firmware version: {device_info['fw_ver_major']}.{device_info['fw_ver_minor']}")
    print(f"Channels number: {device_info['ch_number']}")
    for ch in range(device_info['ch_number']):
        print(f"Channel {ch} type: {'Analog' if device_info['ch_types_mask'] & (1 << ch) else 'PWM'}")
    print(f"Sensors number: {device_info['sens_number']}")
    print_ids()


def get_sensor_data(ser):
    ser.write(b't')
    data_size = device_info['sens_number'] * 2
    return data_size, ser.read(data_size)


def print_sensors(ser):
    data_size, data = get_sensor_data(ser)
    values = []
    for i in range(0, data_size, 2):
        values.append((data[i] * 256 + data[i + 1]) / 2)
    print(*values)


parser = argparse.ArgumentParser(description='Fan controller service utility')
parser.add_argument('--listports', '-l', action='store_true', help='List available serial ports')
parser.add_argument('--port', '-p', action='store', help='Serial port selection')
parser.add_argument('--sensors', '-s', action='store_true', help='Print temperatures')
parser.add_argument('--info', '-i', action='store_true', help='Print device information')
args = parser.parse_args()

if args.listports:
    ports = list_ports.comports()
    for p in ports:
        print(p.description)
elif args.port is not None:
    serial = serial.Serial(args.port, baudrate=19200, timeout=0.2)
    serial.reset_input_buffer()
    init_device_info(serial)
    if args.info:
        print_device_info()
        print("Sensor data: ", end='')
        print_sensors(serial)
else:
    print("Port name must be provided")


