import sys
import struct
import threading
from PyQt5.QtWidgets import QApplication
from rpi_backlight import Backlight

from can_handler import create_can_bus
from dashboard import Dashboard
from detection_mode import DetectionMode

current_mode = "driving"
status_text = {...}

bus = create_can_bus()
backlight = Backlight()
detection_handler = None

app = QApplication(sys.argv)
dashboard_window = Dashboard(bus) 

def can_receiver():
    global current_mode, detection_handler
    print("CAN 메시지 수신 대기 중...")
    while True:
        message = bus.recv()
        if message:
            if message.arbitration_id == 0x02 and current_mode != 'detection':
                current_mode = "detection"
                print("모드 전환 → detection")
                backlight.power = False

                # dashboard 화면 감추기
                dashboard_window.hide()

                # detection 모드 처리 시작
                if not detection_handler:
                    detection_handler = DetectionMode(bus)

            elif message.arbitration_id == 0x03 and current_mode != 'driving':
                current_mode = "driving"
                print("모드 전환 → driving")
                backlight.power = True

                # dashboard 화면 다시 표시
                dashboard_window.showFullScreen()

            # 상태값 업데이트...
            if current_mode == 'detection' and detection_handler:
                detection_handler.handle_can_message(message)

can_thread = threading.Thread(target=can_receiver)
can_thread.daemon = True
can_thread.start()

if __name__ == '__main__':
    dashboard_window.showFullScreen()  # 시작할 때만 한 번 실행
    try:
        sys.exit(app.exec_())  # PyQt 이벤트 루프는 한 번만 실행
    except KeyboardInterrupt:
        print("프로그램 종료!")

