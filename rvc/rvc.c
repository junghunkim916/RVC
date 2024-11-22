#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // sleep 함수 사용
#include <string.h>

// 글로벌 변수로 센서 값을 저장
bool frontSensorValue = false;
bool leftSensorValue = false;
bool rightSensorValue = false;
bool dustSensorValue = false; // 기본값은 먼지가 있다고 가정

typedef struct {
    bool frontobstacle;
    bool leftobstacle;
    bool rightobstacle;
} Obstacle;

char* Power = "ON"; // ON, OFF, UP
char* motorCommand = "Moving forward...\n";

bool frontSensor();
bool leftSensor();
bool rightSensor();
bool dustSensor();

Obstacle determineObstacleLocation();
bool determineDustExistence();

// 모터 동작
void moveForward();
void turnLeft();
void turnRight();
void moveBackward();
void stop();

// 클리닝 파워 제어
void cleaningPower(char* power);

// 메인 컨트롤러
void Controller(char inputLines[][5], int numLines);
void* powerUp(void* arg);
//output Interface
void motorInterface(char* motorCommand);
void cleanerInterface(char* power);

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // 버퍼에 남아 있는 데이터 제거
}

int main(int argc, char* argv[]) {

    char inputLines[100][5]; // 최대 100줄의 입력값 저장
    int numLines = 0;
    printf("Input lines in the format\n(front sensor input, left sensor input, right sensor input, dust sensor input), each represented by T or F (end with 'END'):\n");
    while (1) {
        char buffer[10];
        fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "END", 3) == 0) break; // "STOP"이 입력되면 종료

        if (sscanf(buffer, "%s %s %s %s", 
                   inputLines[numLines], inputLines[numLines] + 1, 
                   inputLines[numLines] + 2, inputLines[numLines] + 3) == 4) {
            numLines++;
        }
    }
    Controller(inputLines, numLines); // Controller 호출 
    return 0;
}

void Controller(char inputLines[][5], int numLines) {
    Power = "ON"; // 초기 상태는 "ON"
    cleaningPower(Power);
    moveForward(); // 초기 상태는 전진

    for (int i = 0; i < numLines; i++) { // 입력된 각 줄을 한 번씩 처리
        printf("Tick\n");
        // 현재 줄의 데이터를 센서 값으로 설정
        frontSensorValue = (inputLines[i][0] == 'T');
        leftSensorValue = (inputLines[i][1] == 'T');
        rightSensorValue = (inputLines[i][2] == 'T');
        dustSensorValue = (inputLines[i][3] == 'T');

        // 장애물 감지
        Obstacle obstacleDetected = determineObstacleLocation();
        bool F = obstacleDetected.frontobstacle;
        bool L = obstacleDetected.leftobstacle;
        bool R = obstacleDetected.rightobstacle;
        bool Dust = determineDustExistence();

        if (strcmp(motorCommand, "Moving Backward...\n") == 0) {
            if (!L) {
                Power = "OFF";
                cleaningPower(Power);
                turnLeft();
            } else if (L && !R) {
                Power = "OFF";
                cleaningPower(Power);
                turnRight();
            } else {
                Power = "OFF";
                cleaningPower(Power);
                moveBackward();
            }
        } else {
            if (!F) {
                if (!Dust && (strcmp(Power, "UP") == 0)) {
                    printf("Dust Cleared\n");
                    Power = "ON";
                    cleaningPower(Power);
                }

                Power = "ON";
                cleaningPower(Power);
                moveForward();

                if (Dust && (strcmp(Power, "ON") == 0)) {
                    printf("Dust Detected\n");
                    Power = "UP";
                    cleaningPower(Power);
                }
            } else {
                if (F && !L) {
                    Power = "OFF";
                    cleaningPower(Power);
                    turnLeft();
                } else if (F && L && !R) {
                    Power = "OFF";
                    cleaningPower(Power);
                    turnRight();
                } else if (F && L && R) {
                    Power = "OFF";
                    cleaningPower(Power);
                    stop();
                }
            }
        }

        sleep(1); // 1초 대기
    }
}

Obstacle determineObstacleLocation() {
    Obstacle obstacle;
    obstacle.frontobstacle = frontSensor();
    obstacle.leftobstacle = leftSensor();
    obstacle.rightobstacle = rightSensor();
    return obstacle;
}

bool determineDustExistence() {
    return dustSensorValue; // 먼지 센서 값
}

bool frontSensor() {
    return frontSensorValue; // 입력받은 값을 반환
}

bool leftSensor() {
    return leftSensorValue; // 입력받은 값을 반환
}

bool rightSensor() {
    return rightSensorValue; // 입력받은 값을 반환
}

bool dustSensor() {
    return dustSensorValue; // 먼지 센서 값
}

void moveForward() {
    motorCommand = "Moving forward...\n";
    motorInterface(motorCommand);
}

void turnLeft() {
    motorCommand = "Turn Left for 5 Tick...\n";
    sleep(5);
    motorInterface(motorCommand);
}

void turnRight() {
    motorCommand = "Turn Right for 5 Tick...\n";
    sleep(5);
    motorInterface(motorCommand);
}

void stop() {
    printf("Stop...\n");
    moveBackward();
}

void moveBackward() {
    motorCommand = "Moving Backward...\n";
    motorInterface(motorCommand);
}

void cleaningPower(char* power) {
    cleanerInterface(power);
}

void motorInterface(char* motorCommand) {
    printf("%s", motorCommand);
}

void cleanerInterface(char* power) {
    printf("Cleaning Power %s\n", power);
}
