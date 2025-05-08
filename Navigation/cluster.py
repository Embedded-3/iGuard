import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QVBoxLayout, QGroupBox
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QSizePolicy

class SimpleVehicleMonitor(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Vehicle Status Monitor")
        self.setGeometry(200, 200, 600, 500)

        # 💡 다크 테마 + 폰트 크기 확대
        self.setStyleSheet("""
            QWidget {
                background-color: #2e2e2e;
                color: #eeeeee;
                font-size: 28px;
            }
            QGroupBox {
                border: 1px solid #555;
                margin-top: 10px;
                padding: 10px;
            }
            QGroupBox:title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 6px;
                color: #bbbbbb;
            }
        """)

        # 상태 표시 레이블 생성 함수 (중복 제거 + 설정 통일)
        def create_label(text):
            label = QLabel(text)
            label.setWordWrap(True)
            label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
            return label

        # 레이블 생성
        self.ignition_label = create_label("시동 상태: UNKNOWN")
        self.heater_label = create_label("히터: UNKNOWN")
        self.ac_label = create_label("에어컨: UNKNOWN")
        self.temp_label = create_label("온도: UNKNOWN")
        self.humidity_label = create_label("습도: UNKNOWN")
        self.eye_label = create_label("아이 감지: UNKNOWN")
        self.co2_label = create_label("CO2 농도: UNKNOWN")
        self.dust_label = create_label("유해가스: UNKNOWN")

        # 레이아웃 설정
        layout = QVBoxLayout()
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(20)

        # 내부 상태
        inside_group = QGroupBox("차량 내부 상태")
        inside_layout = QVBoxLayout()
        inside_layout.setSpacing(15)
        inside_layout.addWidget(self.ignition_label)
        inside_layout.addWidget(self.heater_label)
        inside_layout.addWidget(self.ac_label)
        inside_group.setLayout(inside_layout)

        # 실내 환경
        env_group = QGroupBox("실내 환경")
        env_layout = QVBoxLayout()
        env_layout.setSpacing(15)
        env_layout.addWidget(self.temp_label)
        env_layout.addWidget(self.humidity_label)
        env_layout.addWidget(self.eye_label)
        env_layout.addWidget(self.co2_label)
        env_group.setLayout(env_layout)

        # 외부 상태
        outside_group = QGroupBox("차량 외부 상태")
        outside_layout = QVBoxLayout()
        outside_layout.setSpacing(15)
        outside_layout.addWidget(self.dust_label)
        outside_group.setLayout(outside_layout)

        # 메인 레이아웃 구성
        layout.addWidget(inside_group)
        layout.addWidget(env_group)
        layout.addWidget(outside_group)
        self.setLayout(layout)

        # 상태 업데이트 타이머
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_status)
        self.timer.start(2000)

    def update_status(self):
        self.ignition_label.setText("시동 상태: ON")
        self.heater_label.setText("히터: OFF")
        self.ac_label.setText("에어컨: ON")
        self.temp_label.setText("온도: 22 °C")
        self.humidity_label.setText("습도: 60 %")
        self.eye_label.setText("아이 감지: ON")
        self.co2_label.setText("CO2 농도: 400 ppm")
        self.dust_label.setText("유해가스: 보통")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = SimpleVehicleMonitor()
    win.show()
    sys.exit(app.exec_())
