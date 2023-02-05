#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdio>
#include <windows.h>
#include <stdio.h>

using namespace std;

const int process_number = 6; // ���������� ������� 4 + ((514 + 23) % 5) = 6
int lowerCounter = 0; // ���������� �������� ������ 128
int higherCounter = 0; // ���������� �������� ������ 128
const int defSize = 4096; // ������ ������ ��� ���������
HANDLE pr[process_number];
MSG Msg;


void searchRed(int* pixelArray, int arrayStart, int arrayEnd, int threshold, int& lowerCount, int& higherCount)
{
    int higher = 0;
    int lower = 0;

    for (int j = arrayStart; j < arrayEnd; j++)
    {
        if (pixelArray[j] > threshold)
        {
            higher++;
        }
        else
        {
            lower++;
        }
    }
    higherCount += higher;
    lowerCount += lower;
}


typedef struct
{
    uint8_t r, g, b, a;
} rgb32;


#pragma pack(2)
typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} bitmap_file_header;
#pragma pack()


#pragma pack(2)
typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int16_t biXPelsPerMeter;
    int16_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} bitmap_info_header;
#pragma pack()


#pragma pack(2)
typedef struct
{
    bitmap_file_header fHeader;
    bitmap_info_header iHeader;
} bitmap_header;
#pragma pack()


class bitmap
{
private:
    bitmap_header header;
    uint8_t* pixels;

public:
    bitmap(const char* path);
    rgb32* getPixel(uint32_t x, uint32_t y) const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
};


bitmap::bitmap(const char* path) : header(), pixels(nullptr)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (file)
    {
        file.read(reinterpret_cast<char*>(&header.fHeader), sizeof(header.fHeader));
        file.read(reinterpret_cast<char*>(&header.iHeader), sizeof(header.iHeader));
        file.seekg(header.fHeader.bfOffBits, std::ios::beg);
        pixels = new uint8_t[header.fHeader.bfSize - header.fHeader.bfOffBits];
        file.read(reinterpret_cast<char*>(&pixels[0]), header.fHeader.bfSize - header.fHeader.bfOffBits);
        uint8_t* temp = new uint8_t[header.iHeader.biWidth * header.iHeader.biHeight * sizeof(rgb32)];
        uint8_t* in = pixels;
        rgb32* out = reinterpret_cast<rgb32*>(temp);
        int padding = (header.iHeader.biSizeImage - header.iHeader.biWidth * header.iHeader.biHeight * 3) / header.iHeader.biHeight;

        for (int i = 0; i < header.iHeader.biHeight; ++i, in += padding)
        {
            for (int j = 0; j < header.iHeader.biWidth; ++j)
            {
                out->b = *(in++);
                out->g = *(in++);
                out->r = *(in++);
                out->a = 0xFF;
                ++out;
            }
        }
        delete[] pixels;
        pixels = temp;
    }
}


rgb32* bitmap::getPixel(uint32_t x, uint32_t y) const
{
    rgb32* temp = reinterpret_cast<rgb32*>(pixels);
    return &temp[(header.iHeader.biHeight - 1 - y) * header.iHeader.biWidth + x];
}


uint32_t bitmap::getWidth() const
{
    return header.iHeader.biWidth;
}

uint32_t bitmap::getHeight() const
{
    return header.iHeader.biHeight;
}


int main(int argc, char* argv[])
{
    const int threshold = 128;
    bitmap bmp{ "C:/Users/1642645/source/repos/bmp_tasks/TRU256.bmp" };
    int bmp_sise = bmp.getWidth() * bmp.getHeight();
    int k = 0;
    int* pixelArray = new int[bmp_sise];

    for (int i = 0; i < int(bmp.getWidth()); i++)
    {
        for (int j = 0; j < int(bmp.getHeight()); j++)
        {
            pixelArray[k] = int(bmp.getPixel(i, j)->r);
            k++;
        }
    }


    if (argc == 1)
    {
        for (int proc = 0; proc < process_number; proc++) 
        {
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.wShowWindow = SW_HIDE;
            ZeroMemory(&pi, sizeof(pi));
            DWORD threadId = GetCurrentThreadId();

            // �������� ������ � ����� ������� ��� �������� ��������
            int subArrayStart = 2 * ((bmp_sise * proc) / (process_number * 2));
            int subArrayEnd = 2 * ((bmp_sise * (proc + 1)) / (process_number * 2));

            // ������ ��� �������� � ������� ��� ������� ��������
            char cmdLine[4096];
            sprintf(cmdLine, "\"%s\" %d %d %d %d", argv[0], subArrayStart, subArrayEnd, proc, threadId);

            if (!CreateProcessA(NULL, cmdLine, NULL, NULL, false, 0, NULL, NULL, &si, &pi)) printf("Error CreateProcessA");
            pr[proc] = pi.hProcess;
        }

        int receivedMsgCount = 0;
        while (receivedMsgCount < process_number) 
        {

            while (!PeekMessage(&Msg, NULL, 0, WM_USER, PM_REMOVE)) 
            {
            }

            if (Msg.message == WM_COMMAND) 
            {
                higherCounter += Msg.wParam;
                lowerCounter += Msg.lParam;;
            }
            else
                std::cout << "Message: " << Msg.message << '\n';

            receivedMsgCount++;
        }

        printf("Width: %d\n", bmp.getWidth());
        printf("Height: %d\n", bmp.getHeight());
        printf("Number of pixels: %d\n", bmp_sise);
        printf("Number of pixels higher than %d: %d\n", threshold, higherCounter);
        printf("Number of pixels lower (and equal) than %d: %d\n", threshold, lowerCounter);
    }
    else
    {
        int arrStart = atoi(argv[1]);
        int arrEnd = atoi(argv[2]);
        int processNumber = atoi(argv[3]);
        int mainThreadId = atoi(argv[4]);
        int locallowerCounter = 0, localhigherCounter = 0;

        // ��������� �����������
        searchRed(pixelArray, arrStart, arrEnd, threshold, locallowerCounter, localhigherCounter);

        if (!PostThreadMessage(mainThreadId, WM_COMMAND, (WPARAM)localhigherCounter, (LPARAM)locallowerCounter))
            std::cout << GetLastError() << " PostThreadMessage error\n";

        CloseHandle(pr[processNumber]);
    }
    return 0;
}


