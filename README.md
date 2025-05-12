🚗 iGuard – 차량 내 아동 방치 방지 시스템
iGuard는 보호자의 실수로 차량에 방치된 아이를 보호하기 위해 설계된 시스템입니다. 이 프로젝트는 현대오토에버 모빌리티 스쿨 임베디드 4기 3조(김서진, 김호준, 박주현, 박해웅, 배정우, 조현호)에 의해 2025년 4월 25일부터 5월 12일까지 10일간 구현되었습니다.

📦 시스템 구성
1. 구성 모듈 소개
모듈명	사용 보드	주요 역할 및 기능	사용 센서/액추에이터
stm32_boot	STM32 Nucleo-L073RZ	- 센서 데이터 수집(압력, 초음파, 적외선)
- CAN 메시지 전송: 수집 데이터, Wake-up 트리거(0x1), Sleep 트리거(0x2)	적외선 PIR 모션 센서, 초음파 센서, 압력 센서
stm32_IVI	STM32 Nucleo-L073RZ	- CAN 메시지 수신 후 처리: 노래 재생(0x5), 일시 정지(0x6), LCD 메시지 출력(0x10)	TextLcd Shield, DFPlayer Mini
tc275_hvac	Infineon TC275	- 공조(HVAC) 시스템 제어 및 센서 상태 응답(0x120, 0x150)	모터
RaspberryPi4	Raspberry Pi 4	- 내부 사진 캡처 후 앱에 전송
- CAN ↔ MQTT 메시지 브리지 역할	카메라

2. CAN 통신
ID-Format: CAN Standard

DLC: 8 Byte

메시지 이름	ID	목적 및 설명	송신자
Wakeup_Message	0x01	Wake-up 트리거 CAN 메시지	stm32_boot
Sleep_Message	0x02	Sleep 트리거 메시지 전송	stm32_boot
DFPlayer_StartMusic	0x05	음악 재생	RaspberryPi4
DFPlayer_EndMusic	0x06	음악 정지	RaspberryPi4
DFPlayer_ResumeMusic	0x07	음악 재개	RaspberryPi4
DFPlayer_VolumeUp	0x08	볼륨 증가	RaspberryPi4
DFPlayer_VolumeDown	0x09	볼륨 감소	RaspberryPi4
TextLcd_Message	0x10	LCD 화면에 출력할 문자열 전송	RaspberryPi4
HvacControl_Message	0x17	팬 모드 및 속도 변경 (Driving mode)	RaspberryPi4
AirCondition_Message	0x20	HVAC 시스템 Wake-up, 명령 수행 및 상태 응답 (0x120, 0x150)	tc275_hvac
TempHumid_Message	0x21	CAN → MQTT 메시지 브리지 및 명령 수신 후 CAN 전송	tc275_hvac

3. MQTT 통신
통신 주체: 라즈베리 파이와 서버 간의 통신

Topic	목적 및 설명	Publish	Subscribe
Car/image	차량 내부 캡처 이미지 전송	RaspberryPi4	스마트폰 앱
Car/trigger	차량 내부 캡처 이미지 요청	스마트폰 앱	RaspberryPi4
Car/LCD	LCD 내용 입력 메시지	RaspberryPi4	RaspberryPi4
Car/music	음악 실행 및 정지	스마트폰 앱	RaspberryPi4
Car/inside	차량 내부 온도, 습도 정보	RaspberryPi4	스마트폰 앱
Car/outside	차량 외부 대기질 정보	RaspberryPi4	스마트폰 앱
Car/detection	차량 내부 움직임 발생 알림	RaspberryPi4	스마트폰 앱

🔧 기술 스택
MCU: STM32 Nucleo-L073RZ

임베디드 시스템: Infineon TC275

싱글 보드 컴퓨터: Raspberry Pi 4

통신 프로토콜: CAN, MQTT

센서 및 액추에이터: 적외선 PIR 모션 센서, 초음파 센서, 압력 센서, TextLcd Shield, DFPlayer Mini, 모터, 카메라

📁 프로젝트 구조
복사
편집
iGuard/
├── stm32_boot/
├── stm32_IVI/
├── tc275_hvac/
└── raspberry_pi/
📸 데모 및 시연 영상
iGuard 시연 영상 보기
(실제 링크를 삽입해주세요)

👥 팀원 소개
김서진

김호준

박주현

박해웅

배정우

조현호

📄 라이선스
이 프로젝트는 MIT 라이선스를 따릅니다.