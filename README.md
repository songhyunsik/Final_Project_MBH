# Final_Project_MBH
Final_Project_MBH ....

## JBK brunch 작성자 : 조보근

## 2024.07.02
    - raspberry putty 연결
    - iptime을 통한 인터넷 연결시
        - 192.168.0.1 >> login >> raspberry ip adress확인
        - 라우터에 raspberry(192.168.0.XX)에 연결 가능 설정
        - 이후 putty 에 raspberry ip(!= 192.168.0.XX)기입 후 실행

    - 스마트미러용 raspberryPi 5에 서버코드 운영
    - pthread를 사용하지 않고 서버 코드를 3개 사용하여 운영
    - 받은 데이터는 json파일로 기입하여 내부 데이터 활용 혹은 .json으로 받아서 운영
    - 계층
        - iot기기
        - smartMirror
        - cloud
