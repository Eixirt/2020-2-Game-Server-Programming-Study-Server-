#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_PORT       3500

WSABUF wsaBuf;
SOCKET client_socket; // send�� recv�� �����������Ƿ� ���������ʴ� �����ʹ� ����ó��
char messageBuffer[MAX_BUFFER]; // recv_complete�� �� �� �����Ƿ� �������� �ű�


void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags);

void CALLBACK send_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags) {
	if (bytes > 0) {
		cout << "TRACE - Send message : " << messageBuffer << "(" << bytes << " bytes)\n";
	}
	else {
		closesocket(client_socket);
		return;
	}

	wsaBuf.len = MAX_BUFFER; // ���������� �˼������� MAX_BUFFEr
	ZeroMemory(over, sizeof(*over));

	int ret = WSARecv(client_socket, &wsaBuf, 1, NULL, &flags, over, recv_complete);

}


void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags) {
	if (bytes > 0) {
		messageBuffer[bytes] = 0;
		cout << "TRACE - Receive message : " << messageBuffer << "(" << bytes << " bytes)\n";
	}
	else { // else�� ������ �����ٴ� �ǹ�
		closesocket(client_socket);
		return;
	}
	wsaBuf.len = bytes; // send �� bytes��ŭ �������ϳ� wsaBuf�� max_buffer�� �������.
	ZeroMemory(over, sizeof(*over)); // overlapped ����ü �ʱ�ȭ�ؾ���
	int ret = WSASend(client_socket, &wsaBuf, 1, NULL, NULL, over, send_complete);
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // flag�� �ٲ����
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	::bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
	listen(listenSocket, 5);
	SOCKADDR_IN client_addr;

	WSAOVERLAPPED overlapped;

	while (true) {
		int addr_size = sizeof(client_addr);
		client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size); // listenSocket�� overlapped �����̸� ���� ���ϵ� overlapped�����̹Ƿ� �����ʿ�x
		// char messageBuffer[MAX_BUFFER]; // �̷������� �ڵ� X. ���α׷��� ����Ǵµ��� ����־�� �ϹǷ� �������� or �������� �ű��
		// WSABUF wsaBuf; // �굵 ��������
		// WSAOVERLAPPED overlapped; // �굵 ���󰡸�ȵ�

		wsaBuf.buf = messageBuffer; // ���� ������ �ּ�
		wsaBuf.len = MAX_BUFFER;
		DWORD flags = 0; // 0���� �ʱ�ȭ�ؾ��� �����������˼�������
		ZeroMemory(&overlapped, sizeof(overlapped)); // err_display�� ����غ��� ��������

		int ret = WSARecv(client_socket, &wsaBuf, 1, NULL, &flags, &overlapped, recv_complete);
		
		//if (receiveBytes > 0) { // recv���ڸ��� ������� �ʰ� ��ٷ�����
		//	messageBuffer[receiveBytes] = 0;
		//	cout << "TRACE - Receive message : " << messageBuffer << "(" << receiveBytes << " bytes)\n";
		//}
		//else break;
		//int sendBytes = send(client_socket, messageBuffer, receiveBytes, 0);

	}
	closesocket(listenSocket);
	WSACleanup();
}
