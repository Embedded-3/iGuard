from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QGridLayout
from PyQt5.QtGui import QPixmap, QPainter, QPainterPath
from PyQt5.QtCore import Qt, QRectF, QSize
import sys, subprocess

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

        # 이미지 버튼 (예쁘게 잘린 버튼)
        music_btn = RoundImageButton(
            "/home/jw/Desktop/music_player/babyshark.jpg",
            120,
            self.open_music_player
        )

        layout.addWidget(music_btn, 0, 0)
        self.setLayout(layout)

    def open_music_player(self):
        subprocess.Popen(["/usr/bin/python3", "/home/jw/Desktop/music_player/v.py"])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = Dashboard()
    win.showFullScreen()
    sys.exit(app.exec_())