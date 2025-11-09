#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLOORS 3           // 주차장 층수
#define PER_FLOOR 40 // 층별 주차 공간 (A:10, B:10, C:10, D:10)


// 주차 공간 하나를 나타내는 구조체
typedef struct {
    int isOccupied;         // 0: 비어있음, 1: 차있음
    char Number[10]; // 차량 번호 (예: "12가3456")
    time_t entryTime;       // 입차 시간 (time_t 자료형)
} ParkingSpot;

// parkingLot[층][칸]
ParkingSpot parkingLot[FLOORS][PER_FLOOR];

void initializeParking();   // 주차장 초기화 함수
void displayMenu();         // 메인 메뉴 표시 함수
void Entry();        // 차량 입차 함수
void Exit();         // 차량 출차 함수
void ParkingStatus();   // 주차 현황 조회 함수

int main() {
    int choice;

    // 프로그램 시작 시 주차장을 모두 비어있는 상태로 초기화
    initializeParking();

    // 메인 루프 (프로그램이 종료되지 않게)
    while (1) {
        displayMenu();

        printf("\n메뉴를 선택하세요 (1-4): ");
        scanf_s("%d", &choice);

        // 입력 버퍼에 남아있는 \n(엔터) 제거
        while (getchar() != '\n');

        switch (choice) {
        case 1:
            Entry();
            break;
        case 2:
            Exit();
            break;
        case 3:
            ParkingStatus();
            break;
        case 4:
            printf("프로그램을 종료합니다. 감사합니다.\n");
            return 0; // 프로그램 종료
        default:
            printf("! 경고: 1~4 사이의 숫자만 입력하세요.\n");
        }

        printf("\n(계속하려면 Enter 키를 누르세요...)");
        getchar(); // 사용자가 Enter를 누를 때까지 대기
    }

    return 0;
}

// --- 3. 핵심 기능 함수 구현 ---

/*
주차장 배열을 모두 빈 상태(0)로 초기화합니다.
 */
void initializeParking() {
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            parkingLot[i][j].isOccupied = 0;
            strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
            parkingLot[i][j].entryTime = 0;
        }
    }
}

/*
사용자에게 메인 메뉴를 보여줍니다.
 */
void displayMenu() {
    printf("===================================\n");
    printf("   C언어 주차 관리 프로그램 \n");
    printf("===================================\n");
    printf("  1. 차량 입차\n");
    printf("  2. 차량 출차\n");
    printf("  3. 주차 현황 조회\n");
    printf("  4. 프로그램 종료\n");
    printf("===================================\n");
}

/*
전체 주차장을 순회하며 'O'(주차됨) 또는 'X'(비어있음)로 표시합니다.
 */
void ParkingStatus() {
    printf("\n--- [ 3. 주차 현황 조회 ] ---\n");
    printf("( O: 주차 중 / X: 비어있음 )\n");

    for (int i = 0; i < FLOORS; i++) {
        printf("\n--- 지하 %d층 ---\n", i + 1);

        // A 구역 (인덱스 0 ~ 9)
        printf("  A 구역 (1-10): ");
        for (int j = 0; j < 10; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // B 구역 (인덱스 10 ~ 19)
        printf("\n  B 구역 (1-10): ");
        for (int j = 10; j < 20; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // C 구역 (인덱스 20 ~ 29)
        printf("\n  C 구역 (1-10): ");
        for (int j = 20; j < 30; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // D 구역 (인덱스 30 ~ 39)
        printf("\n  D 구역 (1-10): ");
        for (int j = 30; j < 40; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }
        printf("\n---------------------------------\n");
    }
}


/**
1. 차량 입차 기능 (1주차 기본형)
(1주차 목표: 차량 번호 받고, 첫 번째 빈자리에 주차시키기)
 */
void Entry() {
    char plate[10];
    printf("\n--- [ 1. 차량 입차 ] ---\n");
    printf("입차할 차량 번호를 입력하세요 (예: 12가3456): ");
    scanf_s("%s", plate, 10);
    while (getchar() != '\n'); // 버퍼 비우기

    // (2주차 작업: 차량 번호 중복 검사)

    // (1주차: 가장 단순한 방식 - B1-A1부터 순서대로 첫 번째 빈자리 찾기)
    int parked = 0; // 주차 성공 여부 플래그
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            if (parkingLot[i][j].isOccupied == 0) {
                // 이 자리가 비어있다!
                parkingLot[i][j].isOccupied = 1;
                strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
                parkingLot[i][j].entryTime = time(NULL); // <time.h>의 time() 함수로 현재 시간 저장

                printf("\n [주차 완료]\n");
                printf("   차량 번호: %s\n", plate);
                printf("   주차 위치: 지하 %d층, (인덱스 %d번 칸)\n", i + 1, j);

                parked = 1;
                goto entry_success; // 2중 for문을 한 번에 탈출
            }
        }
    }

entry_success:
    if (parked == 0) {
        printf("\n [주차 실패] 모든 주차 공간이 가득 찼습니다.\n");
    }
}


/**
2. 차량 출차 기능 (1주차 기본형)
(1주차 목표: 차량 번호로 검색해서, 차를 빼고(초기화), 총 시간(초) 보여주기)
 */
void Exit() {
    char plate[10];
    printf("\n--- [ 2. 차량 출차 ] ---\n");
    printf("출차할 차량 번호를 입력하세요: ");
    scanf_s("%s", plate, 10);
    while (getchar() != '\n'); // 버퍼 비우기

    int found = 0;
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {

            // isOccupied가 1이고(차량이 있고), 차량 번호가 일치하면
            if (parkingLot[i][j].isOccupied == 1 && strcmp(parkingLot[i][j].Number, plate) == 0) {

                // 1. 차량 발견
                found = 1;

                // 2. 시간 계산 (1주차: 초 단위로만 계산)
                time_t exitTime = time(NULL);
                time_t entryTime = parkingLot[i][j].entryTime;

                // difftime() 함수로 두 시간의 차이를 초(double)로 구함
                double parkedSeconds = difftime(exitTime, entryTime);

                printf("\n [정산 내역]\n");
                printf("   차량 번호: %s\n", parkingLot[i][j].Number);
                printf("   주차 위치: 지하 %d층, (인덱스 %d번 칸)\n", i + 1, j);
                printf("   총 주차 시간: %.0f 초\n", parkedSeconds);

                // 3. 요금 계산 (2주차 작업)
                printf("   (2주차에 이 시간을 기준으로 요금 계산 기능 추가 예정)\n");

                // 4. 데이터 초기화 (차량 빼기)
                parkingLot[i][j].isOccupied = 0;
                strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
                parkingLot[i][j].entryTime = 0;

                printf("\n>> 출차가 완료되었습니다.\n");

                goto exit_success; // 2중 for문 탈출
            }
        }
    }

exit_success:
    if (found == 0) {
        printf("\n [오류] 해당 차량(%s)이 주차장에 없습니다.\n", plate);
    }
}