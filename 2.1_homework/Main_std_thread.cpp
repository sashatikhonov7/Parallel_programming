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

void Wav_reading(short int* Data, int sample_size, int samples_count, int NumberOfThreads, FILE* File, int th_id)
{
    for (int i = (((th_id + 1) * samples_count) / NumberOfThreads) - 1; i >= (th_id * samples_count) / NumberOfThreads; i--)
    {
        m.lock();
        fread(&Data[i], sample_size, 1, File);
        m.unlock();
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

    int NumberOfThreads = 7;

    File = fopen("C:/Users/1642645/source/repos/2_homework/2_homework/sample.wav", "rb");

    //Reading the file header
    fread(&Header, sizeof(WAV_HEADER), 1, File);

    //Counting the number and dimension of samples
    int sample_size = Header.bitsPerSample / 8;
    int samples_count = Header.subchunk2Size * 8 / Header.bitsPerSample;

    //Creating an array for writing a file
    int p_count = 0, n_count = 0;
    short int* Data = new short int[samples_count];

    //Define method from task: (514+24) % 4 = 2 -> method C
    std::thread th1(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 0);
    std::thread th2(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 1);
    std::thread th3(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 2);
    std::thread th4(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 3);
    std::thread th5(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 4);
    std::thread th6(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 5);
    std::thread th7(Wav_reading, Data, sample_size, samples_count, NumberOfThreads, File, 6);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();

    //Output of the first 50 values to compare correctness with Cool Edit
    for (int i = 0; i < 50; i++)
    {
        cout << i << " element: " << Data[i] << "\n";
    }

    fclose(File);
    (void)getchar();
    return 0;
}