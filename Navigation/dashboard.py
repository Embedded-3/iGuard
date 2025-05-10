import sys
import os
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QGridLayout, QPushButton, QVBoxLayout, QHBoxLayout
)
from PyQt5.QtGui import QPixmap, QPainter, QPainterPath, QFont, QPen, QColor
from PyQt5.QtCore import Qt, QRectF
from music_player import MusicScreen
from can_handler import send_can_message

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

class RoundImageButton(QLabel):
    def __init__(self, image_path, size, callback):
        super().__init__()
        self.setFixedSize(size, size)
        self.setCursor(Qt.PointingHandCursor)
        self.callback = callback
        self.image_path = image_path
        self.size = size
        self.update_pixmap()

    def update_pixmap(self):
        rounded = QPixmap(self.size, self.size)
        rounded.fill(Qt.transparent)

        painter = QPainter(rounded)
        painter.setRenderHint(QPainter.Antialiasing)

        path = QPainterPath()
        path.addRoundedRect(QRectF(0, 0, self.size, self.size), 20, 20)
        painter.setClipPath(path)

        pixmap = QPixmap(self.image_path).scaled(
            self.size, self.size, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation
        )
        painter.drawPixmap(0, 0, pixmap)

        pen = QPen(QColor("#4682b4"), 3)
        painter.setPen(pen)
        painter.setBrush(Qt.NoBrush)
        painter.drawPath(path)

        painter.end()
        self.setPixmap(rounded)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.callback()

class Dashboard(QWidget):
    def __init__(self, bus):
        super().__init__()
        self.bus = bus
        self.setWindowTitle("차량 대시보드")
        self.setStyleSheet("background-color: #e6f2fa;")
        self.setGeometry(0, 0, 800, 480)

        main_layout = QHBoxLayout()
        main_layout.setSpacing(50)
        main_layout.setContentsMargins(40, 40, 40, 40)

        button_grid = QGridLayout()
        button_grid.setHorizontalSpacing(40)
        button_grid.setVerticalSpacing(40)

        buttons = [
            ("전화", os.path.join(BASE_DIR, "icons/phonecall.png"), None),
            ("지도", os.path.join(BASE_DIR, "icons/navermap.png"), None),
            ("카카오톡", os.path.join(BASE_DIR, "icons/kakaotalk.jpg"), None),
            ("노래", os.path.join(BASE_DIR, "icons/babyshark.jpg"), "music"),
        ]

        positions = [(0, 0), (0, 1), (1, 0), (1, 1)]
        for (name, img_path, app_id), (row, col) in zip(buttons, positions):
            if app_id:
                callback = lambda aid=app_id: self.launch_app(aid)
            else:
                callback = lambda: None
            btn = RoundImageButton(img_path, 130, callback)
            label = QLabel(name)
            label.setAlignment(Qt.AlignCenter)
            label.setFont(QFont("Arial", 12, QFont.Bold))
            label.setStyleSheet("color: #1e3f66;")

            vbox = QVBoxLayout()
            vbox.addWidget(btn)
            vbox.addWidget(label)

            container = QWidget()
            container.setLayout(vbox)
            button_grid.addWidget(container, row, col, alignment=Qt.AlignCenter)

        ac_layout = QVBoxLayout()
        ac_layout.setSpacing(20)

        ac_label = QLabel("\u2744\ufe0f A/C Control")
        ac_label.setAlignment(Qt.AlignCenter)
        ac_label.setFont(QFont("Arial", 14, QFont.Bold))
        ac_label.setStyleSheet("color: #1e3f66;")
        ac_layout.addWidget(ac_label)

        self.ac_btn = QPushButton("A/C OFF")
        self.ac_btn.setFixedSize(150, 45)
        self.ac_btn.setStyleSheet(self._button_style())
        self.ac_btn.clicked.connect(lambda: self.send_ac_message(0))
        ac_layout.addWidget(self.ac_btn)

        for level, code in zip(["LOW", "MID", "HIGH"], [1, 2, 3]):
            btn = QPushButton(level)
            btn.setFixedSize(150, 45)
            btn.setStyleSheet(self._button_style())
            btn.clicked.connect(self.make_ac_callback(code))
            ac_layout.addWidget(btn)

        ac_layout.addStretch()

        main_layout.addLayout(button_grid, 2)
        main_layout.addLayout(ac_layout, 1)
        self.setLayout(main_layout)

    def _button_style(self):
        return """
            QPushButton {
                background-color: #ffffff;
                border: 2px solid #4682b4;
                border-radius: 10px;
                font-size: 14px;
                font-weight: bold;
                color: #1e3f66;
            }
            QPushButton:hover {
                background-color: #deeffa;
            }
        """

    def make_ac_callback(self, code):
        return lambda _: self.send_ac_message(code)

    def send_ac_message(self, level_code):
        can_id = 0x31
        data = [level_code] + [0] * 7
        send_can_message(self.bus, can_id, data)

    def launch_app(self, app_name):
        if app_name == "music":
            self.music_window = MusicScreen(self.bus)
            self.music_window.showFullScreen()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = Dashboard(bus=None)  # 테스트용 None 사용
    win.showFullScreen()
    sys.exit(app.exec_())