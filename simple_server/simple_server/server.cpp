#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_PORT       3500

WSABUF wsaBuf;
SOCKET client_socket; // send와 recv가 떨어져있으므로 공유하지않는 데이터는 전역처리
char messageBuffer[MAX_BUFFER]; // recv_complete가 볼 수 없으므로 전역으로 옮김


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

	wsaBuf.len = MAX_BUFFER; // 무슨값들어갈지 알수없으니 MAX_BUFFEr
	ZeroMemory(over, sizeof(*over));

	int ret = WSARecv(client_socket, &wsaBuf, 1, NULL, &flags, over, recv_complete);

}


void CALLBACK recv_complete(DWORD err, DWORD bytes, LPWSAOVERLAPPED over, DWORD flags) {
	if (bytes > 0) {
		messageBuffer[bytes] = 0;
		cout << "TRACE - Receive message : " << messageBuffer << "(" << bytes << " bytes)\n";
	}
	else { // else면 접속을 끊었다는 의미
		closesocket(client_socket);
		return;
	}
	wsaBuf.len = bytes; // send 시 bytes만큼 보내야하나 wsaBuf에 max_buffer가 들어있음.
	ZeroMemory(over, sizeof(*over)); // overlapped 구조체 초기화해야함
	int ret = WSASend(client_socket, &wsaBuf, 1, NULL, NULL, over, send_complete);
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // flag를 바꿔야함
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
		client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size); // listenSocket이 overlapped 소켓이면 받을 소켓도 overlapped소켓이므로 수정필요x
		// char messageBuffer[MAX_BUFFER]; // 이런식으로 코딩 X. 프로그램이 실행되는동안 살아있어야 하므로 전역변수 or 전역으로 옮길것
		// WSABUF wsaBuf; // 얘도 마찬가지
		// WSAOVERLAPPED overlapped; // 얘도 날라가면안됨

		wsaBuf.buf = messageBuffer; // 실제 버퍼의 주소
		wsaBuf.len = MAX_BUFFER;
		DWORD flags = 0; // 0으로 초기화해야함 무슨값들어갈지알수없으니
		ZeroMemory(&overlapped, sizeof(overlapped)); // err_display로 출력해볼것 에러나면

		int ret = WSARecv(client_socket, &wsaBuf, 1, NULL, &flags, &overlapped, recv_complete);
		
		//if (receiveBytes > 0) { // recv하자마자 출력하지 않고 기다려야함
		//	messageBuffer[receiveBytes] = 0;
		//	cout << "TRACE - Receive message : " << messageBuffer << "(" << receiveBytes << " bytes)\n";
		//}
		//else break;
		//int sendBytes = send(client_socket, messageBuffer, receiveBytes, 0);

	}
	closesocket(listenSocket);
	WSACleanup();
}
