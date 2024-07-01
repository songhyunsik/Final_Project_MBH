# Final_Project_MBH
Final_Project_MBH ....
팀장 : 정현우
팀원 : 강민우
팀원 : 조보근

# 2024-06-20[목]
## 파이널 프로젝트 시작
    - 프로젝트 깃허브 제작
    - ppt 링크 추가
    - 아이디어 회의 시작

## 회의 내용
    - 프로젝트명 : SmartMirror
        - 목적 : 일상생활에서 쉽게 접근 가능한 거울로 데이터 제공
                 유저가 원하는 생활데이터 분석 및 제공
                 날씨, 주요기사, 스포츠데이터 등 생활 데이터 제공
        - 기능 : 올림픽, 날씨(생활) 정보 출력
        - Display 구성품
            - 디스플레이, OS(라즈베리파이 + WIFI), 거치, 거울필름, 적외선 틀(IR 터치 프레임), Chat GPT 4.0
            - 1. 올림픽 데이터, 데이터 분석
            - 2. 기상청 API, 지역 선택, 날씨 정보 선택, 구글 어시스턴트
        - 코드 분류 
            - 1. SmartMirror
            - 2. Soket
            - 3. Display
            - 4. User
            - 5. 목적에 따른 코드
        - 기타 : 행정, 품의서, 회의보고서

- 구매할 재료 선정 후, 품의서 작성

# 2024-06-21[금]
## 회의 내용
    - 디스플레이 재료 최종 결정
        - 1. 디스플레이 사이즈
        - 2. OS
        - 3. 거울 필름
        - 4. 적외선 틀

## 스마트 미러 구상도

```markdown
---------------         --------------- 
| 나라1 나라2  |        |시간, 지역, 날씨|
|             |         |             |
|             |         |             |
|             |         |             |
|             |         |             |
|             |         |             |
|             |         |             |
|-------------|         |-------------|
|  예측분석    |         |  주요 뉴스  |
|             |         |             |
---------------         --------------- 
```
- 품의서 제출

# 2024-06-24[월]
## 회의 내용
--------------------------------------------
- 기능 1 올림픽
    - (0) 나라 : 한국
    - (1) 종목 : 한국대표 주중 3~5
    - (2) Data : KUIS 올림픽 자료
    - (3) Analysis : 예측 순위 <-- py
    - (4) numpy, plot, else... <-- py
--------------------------------------------
- 기능 2 생활
    - (1) 종류 : 날씨, 주요기사
    - (2) 구동 : OpenCV <-- C++
    - (3) Data : 기상청 API, 주요기사 <-- py
--------------------------------------------

## 유저 선택 기능
- DB or SQL ...?
- cvs, TXT 파일로도...?
- read_cvs --> file 방식으로, 유저의 정보를 저장한다. 에 대한 아이디어 점검
```markdown
-----------------------------
| 유저(1) | 기능 | 세부 기능 |
| 유저(2) | 기능 | 세부 기능 |
| 유저(3) | 기능 | 세부 기능 |
| 유저(4) | 기능 | 세부 기능 |
-----------------------------
```
--> 유저, 기능, 세부기능을 담당하는 변수를 지정한다.

- 재료 주문 완료(2~3일 정도 소요)

# 2024-06-25[화]

```markdown

	                 main.cpp
  		            |
   		            |
   		            |
|------------>[유저선택]-----|
|           |  		    v
|           |	        while(ON)
|           | 
|-----------v		  (pg)

 		         exit()
   		           |
   		           |
   	   	           |
   		           |
   		           v

	               return 0;
```

- 현 진행사황
- 가. SRC
    - 소켓 블루투스(BlueZ)
    - 멀티쓰레드 블루투스
    - 자료 저장, 변수 기기 조
    
# 2024-06-26[수]

- 맥시머스 특강(1 : 1 진행, 특강과 프로젝트 병행)
    - -> 특강 취소, 프로젝트에 집중

- 라즈베리파이 2 제로 도착 및 os 설치
    - 제로 제어를 위한 장비 추가 구매

# 2024-06-27[목]

- 이은하 강사님 특강
- 디바이스마트 제품 모두 도착 및 재점검
- 라즈베리파이 2W Zero + 온습도 센서(DHT22) 연결 후 센서 구동 테스트

# 2024-06-28[금]

- 라즈베리파이 2W Zero + 온습도 센서(DHT22) 결과값 받기 성공

- 라즈베리파이 5 하나 태움
    - 재주문 필요

- 스마트 미러 키드 조립
    - 라즈베리파이 5으로 대신 구동

# 2024-07-01[월]

- 스마트미러 키트 재조립 시작.

# 2024-07-02[화]

# 2024-07-03[수]