#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/msg.h>
#include <sts/ipc.h>

// 192.168.1.128, 192.168.1.145

int mmi_socket_open(void) {
	int mmi_server_socket;
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(9000)
	};
	mmi_server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (mmi_server_socket < 0) return -1;

	if (bind(mmi_server_socket, (struct sockaddr*)&serv_addr), sizeof serv_addr) < 0) return -2;

	if (listen(mmi_server_socket, 5) < 0) return -3;

	return mmi_server_socket;
}

int stdby_socket_open(void) {
	int stdby_server_socket;
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(9001)
	};
	stdby_server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (stdby_server_socket < 0) return -1;

	if (bind(stdby_server_socket, (struct sockaddr*)&serv_addr), sizeof serv_addr) < 0) return -2;

	if (listen(stdby_server_socket, 5) < 0) return -3;

	return stdby_server_socket;
}


int mmi_server_worker(int clnt_sock, char *buf) {
	int i = 0;
	char* arg[4] = { "", "", "" };
	char send_buf[2048] = "";

	int qid,len;
	
	// �ƱԸ�Ʈ ��ū �и�
	for (char* p = strtok(buf, "\n"); p; p = strtok(NULL, "\n")) {
		arg[i++] = p;
	}
	printf("��ū �и� ����\n");

	struct msgq_data {
		long type;
		char text[2048];
	};

	if ((qid = msgget((key_t)0111, IPC_CREAT | 0666)) == -1) {
		printf("�޽��� ť ���� ����\n");
	}


	

	//�ƱԸ�Ʈ �� ��� ����
	if (strcmp(arg[0], "DIS-RESOURCE")) {
		if (strcmp(arg[1], "MEMORY")) {
			struct msgq_data send_data = { 1, arg[1] };
			struct msgq_data recv_data;
			if (msgsnd(qid, &send_data, strlen(send_data.text), 0) == -1) {
				printf("�޽��� ť ���� ����\n");
			}
			if ((len = msgrcv(qid, &recv_data, 100, 0, 0)) == -1)
			{
				printf("�޽��� ť ���� ����\n");
			}
			send_buf = recv_data.text;
			if (msgctl(qid, IPC_RMID, 0) == -1) {
				printf(("msgctl ����\n");
			}
		}
		else if (strcmp(arg[1], "DISK")) {
			struct msgq_data send_data = { 1, arg[1] };
			struct msgq_data recv_data;
			if (msgsnd(qid, &send_data, strlen(send_data.text), 0) == -1) {
				printf("�޽��� ť ���� ����\n");
			}
			if ((len = msgrcv(qid, &recv_data, 100, 0, 0)) == -1)
			{
				printf("�޽��� ť ���� ����\n");
			}
			send_buf = recv_data.text;
			if (msgctl(qid, IPC_RMID, 0) == -1) {
				printf(("msgctl ����\n");
			}
		}
		else if (strcmp(arg[1], "CPU")) {
			struct msgq_data send_data = { 1, arg[1] };
			struct msgq_data recv_data;
			if (msgsnd(qid, &send_data, strlen(send_data.text), 0) == -1) {
				printf("�޽��� ť ���� ����\n");
			}
			if ((len = msgrcv(qid, &recv_data, 100, 0, 0)) == -1)
			{
				printf("�޽��� ť ���� ����\n");
			}
			send_buf = recv_data.text;
			if (msgctl(qid, IPC_RMID, 0) == -1) {
				printf(("msgctl ����\n");
			}
		}
		else {
			send_buf = "�߸��� ��ɾ��Դϴ�.\n";
		}

	}
	else if (strcmp(arg[0], "DIS-SW-STS")) {

	}
	write(clnt_sock, send_buf, strlen(send_buf));
	close(clnt_sock);
}

int main(void) {
	// ���� ����, accept�� ��� �� ���� ����, read�� ��� �� ���� ����
	int mmi_client_socket;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	int mmi_server_socket = mmi_socket_open(), recv_len; // ���� ���� �Լ�
	char buf[2048];

	switch (mmi_server_socket)
	{
	case -1: printf("���� ���� ����\n"); return 1;
	case -2: printf("���ε� ����\n"); return 1;
	case -3: printf("listen ����\n"); return 1;
	}

	while (1) {
		clnt_addr_size = sizeof(clnt_addr);
		mmi_client_socket = accept(mmi_server_socket, (struct sockaddr*)&clnt_addr_size, &clnt_addr_size);
		printf("mmi ���� ����\n");
		recv_len = read(mmi_client_socket, buf, sizeof buf);
		if (recv_len < 0) continue;
		buf[recv_len] = '\0';
		printf(buf);

		mmi_server_worker(mmi_client_socket, buf); //���� ��ɾ� ���� �ڵ�


	}
}

