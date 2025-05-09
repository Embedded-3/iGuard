from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QGridLayout, QPushButton, QVBoxLayout, QHBoxLayout
)
from PyQt5.QtGui import QPixmap, QPainter, QPainterPath
from PyQt5.QtCore import Qt, QRectF
import sys, subprocess, os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

class RoundImageButton(QLabel):
    def __init__(self, image_path, size, callback):
        super().__init__()
        self.setFixedSize(size, size)
        self.setCursor(Qt.PointingHandCursor)
        self.callback = callback
        self.setPixmap(self._get_rounded_pixmap(image_path, size))
        self.setStyleSheet("border: 2px solid #3498db; border-radius: 20px;")

    def _get_rounded_pixmap(self, path, size):
        pixmap = QPixmap(path).scaled(size, size, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation)
        rounded = QPixmap(size, size)
        rounded.fill(Qt.transparent)

        painter = QPainter(rounded)
        painter.setRenderHint(QPainter.Antialiasing)
        path = QPainterPath()
        path.addRoundedRect(QRectF(0, 0, size, size), 20, 20)
        painter.setClipPath(path)
        painter.drawPixmap(0, 0, pixmap)
        painter.end()
        return rounded

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.callback()

class Dashboard(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("차량 대시보드")
        self.setStyleSheet("background-color: #d6eaf8;")
        self.setGeometry(0, 0, 800, 480)

        self.ac_on = False  # 에어컨 상태 초기화

        # 전체 레이아웃
        main_layout = QHBoxLayout()
        main_layout.setSpacing(40)

        # 앱 버튼 그리드 (2x2)
        button_grid = QGridLayout()
        button_grid.setHorizontalSpacing(30)
        button_grid.setVerticalSpacing(30)

        buttons = [
            ("전화", os.path.join(BASE_DIR, "icons/phonecall.png"), None),
            ("지도", os.path.join(BASE_DIR, "icons/navermap.png"), None),
            ("카카오톡", os.path.join(BASE_DIR, "icons/kakaotalk.jpg"), None),
            ("노래", os.path.join(BASE_DIR, "icons/babyshark.jpg"), os.path.join(BASE_DIR, "music_player.py")),
        ]

        positions = [(0, 0), (0, 1), (1, 0), (1, 1)]
        for (name, img_path, script_path), (row, col) in zip(buttons, positions):
            if script_path:
                callback = lambda path=script_path: self.launch_app(path)
            else:
                callback = lambda: None
            btn = RoundImageButton(img_path, 120, callback)
            button_grid.addWidget(btn, row, col, alignment=Qt.AlignCenter)

        # 에어컨 버튼들
        ac_layout = QVBoxLayout()
        ac_layout.setSpacing(15)

        # AC ON/OFF 토글 버튼
        self.ac_btn = QPushButton("A/C ON")
        self.ac_btn.setFixedSize(140, 40)
        self.ac_btn.setStyleSheet(self._button_style())
        self.ac_btn.clicked.connect(self.toggle_ac)
        ac_layout.addWidget(self.ac_btn)

        # FAN 세기 버튼들
        for level in ["LOW", "MID", "HIGH"]:
            btn = QPushButton(level)
            btn.setFixedSize(140, 40)
            btn.setStyleSheet(self._button_style())
            btn.clicked.connect(lambda _, lvl=level: print(f"FAN {lvl}"))
            ac_layout.addWidget(btn)

        ac_layout.addStretch()

        # 레이아웃 통합
        main_layout.addLayout(button_grid)
        main_layout.addLayout(ac_layout)
        self.setLayout(main_layout)

    def _button_style(self):
        return """
            QPushButton {
                background-color: #ffffff;
                border: 2px solid #2980b9;
                border-radius: 10px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #d6eaf8;
            }
        """

    def toggle_ac(self):
        self.ac_on = not self.ac_on
        state = "ON" if self.ac_on else "OFF"
        self.ac_btn.setText(f"A/C {state}")
        print(f"에어컨 상태: {state}")

    def launch_app(self, path):
        subprocess.Popen(["/usr/bin/python3", path])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = Dashboard()
    win.showFullScreen()
    sys.exit(app.exec_())

