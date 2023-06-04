from serial.tools import list_ports
import serial
from ruamel.yaml import YAML
import argparse
import sys
import pprint

# 16 bytes of DeviceInfo struct, 1+8 bytes of sensor IDs array
SIZEOF_DEVICE_INFO = 25
SIZEOF_CONTROL_STRUCT = 15
SIZEOF_DEBUG_STRUCT = 8
MAX_CH_NUMBER = 2

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


device_info = {}

def get_integer(byte_arr, index):
    return int.from_bytes(byte_arr[index:index + 4], 'big', signed="False")

def init_device_info(ser):
    ser.write(b'i')
    result = ser.read(SIZEOF_DEVICE_INFO)
    if len(result) != SIZEOF_DEVICE_INFO:
        print(f"Device info read failed, data size = {len(result)} != 25")
        exit(-1)
    device_info['fw_ver_major'] = get_integer(result, 0)
    device_info['fw_ver_minor'] = get_integer(result, 4)
    device_info['ch_mask'] = get_integer(result, 8)
    device_info['ch_types_mask'] = get_integer(result, 12)
    sens_number = result[16]
    device_info['sens_number'] = sens_number
    device_info['sensor_ids'] = result[17:(17 + sens_number)]


def format_ids():
    ids_string = "Sensor IDs:  "
    for n in range(device_info['sens_number']):
        ids_string += f"0x{(device_info['sensor_ids'][n]):02x} "
    return ids_string


def format_device_info():
    result = (f"Firmware version: {device_info['fw_ver_major']}.{device_info['fw_ver_minor']}\n"
    f"Channels mask: {device_info['ch_mask']}\n")
    for ch in range(MAX_CH_NUMBER):
        state = 'Active' if device_info['ch_mask'] & (1 << ch) else 'Disabled'
        type = 'Analog' if device_info['ch_types_mask'] & (1 << ch) else 'PWM'
        result += f"Channel {ch} mode: {type}, {state}\n"
    result += f"Sensors amount: {device_info['sens_number']}\n"
    result += format_ids()
    return result

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
        'algo': {}
    }
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
    ch_number = device_info['ch_mask']
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


PWM_MAX_RAW = 80
PWM_MAX_PERCENT = 100
SCALE_FACTOR_KP = 16
SCALE_FACTOR_KI = 128


def norm_pwm(pwm_raw):
    return round((pwm_raw * PWM_MAX_PERCENT) / PWM_MAX_RAW)


norm_max_i = norm_pwm


def to_raw_pwm(pwm):
    return round((pwm * PWM_MAX_RAW) / PWM_MAX_PERCENT)


to_raw_max_i = to_raw_pwm


def norm_kp(kp_raw):
    return (kp_raw * PWM_MAX_PERCENT) / (SCALE_FACTOR_KP * PWM_MAX_RAW)


def to_raw_kp(kp):
    result = round((kp * SCALE_FACTOR_KP * PWM_MAX_RAW) / PWM_MAX_PERCENT)
    return result if result < 256 else 255


def norm_ki(ki_raw):
    return (ki_raw * PWM_MAX_PERCENT) / (SCALE_FACTOR_KI * PWM_MAX_RAW)


def to_raw_ki(ki):
    result = round((ki * SCALE_FACTOR_KI * PWM_MAX_RAW) / PWM_MAX_PERCENT)
    return result if result < 256 else 255


# YAML config functions
def add_config_sensors(config):
    config['sensors'] = {}
    for i, s_id in enumerate(device_info['sensor_ids']):
        s_type = 'DS18B20' if s_id & 0x80 else 'LM75'
        s_id &= ~0x80
        config['sensors'][f'S{i}'] = {'type': s_type, 'id': s_id}


PWM_NAME_PREFIX = "OUT"


def add_config_pwms(config, cs):
    config['pwms'] = {}
    for i in range(device_info['ch_mask']):
        config['pwms'][f'{PWM_NAME_PREFIX}{i}'] = {f'channel': i, 'minpwm': norm_pwm(cs[i]['pwmMin']),
                                                   'maxpwm': norm_pwm(cs[i]['pwmMax'])}
        fanstop_hyst = cs[i]['fanStopHysteresis']
        if fanstop_hyst:
            config['pwms'][f'OUT{i}']['fanstop_hyst'] = fanstop_hyst


def add_config_controllers(config, cs):
    config['controllers'] = {}
    for i in range(device_info['ch_mask']):
        sensors = []
        for sn in range(cs[i]['sensorNumber']):
            sensors.append(f'S{sn}')
        settings = cs[i]['algo'].copy()
        if cs[i]['algoType'] == 0:
            mode = 'two_point'
        else:
            mode = 'pi'
            settings['kp'] = norm_kp(settings['kp'])
            settings['ki'] = norm_ki(settings['ki'])
            settings['max_i'] = norm_max_i(settings['max_i'])
        poll_time = cs[i]['pollTimeSecs']
        config['controllers'][f'CTRL{i}'] = {'sensor': sensors, 'pwm': f'{PWM_NAME_PREFIX}{i}', 'mode': mode,
                                             'set': settings, 'poll': poll_time}


def get_sensor_raw_id(sensor):
    if sensor['type'] == 'DS18B20':
        result = sensor['id'] | 0x80
    else:
        result = sensor['id']
    return result


# Upload config to the controller
def to_raw_config(conf):
    packet = bytearray(SIZEOF_CONTROL_STRUCT * device_info['ch_mask'])
    print(conf['controllers'])
    pwms = conf['pwms']
    all_sensors = conf['sensors']
    ch = 0
    for name, settings in conf['controllers'].items():
        data = bytearray(SIZEOF_CONTROL_STRUCT)
        data[0] = settings['poll']
        pwm = pwms[settings['pwm']]
        data[1] = pwm.get('fanstop_hyst', 0)
        data[2] = to_raw_pwm(pwm['minpwm'])
        data[3] = to_raw_pwm(pwm['maxpwm'])

        data[4] = len(settings['sensor'])
        for i, s_name in enumerate(settings['sensor']):
            data[5 + i] = get_sensor_raw_id(all_sensors[s_name])
        algo = settings['set']
        if settings['mode'] == 'two_point':
            data[9] = 0
            data[10] = algo['tmin']
            data[11] = algo['tmax']
        else:
            data[9] = 1
            data[10] = algo['t']
            data[11] = to_raw_kp(algo['kp'])
            data[12] = to_raw_ki(algo['ki'])
            data[13] = to_raw_max_i(algo['max_i'])
        data[14] = calc_crc(data[: -1])
        packet[SIZEOF_CONTROL_STRUCT * ch:] = data
        ch += 1
    return packet


def update_header(text):
    origin = '# The template configuration file for the mcu_fancontrol project, must not be changed by human'
    actual = '# The configuration file for the particular device\n'
    actual += '# --- Device info ---\n# '
    actual += '# '.join(format_device_info().splitlines(True))
    return text.replace(origin, actual)


# Parse args
parser = argparse.ArgumentParser(description='Fan controller service utility')
mutex_group = parser.add_mutually_exclusive_group()

mutex_group.add_argument('--listports', '-l', action='store_true', help='List available serial ports')
mutex_group.add_argument('--port', '-p', action='store', help='Serial port selection')
parser.add_argument('--read', '-r', action='store', help='Retrieve current config and store it to file, expects file '
                                                         'name')
parser.add_argument('--write', '-w', action='store', help='Upload config file to the controller, expects file name')
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
        print(format_device_info())
        print("Sensor data: ", end='')
        print_sensors(serial)
    elif args.sensors:
        print_sensors(serial)
    elif args.control:
        cs = get_control_structs(serial)
        yaml = YAML()
        yaml.dump(get_control_structs(serial), sys.stdout)
    elif args.read:
        cs = get_control_structs(serial)
        f = open("ref.yaml", "r")
        config_text = update_header(f.read())
        yaml = YAML()
        yaml.default_flow_style = True
        config_template = yaml.load(config_text)
        f.close()
        add_config_sensors(config_template)
        add_config_pwms(config_template, cs)
        add_config_controllers(config_template, cs)
        f = open(args.read, "w")
        yaml.dump(config_template, f)
        f.close()
    elif args.write:
        yaml = YAML()
        f = open(args.write, "r")
        config = yaml.load(f)
        f.close()
        data = to_raw_config(config)
        serial.write(b'w')
        serial.write(data)
        status = int.from_bytes(serial.read(1), 'little')
        if status == device_info['ch_mask'] * 2:
            print("Config upload finished successfully")
        else:
            print(f"Config upload failed with status: {status}")
    elif args.debug:
        pp = pprint.PrettyPrinter(indent=4)
        pp.pprint(get_debug_data(serial))
else:
    print("Port name must be provided")
