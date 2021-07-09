#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define BUFSIZE 50 
#define QKEY (key_t)0111 // 메시지 큐 통신을 위한 key값 지정

typedef struct msgq_data {
  long type; //메시지 타입
  char text[BUFSIZE]; //메시지 데이터
} Message;

void main() {
  int qid;
  char tmp[2048];
  char fcnt[BUFSIZE] = "";
  FILE* fp;

  printf("rstat is running\n");
 
  if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) { // msgget으로 System V의 메시지 큐 id를 얻어옵니다. IPC_CREAT로 메시지 큐가 없음 새로 생성합니다.
    perror("msgget failed");
    exit(1);
  }
  while (1) {
    Message recv_data, send_data;
    memset(&recv_data, 0x00, sizeof(recv_data));

    if ((msgrcv(qid, &recv_data, BUFSIZE, 0, 0)) == -1) { //msgrev로  메시지큐 id의 메시지를 하나 읽고 그 메시지를 큐에서 제거합니다.
      perror("msgrcv failed");
      exit(1);
    }

    if (strcmp(recv_data.text, "CPU") == 0) { // recv_data 선언된 구조체 변수에서 수신받는 메시지 데이터를 비교합니다.
      fp = popen("top -n 1 -b | awk '/^%Cpu/{print $2}'", "r"); // 해당 메시지 데이터에 맞는 리눅스 명령어를 popen함수로 읽기 전용으로 넘겨줍니다.
    } else if (strcmp(recv_data.text, "MEMORY") == 0) {
      fp = popen("free | grep Mem | awk '{print $4/$3 * 100.0}'", "r");
    } else if (strcmp(recv_data.text, "DISK") == 0) {
      fp = popen("df|tail -1|tr -s ' '|cut -d ' ' -f5", "r");
    }
    fgets(fcnt, sizeof fcnt, fp); //문자열을 받아옵니다.
    send_data.type = 1; //전송할 데이터 타입을 설정합니다
    sprintf(send_data.text, "Usage>%s", fcnt);
    msgsnd(qid, &send_data, strlen(send_data.text), 0); //메시지를 agtd 서버에 전송해줍니다.
  }
}
