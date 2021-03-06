#include "stdafx.h"
#include "Communicates.h"

#define BUFSIZE 4096

void textConvert(const char* msg, wchar_t* ret)
{
    mbstowcs(ret, msg, strlen(msg) + 1);//Plus null
}

void error_quit(const char* msg)
{
    wchar_t wMsg[20];
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    textConvert(msg, wMsg);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, wMsg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void error_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

bool SendFrameData(SOCKET& sock, string& str, int& retval)
{
    int len = str.length();

    // 데이터 보내기(고정 길이)
    retval = send(sock, (char*)&len, sizeof(int), 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }

    // 데이터 보내기(가변 길이)
    retval = send(sock, str.c_str(), len, 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    return true;
}

bool RecvFrameData(SOCKET& sock, char* buf, int& retval)
{
    // 데이터 받기(고정 길이)
    int len;
    retval = recvn(sock, (char*)&len, sizeof(int), 0);

    if (retval == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }
    else if (retval == 0) return false;

    // 데이터 받기(가변 길이)
    retval = recvn(sock, buf, len, 0);

    if (retval == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }

    buf[retval] = '\0';
    return true;
}

Vector2f GetPositionFromText(const char* text)
{
    if (strstr(text, "<Position>:"))       //(token, "<position>:"))
    {
        Vector2f position;
        int count = 0;
        for (int i = 11; i < strlen(text); ++i, ++count)
        {
            if (text[i] == ' ')
            {
                char temp[20] = {};
                strncpy(temp, text + 11, count);
                position.x = atof(temp);
                strncpy(temp, text + i, strlen(text) - i);
                position.y = atof(temp);

                return position;
            }
        }
    }
    return { -1,-1 };
}

void GetCoordsFromText(const char* token, int num, vector<Vector2i>& coords)
{
    for (int i = 0; i < num; ++i)
    {
        token = strtok(NULL, "\n");
        Vector2i coord;
        int count = 0;
        for (int i = 0; i < strlen(token); ++i, ++count)
        {
            if (token[i] == ' ')
            {
                char temp[20] = {};
                strncpy(temp, token, count);
                coord.x = atof(temp);
                strncpy(temp, token + i, strlen(token) - i);
                coord.y = atof(temp);
               
                coords.push_back(coord);
                break;
            }
        } 
    }
}

void GetCoordsFromText(const char* token, int num, vector<int>& names, vector<Vector2i>& coords)
{
    for (int i = 0; i < num; ++i)
    {
        token = strtok(NULL, "\n");
        Vector2i coord;
        int name;
        int count = 0; 
        for (int j = 0; j < strlen(token); ++j, ++count)
        {
            char temp[20] = {};

            if (token[j] == ' ')
            {
                strncpy(temp, token, count); 
                name = atof(temp);  
                for (int k = j + 1, count_ = 0; k < strlen(token); ++k, ++count_)
                {
                    if (token[k] == ' ')
                    {
                        strncpy(temp, token + j + 1, 2);
                        coord.x = atof(temp); 
                     
                        strncpy(temp, token + j + k, 2);
                        coord.y = atof(temp);
                    }
                } 
                coords.push_back(coord);
                names.push_back(name);
            } 
        }
    }
}

void GetPlayerInfoFromText(const char* token, int num, vector<int>& indices, vector<int>& states, vector<Vector2f>& positions)
{
    for (int i = 0; i < num; ++i)
    {
        token = strtok(NULL, "\n");
        Vector2f position;
        int state;
        int index;
        int count = 0;
        for (int j = 0; j < strlen(token); ++j, ++count)
        {
            char temp[20] = {};

            if (token[j] == ' ')
            {
                strncpy(temp, token, count);
                index = atoi(temp);

                strncpy(temp, token + j, strlen(token) - j);
                state = atoi(temp);

                token = strtok(NULL, "\n");
                count = 0;
                for (int k = 0; k < strlen(token); ++k, ++count)
                {
                    if (token[k] != ' ') continue;

                    strncpy(temp, token, count);
                    position.x = atof(temp);

                    strncpy(temp, token + k, strlen(token) - k);
                    position.y = atof(temp);
                } 
                
                indices.push_back(index);
                positions.push_back(position);
                states.push_back(state);
                break;
            }
        }
    }
}

int ConvertoIntFromText(const char* text, const char* token)
{
    char buf[256];
    ZeroMemory(buf, 256);
    int tokenLen = strlen(token);
    strncpy(buf, text + tokenLen, strlen(text) - tokenLen);
    return atoi(buf);
}
