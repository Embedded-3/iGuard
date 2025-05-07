import sys
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QVBoxLayout, QGroupBox, QPushButton, QHBoxLayout
)
from PyQt5.QtCore import QTimer

class SimpleVehicleMonitor(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Vehicle Status Monitor")
        self.setGeometry(200, 200, 800, 480)  # 넉넉한 가로 길이 확보

        # 다크 테마
        self.setStyleSheet("""
            QWidget {
                background-color: #2e2e2e;
                color: #eeeeee;
                font-size: 14px;
            }
            QGroupBox {
                border: 1px solid #555;
                border-radius: 5px;
                margin-top: 10px;
                padding: 10px;
            }
            QGroupBox:title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 5px;
                color: #bbbbbb;
            }
            QPushButton {
                background-color: #3a3a3a;
                color: white;
                border: none;
                border-radius: 8px;
                font-size: 15px;
                padding: 8px 12px;
            }
            QPushButton:hover {
                background-color: #5a5a5a;
            }
        """)

        # 종료 버튼
        self.close_button = QPushButton("X", self)
        self.close_button.setFixedSize(30, 30)
        self.close_button.clicked.connect(self.close)
        self.close_button.setStyleSheet("""
            QPushButton {
                background-color: #ff5555;
                color: white;
                border-radius: 5px;
                font-size: 16px;
            }
            QPushButton:hover {
                background-color: #ff0000;
            }
        """)

        # 차량 상태 정보 그룹들 ----------------------------
        self.ignition_label = QLabel("시동 상태: UNKNOWN")
        self.heater_label = QLabel("히터: UNKNOWN")
        self.ac_label = QLabel("에어컨: UNKNOWN")
        self.temp_label = QLabel("온도: UNKNOWN")
        self.humidity_label = QLabel("습도: UNKNOWN")
        self.eye_label = QLabel("아이 감지: UNKNOWN")
        self.co2_label = QLabel("CO2 농도: UNKNOWN")
        self.dust_label = QLabel("유해가스: UNKNOWN")

        inside_group = QGroupBox("차량 내부 상태")
        inside_layout = QVBoxLayout()
        inside_layout.addWidget(self.ignition_label)
        inside_layout.addWidget(self.heater_label)
        inside_layout.addWidget(self.ac_label)
        inside_group.setLayout(inside_layout)

        env_group = QGroupBox("실내 환경")
        env_layout = QVBoxLayout()
        env_layout.addWidget(self.temp_label)
        env_layout.addWidget(self.humidity_label)
        env_layout.addWidget(self.eye_label)
        env_layout.addWidget(self.co2_label)
        env_group.setLayout(env_layout)

        outside_group = QGroupBox("차량 외부 상태")
        outside_layout = QVBoxLayout()
        outside_layout.addWidget(self.dust_label)
        outside_group.setLayout(outside_layout)

        left_layout = QVBoxLayout()
        left_layout.addWidget(inside_group)
        left_layout.addWidget(env_group)
        left_layout.addWidget(outside_group)

        # 제어 버튼들 -----------------------------------
        self.volume_up_btn = QPushButton("🔊 소리 크게")
        self.volume_down_btn = QPushButton("🔉 소리 작게")
        self.prev_track_btn = QPushButton("⏮ 이전 노래")
        self.next_track_btn = QPushButton("⏭ 다음 노래")

        for btn in [self.volume_up_btn, self.volume_down_btn,
                    self.prev_track_btn, self.next_track_btn]:
            btn.setFixedSize(140, 40)

        self.volume_up_btn.clicked.connect(lambda: print("🔊 소리 크게"))
        self.volume_down_btn.clicked.connect(lambda: print("🔉 소리 작게"))
        self.prev_track_btn.clicked.connect(lambda: print("⏮ 이전 노래"))
        self.next_track_btn.clicked.connect(lambda: print("⏭ 다음 노래"))

        control_layout = QVBoxLayout()
        control_layout.addWidget(self.volume_up_btn)
        control_layout.addWidget(self.volume_down_btn)
        control_layout.addSpacing(20)
        control_layout.addWidget(self.prev_track_btn)
        control_layout.addWidget(self.next_track_btn)
        control_layout.addStretch()

        control_group = QGroupBox("제어")
        control_group.setLayout(control_layout)

        # 메인 수평 레이아웃 -------------------------------
        main_layout = QHBoxLayout()
        main_layout.setContentsMargins(20, 60, 20, 20)
        main_layout.addLayout(left_layout, 3)
        main_layout.addWidget(control_group, 1)
        self.setLayout(main_layout)

        # 상태 업데이트 타이머
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_status)
        self.timer.start(2000)

        self.resizeEvent = self.on_resize

    def on_resize(self, event):
        # 종료 버튼 위치 (우상단 고정)
        self.close_button.move(self.width() - 50, 20)
        self.close_button.raise_()

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
    win.showFullScreen()
    sys.exit(app.exec_())
