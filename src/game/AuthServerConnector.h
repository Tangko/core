#ifndef AUTH_SERVER_CONNECTOR_H
#define AUTH_SERVER_CONNECTOR_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bcrypt.h>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")

namespace AuthServerConnector
{
	const std::string HMAC_SECRET = "24a0dad70f71d73003a9dbcd866ef03c2e72ecdf70f49049144993573cbf5278";

	inline std::string HmacSha256(const std::string& key, const std::string& data)
	{
		BCRYPT_ALG_HANDLE hAlg = nullptr;
		BCRYPT_HASH_HANDLE hHash = nullptr;
		BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
		BCryptCreateHash(hAlg, &hHash, nullptr, 0, (PUCHAR)key.c_str(), (ULONG)key.size(), 0);
		BCryptHashData(hHash, (PUCHAR)data.c_str(), (ULONG)data.size(), 0);
		BYTE hash[32] = {};
		BCryptFinishHash(hHash, hash, 32, 0);
		BCryptDestroyHash(hHash);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return std::string((char*)hash, 32);
	}

	inline std::string ToHex(const std::string& data)
	{
		const char* hex = "0123456789abcdef";
		std::string result;
		for (unsigned char c : data) {
			result += hex[c >> 4];
			result += hex[c & 0xF];
		}
		return result;
	}

	inline std::string SendCommand(const std::string& command)
	{
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(12345);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		std::string result;
		if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0)
		{
			send(sock, command.c_str(), command.size(), 0);
			char buf[256] = {};
			int timeout = 3000;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
			int len = recv(sock, buf, sizeof(buf) - 1, 0);
			if (len > 0) result.assign(buf, len);
		}
		closesocket(sock);
		WSACleanup();
		return result;
	}

	inline std::string SendSignedCommand(const std::string& action, const std::string& username, const std::string& param)
	{
		std::string ts = std::to_string(time(nullptr));
		std::string msg = action + " " + username + " " + param + " " + ts;
		std::string sig = ToHex(HmacSha256(HMAC_SECRET, msg));
		std::string command = action + " " + username + " " + param + " " + ts + " " + sig;
		return SendCommand(command);
	}
}

#endif