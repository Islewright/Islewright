#ifndef DEFINE_H
#define DEFINE_H

#pragma comment(lib, "ws2_32.lib")


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include<iostream>
#include <format>

#define MAX_BUFFER_SIZE 1024

enum class IOOperation{
    RECV,
    SEND,
};


typedef struct OverlappedEx{
    WSAOVERLAPPED overlapped;
    UINT64 clientIndex;
    IOOperation operation;
    WSABUF wsabuf;
}OverlappedEx,* lpOverlappedEx;

#endif