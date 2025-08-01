import socket
import csv
import time
import threading
import re
from datetime import datetime


class TCPService():
    """TCP服务类，实现与WiFisystem客户端的通信、消息解析和数据记录"""

    def __init__(self, host='0.0.0.0', port=8888, csv_file='data_record.csv'):
        """初始化TCP服务"""
        self.host = host
        self.port = port
        self.csv_file = csv_file
        self.server_socket = None
        self.client_socket = None
        self.client_id = "WiFisystem"
        self.is_running = False
        self.thread = None
        self.data_callbacks = []  # 存储数据回调函数
        self.command_callbacks = []  # 存储命令回调函数

        # 初始化CSV文件头
        self._init_csv_file()

    def _init_csv_file(self):
        """初始化CSV文件，创建表头"""
        try:
            with open(self.csv_file, 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(['timestamp', 'temperature', 'humidity', 'client_id'])
        except Exception as e:
            print(f"CSV文件初始化错误: {e}")

    def register_data_callback(self, callback):
        """注册数据接收回调函数"""
        self.data_callbacks.append(callback)
        print(f"数据回调函数已注册: {callback.__name__}")

    def register_command_callback(self, callback):
        """注册命令处理回调函数"""
        self.command_callbacks.append(callback)
        print(f"命令回调函数已注册: {callback.__name__}")

    def start(self):
        """启动TCP服务"""
        try:
            # 创建TCP socket
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

            # 绑定地址和端口
            self.server_socket.bind((self.host, self.port))

            # 开始监听
            self.server_socket.listen(1)
            self.is_running = True
            print(f"TCP服务已启动，监听地址: {self.host}:{self.port}")

            # 创建线程处理客户端连接
            self.thread = threading.Thread(target=self._accept_client)
            self.thread.daemon = True
            self.thread.start()

        except Exception as e:
            print(f"启动服务错误: {e}")
            self.stop()

    def _accept_client(self):
        """接受客户端连接"""
        try:
            print("等待客户端连接...")
            self.client_socket, addr = self.server_socket.accept()
            print(f"客户端已连接: {addr}")

            # 创建线程处理客户端消息
            client_thread = threading.Thread(target=self._handle_client)
            client_thread.daemon = True
            client_thread.start()

        except Exception as e:
            print(f"客户端连接错误: {e}")
            self.stop()

    def _handle_client(self):
        """处理客户端消息"""
        try:
            while self.is_running and self.client_socket:
                # 接收消息
                data = self.client_socket.recv(1024).decode('utf-8')
                if not data:
                    break

                #print(f"接收来自客户端的消息: {data}")

                # 解析消息
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                temperature = None
                humidity = None

                parsed_data = self._parse_message(data, timestamp, temperature, humidity)
                #print(parsed_data)
                timestamp = parsed_data['timestamp']
                temperature = parsed_data.get('temperature')
                humidity = parsed_data.get('humidity')

                # 写入CSV文件
                self._write_to_csv(timestamp, temperature, humidity)

                # 调用数据回调函数
                self._notify_data_callbacks(parsed_data)

        except Exception as e:
            print(f"处理客户端消息错误: {e}")
            self.stop()

    def _parse_message(self, message, timestamp, temperature, humidity):
        """解析客户端消息，返回包含解析结果的字典"""
        result = {
            'timestamp': timestamp,
            'temperature': temperature,
            'humidity': humidity
        }

        # 处理包含多条消息的情况
        #ipd_messages = re.findall(r'\+IPD,\d+,\d+:(.*?)(\r\n|$)', message, re.DOTALL)
        ipd_messages = re.findall(r'VFD\d+[A-Z]?[+-]?\d+\.?\d*VF', message)
        #print(ipd_messages)

        for msg_match in ipd_messages:
            msg_content = msg_match.strip()  # 获取消息内容并去除空白
            print(f"处理消息: {msg_content}")

            # 解析温度数据上报
            temp_match = re.match(r'VFD1A([+-]?\d+\.\d+)VF', msg_content)
            if temp_match:
                result['temperature'] = float(temp_match.group(1))
                print(f"解析到温度数据: {result['temperature']}°C")
                continue  # 继续处理下一条消息

            # 解析湿度数据上报
            humi_match = re.match(r'VFD2A(\d{5})VF', msg_content)
            if humi_match:
                five_digits = humi_match.group(1)
                last_two_digits = five_digits[-2:]
                result['humidity'] = int(last_two_digits)
                print(f"解析到湿度数据: {result['humidity']}%RH")

        return result

    def _write_to_csv(self, timestamp, temperature, humidity):
        """将数据写入CSV文件"""
        try:
            with open(self.csv_file, 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow([timestamp, temperature, humidity, self.client_id])
        except Exception as e:
            print(f"写入CSV文件错误: {e}")

    def _notify_data_callbacks(self, data):
        """通知所有数据回调函数"""
        for callback in self.data_callbacks:
            try:
                # 使用线程池或异步方式调用，避免阻塞
                threading.Thread(target=callback, args=(data,), daemon=True).start()
            except Exception as e:
                print(f"调用数据回调函数时出错: {e}")

    def send_device_on(self):
        """发送设备开启命令（针对单设备，新协议）"""
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送命令")
            return False

        try:
            # 单设备命令格式: CRD0AONCR（假设设备ID固定为0）
            command = "CRD0AONCR"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送设备开启命令: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'device_on',
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送设备开启命令错误: {e}")
            return False

    def send_device_off(self):
        """发送关闭A设备命令"""
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送命令")
            return False

        try:
            command = "CRD0AOFCR"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送设备关闭命令: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'device_off',
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送设备关闭命令错误: {e}")
            return False

    def send_alert_on(self, alert_id):
        """
        发送开启警报命令

        :param alert_id: 警报ID，1或2
        """
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送命令")
            return False

        try:
            if alert_id not in [1, 2]:
                print(f"无效的警报ID: {alert_id}，支持1或2")
                return False

            command = f"CRALM{alert_id}ONCR"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送开启警报{alert_id}命令: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'alert_on',
                'alert_id': alert_id,
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送开启警报命令错误: {e}")
            return False

    def send_alert_off(self, alert_id):
        """
        发送关闭警报命令

        :param alert_id: 警报ID，1或2
        """
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送命令")
            return False

        try:
            if alert_id not in [1, 2]:
                print(f"无效的警报ID: {alert_id}，支持1或2")
                return False

            command = f"CRALM{alert_id}OFCR"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送关闭警报{alert_id}命令: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'alert_off',
                'alert_id': alert_id,
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送关闭警报命令错误: {e}")
            return False

    def send_temperature_threshold(self, value):
        """
        发送温度阈值设置命令

        :param value: 温度值，格式为±XX.X（如+25.5）
        """
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送阈值设置")
            return False

        try:
            # 验证温度值格式
            if not re.match(r'^[+-]?\d+\.\d+$', str(value)):
                print(f"无效的温度阈值格式: {value}，应为±XX.X格式")
                return False

            command = f"VFS1A{value}VF"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送温度阈值设置: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'temp_threshold',
                'value': value,
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送温度阈值设置错误: {e}")
            return False

    def send_humidity_threshold(self, value):
        """
        发送湿度阈值设置命令

        :param value: 湿度值（0-100的整数）
        """
        if not self.client_socket or not self.is_running:
            print("客户端未连接，无法发送阈值设置")
            return False

        try:
            value = int(value)
            if not 0 <= value <= 100:
                print(f"无效的湿度阈值: {value}，范围应为0-100")
                return False

            # 格式: VFS2A000{value}VF（后两位为数据）
            formatted_value = f"000{value}"[-2:]
            command = f"VFS2A000{formatted_value}VF"
            formatted_command = f"{command}"
            self.client_socket.send(formatted_command.encode('utf-8'))
            print(f"发送湿度阈值设置: {formatted_command}")

            # 调用命令回调函数
            self._notify_command_callbacks({
                'command': 'humi_threshold',
                'value': value,
                'data': formatted_command,
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })

            return True
        except Exception as e:
            print(f"发送湿度阈值设置错误: {e}")
            return False

    def _notify_command_callbacks(self, data):
        """通知所有命令回调函数"""
        for callback in self.command_callbacks:
            try:
                # 使用线程池或异步方式调用，避免阻塞
                threading.Thread(target=callback, args=(data,), daemon=True).start()
            except Exception as e:
                print(f"调用命令回调函数时出错: {e}")

    def stop(self):
        """停止TCP服务"""
        self.is_running = False
        try:
            if self.client_socket:
                self.client_socket.close()
                self.client_socket = None
            if self.server_socket:
                self.server_socket.close()
                self.server_socket = None
            print("TCP服务已停止")
        except Exception as e:
            print(f"停止服务错误: {e}")