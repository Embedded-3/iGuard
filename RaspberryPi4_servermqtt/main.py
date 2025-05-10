import os
import base64
import threading
import paho.mqtt.client as mqtt
import can
import time
import struct

# 이미지 저장 파일명
image_file = "image.jpg"

# MQTT 서버 설정
broker_ip = "172.20.10.5"
port = 1883
topic_publish = "Car/image"
topic_trigger = "Car/trigger"
topic_status = "Car/status"
topic_message = "Car/LCD"
inside_topic = "Car/inside"
outside_topic = "Car/outside"
music_topic = "Car/music"

# CAN interface 설정
can_interface = 'can0'
bus = can.interface.Bus(channel=can_interface, bustype='socketcan')

# CAN 메시지 전송 함수 (bytes, list 모두 처리)
def send_can_message(can_id, data_list):
    if isinstance(data_list, bytes):
        data_list = bytearray(data_list)
    can_msg = can.Message(arbitration_id=can_id, data=data_list, is_extended_id=False)
    try:
        bus.send(can_msg)
        print(f"CAN 메시지 전송: ID={hex(can_id)}, Data={list(data_list)}")
    except can.CanError as e:
        print(f"CAN 전송 실패: {e}")

# MQTT 연결 콜백
def on_connect(client, userdata, flags, rc):
    print(f"MQTT 연결 상태: {rc}")
    if rc == 0:
        print("MQTT 연결 성공!")
        client.subscribe(topic_trigger)
        client.subscribe(topic_status)
        client.subscribe(topic_message)
        client.subscribe(music_topic)
    else:
        print(f"MQTT 연결 실패, 코드: {rc}")

# MQTT 메시지 콜백
def on_message(client, userdata, msg):
    payload = msg.payload.decode().strip()
    print(f"{msg.topic} -> {payload}")

    if msg.topic == topic_trigger and payload == "trigger":
        print("MQTT Trigger 수신! 사진 촬영 시작!")
        capture_and_publish(client)

    elif msg.topic == topic_message:
        payload_bytes = payload.encode('ascii', errors='ignore')
        fixed_length = 32
        padded_payload = payload_bytes.ljust(fixed_length, b'\x00')
        for i in range(0, fixed_length, 8):
            chunk = padded_payload[i:i+8]
            send_can_message(0x010, chunk)

    elif msg.topic == music_topic:
        if ":" in payload:
            state, music_number = payload.split(":")
            music_number = int(music_number)
            if state == "1":
                print(f"{music_number}번 음악 재생")
                send_can_message(0x005, [music_number, 0, 0, 0, 0, 0, 0, 0])
            elif state == "0":
                print(f"{music_number}번 음악 종료")
                send_can_message(0x006, [0, 0, 0, 0, 0, 0, 0, 0])
            else:
                print("Music Player Error")
        else:
            state = payload
            if state == "1":
                print("Random Play")
                send_can_message(0x005, [0xFF, 0, 0, 0, 0, 0, 0, 0])
            elif state == "0":
                print("Stop")
                send_can_message(0x006, [0, 0, 0, 0, 0, 0, 0, 0])
            else:
                print("Music Player Error")

# 사진 촬영 및 MQTT 발행
def capture_and_publish(client):
    print("사진 촬영 중...")
    os.system(f"fswebcam -d /dev/video0 --jpeg 85 -r 640x480 {image_file}")
    with open(image_file, "rb") as f:
        img_bytes = f.read()
    encoded_image = base64.b64encode(img_bytes).decode()
    client.publish(topic_publish, encoded_image)
    print("이미지 MQTT 발행 완료!")

# MQTT 클라이언트 설정
client = mqtt.Client()
client.username_pw_set("master", "1234")
client.on_connect = on_connect
client.on_message = on_message

# MQTT 브로커 연결
client.connect(broker_ip, port, 60)
client.loop_start()

# CAN 수신 처리 함수
def can_receiver():
    print("CAN 메시지 수신 대기 중...")
    while True:
        message = bus.recv()
        if message is not None:
            if message.arbitration_id == 0x21:
                temperature = struct.unpack('<H', message.data[0:2])[0] / 10.0
                humidity = struct.unpack('<H', message.data[4:6])[0] / 10.0
                payload = f"Temperature: {temperature}°C, Humidity: {humidity}%"
                client.publish(inside_topic, payload)
                print(f"MQTT 발행 ({inside_topic}): {payload}")

            elif message.arbitration_id == 0x20:
                data0 = struct.unpack('<I', message.data[0:4])[0]
                data1 = struct.unpack('<I', message.data[4:8])[0]
                ext_air = (data0 >> 16) & 0xFFFF
                int_co2 = data0 & 0xFFFF
                mode = (data1 >> 16) & 0xFFFF
                speed = data1 & 0xFFFF
                payload = f"Ext Air: {ext_air}, CO2: {int_co2}, Mode: {mode}, Speed: {speed}"
                client.publish(outside_topic, payload)
                print(f"MQTT 발행 ({outside_topic}): {payload}")

# CAN 수신 쓰레드 실행
can_thread = threading.Thread(target=can_receiver)
can_thread.daemon = True
can_thread.start()

# 메인 루프는 MQTT 유지
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("프로그램 종료")

