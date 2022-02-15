from serial.tools import list_ports
import serial
import yaml
import argparse

SIZEOF_DEVICE_INFO = 13
SIZEOF_CONTROL_STRUCT = 15
SIZEOF_DEBUG_STRUCT = 8

device_info = {}


def calc_crc_byte(crc, byte):
    for i in range(8, 0, -1):
        mix = (crc ^ byte) & 0x01
        crc >>= 1
        if mix > 0:
            crc ^= 0x8C
        byte >>= 1
    return crc


def calc_crc(arr):
    c = 0xDE
    for b in arr:
        c = calc_crc_byte(c, b)
    return c


def init_device_info(ser):
    ser.write(b'i')
    result = ser.read(SIZEOF_DEVICE_INFO)
    if len(result) != SIZEOF_DEVICE_INFO:
        print(f"Device info read failed, data size = {len(result)}, it should be 13")
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


def get_debug_data(ser):
    ser.write(b'd')
    data = ser.read(SIZEOF_DEBUG_STRUCT)
    if len(data) != SIZEOF_DEBUG_STRUCT:
        print("Debug data reading failed")
        exit(-1)
    debug_struct = {}
    debug_struct['pwm'] = list(data[0:2])
    debug_struct['isStopped'] = list(data[2:4])
    debug_struct['iVal'] = [(data[4] * 256 + data[5]),
                            (data[6] * 256 + data[7])]
    return debug_struct


def parse_control_struct(data):
    result = {
        'pollTimeSecs': data[0],
        'fanStopHysteresis': data[1],
        'pwmMin': data[2],
        'pwmMax': data[3],
        'sensorNumber': data[4],
        'sensorIds': list(data[5:5 + data[4]]),
        'algoType': data[9],
        'algo': {}}
    if result['algoType'] == 0:
        result['algo']['tmin'] = data[10]
        result['algo']['tmax'] = data[11]
    else:
        result['algo']['t'] = data[10]
        result['algo']['kp'] = data[11]
        result['algo']['ki'] = data[12]
        result['algo']['max_i'] = data[13]
    result['crc'] = data[14]
    return result


def get_control_structs(ser):
    ser.write(b'r')
    ch_number = device_info['ch_number']
    data_size = ch_number * SIZEOF_CONTROL_STRUCT
    data = ser.read(data_size)
    if len(data) != data_size:
        print("Control structs reading failed")
        exit(-1)

    control_structs = []
    for i in range(ch_number):
        begin = i * SIZEOF_CONTROL_STRUCT
        end = (i + 1) * SIZEOF_CONTROL_STRUCT
        control_structs.append(parse_control_struct(data[begin:end]))
        crc = calc_crc(data[begin:end])
        if crc:
            print(f"CRC {i} check failed: {hex(crc)}")
            exit(-1)
    return control_structs


def print_sensors(ser):
    data_size, data = get_sensor_data(ser)
    values = []
    for i in range(0, data_size, 2):
        values.append((data[i] * 256 + data[i + 1]) / 2)
    print(*values)


parser = argparse.ArgumentParser(description='Fan controller service utility')
mutex_group = parser.add_mutually_exclusive_group()

mutex_group.add_argument('--listports', '-l', action='store_true', help='List available serial ports')
mutex_group.add_argument('--port', '-p', action='store', help='Serial port selection')
parser.add_argument('--sensors', '-s', action='store_true', help='Print temperatures')
parser.add_argument('--info', '-i', action='store_true', help='Print device information')
parser.add_argument('--control', '-c', action='store_true', help='Print control structs')
parser.add_argument('--debug', '-d', action='store_true', help='Print debug data')

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
    elif args.sensors:
        print_sensors(serial)
    elif args.control:
        cs = get_control_structs(serial)
        # print(yaml.dump(get_control_structs(serial), sort_keys=False))
    elif args.debug:
        print(yaml.dump(get_debug_data(serial), default_flow_style=True, sort_keys=False))
else:
    print("Port name must be provided")


