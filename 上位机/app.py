import eventlet
eventlet.monkey_patch()  # 必须在所有其他模块导入前执行
from datetime import datetime
from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit
from models.last import TCPService  # 假设TCPService在models.last模块中
import threading

# 配置Flask应用
app = Flask(__name__)
app.config['SECRET_KEY'] = 'your-secret-key'  # 用于WebSocket安全连接

# 初始化SocketIO，设置eventlet异步模式
socketio = SocketIO(
    app,
    cors_allowed_origins="*",
    async_mode='eventlet',
    logger=True,  # 启用SocketIO日志
    engineio_logger=True  # 启用Engine.IO日志
)

# 创建TCP服务实例
tcp_service = TCPService(port=8888)
# 存储SocketIO上下文，用于跨线程通信
socketio_context = {"socketio": socketio, "app": app}


def handle_data(data):
    """处理接收到的数据，通过WebSocket广播"""
    print(f"[TCP服务] 接收到数据: {data}")
    # 使用带上下文的广播方法
    broadcast_data_with_context(data, socketio_context)


def broadcast_data_with_context(data, context):
    """在独立线程中安全地广播数据"""
    # 确保在Flask应用上下文中执行
    with context["app"].app_context():
        try:
            print(f"[SocketIO] 尝试广播数据: {data}")
            # 明确指定广播参数
            context["socketio"].emit('data_update', data)
            print(f"[SocketIO] 数据广播成功")
        except Exception as e:
            print(f"[SocketIO] 广播异常: {str(e)}")
            import traceback
            traceback.print_exc()  # 打印详细堆栈信息


def handle_command(command_data):
    """处理命令发送事件"""
    print(f"[命令处理] 发送命令: {command_data['command']}")
    with app.app_context():
        socketio.emit('command_status', command_data)



@app.route('/')
def index():
    """首页路由"""
    return render_template('index.html')


@socketio.on('connect')
def on_connect():
    """客户端连接处理"""
    print(f"[SocketIO] 客户端连接成功，ID: {request.sid}")
    # 连接后可以发送历史数据或初始化数据
    # socketio.emit('initial_data', get_initial_data())


@socketio.on('disconnect')
def on_disconnect():
    """客户端断开连接处理"""
    print(f"[SocketIO] 客户端断开连接，ID: {request.sid}")


@socketio.on('send_command')
def handle_send_command(data):
    command = data.get('command')
    if command == 'device_on':
        result = tcp_service.send_device_on()
    elif command == 'device_off':
        result = tcp_service.send_device_off()
    elif command == 'alert_on':
        alert_id = data.get('alert_id')
        result = tcp_service.send_alert_on(alert_id)
    elif command == 'alert_off':
        alert_id = data.get('alert_id')
        result = tcp_service.send_alert_off(alert_id)
    elif command == 'temp_threshold':
        value = data.get('value')
        result = tcp_service.send_temperature_threshold(value)
    elif command == 'humi_threshold':
        value = data.get('value')
        result = tcp_service.send_humidity_threshold(value)
    else:
        result = False

    status = {
        'command': command,
        'success': result,
        'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }
    handle_command(status)


def start_tcp_service():
    """启动TCP服务（独立线程）"""
    try:
        tcp_service.start()
    except Exception as e:
        print(f"[TCP服务] 启动异常: {str(e)}")
        import traceback
        traceback.print_exc()


if __name__ == '__main__':
    # 注册TCP服务回调函数
    tcp_service.register_data_callback(handle_data)
    tcp_service.register_command_callback(handle_command)

    # 启动TCP服务线程
    tcp_thread = threading.Thread(target=start_tcp_service)
    tcp_thread.daemon = True
    tcp_thread.start()

    print("[系统启动] TCP服务已启动，监听地址: 0.0.0.0:8888")
    print("[系统启动] 等待客户端连接...")

    # 使用eventlet的WSGI服务器（推荐生产环境）
    print("[系统启动] 使用eventlet服务器，访问 http://localhost:5000")
    from eventlet import wsgi
    wsgi.server(eventlet.listen(('0.0.0.0', 5000)), app)