#include <iostream>
#include <string>
#include <fstream>
#pragma warning(disable : 4996)
#include <thread>
#include <mutex>

using namespace std;
using std::string;
using std::fstream;

std::mutex m;

void Counting(short int* Data, int samples_count, int g_nNumberOfThreads, int th_id, int *l_count, int *m_count)
{
    for (int i = (((th_id + 1) * samples_count) / g_nNumberOfThreads) - 1; i >= (th_id * samples_count) / g_nNumberOfThreads; i--)
    {
        if (int(Data[i]) < 16000)
        {
            m.lock();
            (* l_count)++;
            m.unlock();
        }
        else if (int(Data[i]) > 16000)
        {
            m.lock();
            (* m_count)++;
            m.unlock();
        }
    }
};

//Creating a structure for the wav file header
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

    int g_nNumberOfThreads = 7;
    int l_count = 0, m_count = 0;

    File = fopen("C:/Users/1642645/source/repos/2_homework/2_homework/sample.wav", "rb");

    //Reading the file header
    fread(&Header, sizeof(WAV_HEADER), 1, File);

    //Counting the number and dimension of samples
    int sample_size = Header.bitsPerSample / 8;
    int samples_count = Header.subchunk2Size * 8 / Header.bitsPerSample;

    //Creating an array for writing a file
    short int* Data = new short int[samples_count];

    //Reading a block of file data
    for (int i = 0; i < samples_count; i++)
    {
        fread(&Data[i], sample_size, 1, File);
    }

    //Define method from task: (514+24) % 4 = 2 -> method C
    std::thread th1(Counting, Data, samples_count, g_nNumberOfThreads, 0, &l_count, &m_count);
    std::thread th2(Counting, Data, samples_count, g_nNumberOfThreads, 1, &l_count, &m_count);
    std::thread th3(Counting, Data, samples_count, g_nNumberOfThreads, 2, &l_count, &m_count);
    std::thread th4(Counting, Data, samples_count, g_nNumberOfThreads, 3, &l_count, &m_count);
    std::thread th5(Counting, Data, samples_count, g_nNumberOfThreads, 4, &l_count, &m_count);
    std::thread th6(Counting, Data, samples_count, g_nNumberOfThreads, 5, &l_count, &m_count);
    std::thread th7(Counting, Data, samples_count, g_nNumberOfThreads, 6, &l_count, &m_count);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();

    cout << "Number of elements < 16000: " << l_count << "\n";
    cout << "Number of elements > 16000: " << m_count << "\n";

    //Output of the first 50 values to compare correctness with Cool Edit
    for (int i = 0; i < 50; i++)
    {
        cout << i << " element: " << Data[i] << "\n";
    }

    fclose(File);
    (void)getchar();
    return 0;
}