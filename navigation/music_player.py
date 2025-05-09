import sys
import os
import time
import serial
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QVBoxLayout, QPushButton, QHBoxLayout
)
from PyQt5.QtGui import QPixmap, QFont
from PyQt5.QtCore import Qt
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
class MusicScreen(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("노래 플레이어")
        self.setGeometry(0, 0, 800, 480)
        self.setStyleSheet("background-color: #d6eaf8;")  # 연한 하늘색

        # DFPlayer 시리얼 초기화
        try:
            self.ser = serial.Serial("/dev/serial0", 9600, timeout=1)
            time.sleep(2)
        except Exception as e:
            print("시리얼 연결 실패:", e)
            self.ser = None

        # 곡 목록
        self.songs = {
            1: {"title": "babyshark", "image": os.path.join(BASE_DIR, "icons/babyshark.jpg")},
            2: {"title": "bbororo", "image": os.path.join(BASE_DIR, "icons/bbororo.jpg")},
            3: {"title": "totoro", "image": os.path.join(BASE_DIR, "icons/totoro.jpg")}
        }
        self.current_track = 1

        # 앨범 이미지
        self.image_label = QLabel()
        self.image_label.setAlignment(Qt.AlignCenter)

        # 곡 제목
        self.title_label = QLabel()
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setFont(QFont("Arial", 28, QFont.Bold))

        # 이전/다음 버튼
        self.prev_btn = QPushButton("⏮ 이전 곡")
        self.next_btn = QPushButton("⏭ 다음 곡")
        for btn in [self.prev_btn, self.next_btn]:
            btn.setFixedHeight(60)
            btn.setStyleSheet("font-size: 20px;")
        self.prev_btn.clicked.connect(self.play_prev)
        self.next_btn.clicked.connect(self.play_next)

        # 종료 버튼
        self.close_button = QPushButton("X", self)
        self.close_button.setFixedSize(40, 40)
        self.close_button.setStyleSheet("""
            QPushButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                font-weight: bold;
                font-size: 20px;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #c0392b;
            }
        """)
        self.close_button.clicked.connect(self.close)

        # 메인 레이아웃
        layout = QVBoxLayout()
        layout.addWidget(self.image_label)
        layout.addWidget(self.title_label)

        btn_layout = QHBoxLayout()
        btn_layout.addWidget(self.prev_btn)
        btn_layout.addWidget(self.next_btn)
        layout.addLayout(btn_layout)

        self.setLayout(layout)
        self.play_song(self.current_track)

    def resizeEvent(self, event):
        self.close_button.move(self.width() - 60, 20)
        self.close_button.raise_()

    def send_dfplayer_command(self, command, param=0):
        if not self.ser:
            return
        high_byte = (param >> 8) & 0xFF
        low_byte = param & 0xFF
        data = [0x7E, 0xFF, 0x06, command, 0x00, high_byte, low_byte, 0xEF]
        checksum = -(sum(data[1:7])) & 0xFFFF
        data.insert(7, (checksum >> 8) & 0xFF)
        data.insert(8, checksum & 0xFF)
        try:
            self.ser.write(bytes(data))
        except Exception as e:
            print("DFPlayer 전송 오류:", e)

    def play_song(self, track):
        song = self.songs.get(track)
        if not song:
            return
        self.title_label.setText(song["title"])

        if os.path.exists(song["image"]):
            pixmap = QPixmap(song["image"]).scaled(320, 320, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            self.image_label.setPixmap(pixmap)
        else:
            self.image_label.setText("이미지 없음")
            self.image_label.setPixmap(QPixmap())

        self.send_dfplayer_command(0x03, track)

    def play_next(self):
        self.current_track += 1
        if self.current_track > len(self.songs):
            self.current_track = 1
        self.play_song(self.current_track)

    def play_prev(self):
        self.current_track -= 1
        if self.current_track < 1:
            self.current_track = len(self.songs)
        self.play_song(self.current_track)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = MusicScreen()
    win.showFullScreen()
    sys.exit(app.exec_())

