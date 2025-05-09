from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QGridLayout
from PyQt5.QtGui import QPixmap, QPainter, QPainterPath
from PyQt5.QtCore import Qt, QRectF, QSize
import sys, subprocess
import os
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

        layout = QGridLayout()
        layout.setSpacing(30)

        buttons = [
            ("전화", os.path.join(BASE_DIR, "icons/phonecall.png"), None),
            ("카카오톡", os.path.join(BASE_DIR, "icons/kakaotalk.jpg"), None),
            ("지도", os.path.join(BASE_DIR, "icons/navermap.png"), None),
            ("노래", os.path.join(BASE_DIR, "icons/babyshark.jpg"), os.path.join(BASE_DIR, "music_player.py")),
        ]

        positions = [(i // 2, i % 2) for i in range(len(buttons))]

        for (name, img_path, script_path), (row, col) in zip(buttons, positions):
            if script_path:
                callback = lambda path=script_path: self.launch_app(path)
            else:
                callback = lambda: None  # 아무 동작 없음

            btn = RoundImageButton(img_path, 120, callback)
            layout.addWidget(btn, row, col)

        self.setLayout(layout)

    def launch_app(self, path):
        subprocess.Popen(["/usr/bin/python3", path])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = Dashboard()
    win.showFullScreen()
    sys.exit(app.exec_())

