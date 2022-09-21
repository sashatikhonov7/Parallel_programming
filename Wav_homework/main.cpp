#include <iostream>
#include <string>
#include <fstream>
#pragma warning(disable : 4996)

using namespace std;
using std::string;
using std::fstream;

//�������� ��������� ��� ��������� wav-�����
struct  WAV_HEADER
{
    char chunkId[4];
    unsigned long chunkSize;
    char format[4];
    char subchunk1Id[4];
    unsigned long subchunk1Size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char subchunk2Id[4];
    unsigned long subchunk2Size;
};

int main()
{
    setlocale(LC_ALL, "Russian");

    WAV_HEADER Header;
    FILE* File;

    //�������� �����
    File = fopen("sample.wav", "rb");

    //������ ��������� �����
    fread(&Header, sizeof(WAV_HEADER), 1, File);

    //������� ���������� � ����������� �������
    int sample_size = Header.bitsPerSample / 8;
    int samples_count = Header.subchunk2Size * 8 / Header.bitsPerSample;

    //�������� ������� ��� ������ �����
    int p_count = 0, n_count = 0;
    short int* Data = new short int[samples_count];

    //������ ����� ������ ����� � ������� ������������� � ������������� ��������
    for (int i = 0; i < samples_count; i++)
    {
        fread(&Data[i], sample_size, 1, File);
        if (Data[i] > 0)
        {
            p_count++;
        }
        else if (Data[i] < 0)
        {
            n_count++;
        }
    }

    //����� ������ 50 �������� ��� ��������� ������������ � Cool Edit
    for (int i = 0; i < 50; i++)
    {
        cout << i << " �������: " << Data[i] << "\n";
    }

    cout << "Number of positive elements: " << p_count << "\n";
    cout << "Number of negative elements: " << n_count << "\n";

    fclose(File);
    (void)getchar();
    return 0;
}