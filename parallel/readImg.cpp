#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <time.h>
#include <pthread.h>

#define MAX 255
#define NUM_OF_THREADS 4

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

using namespace std;

const string output_address = "output.bmp";

#pragma pack(1)
// #pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

struct T_data{
  int num;
  vector<vector<vector<int>>> *in;
  vector<vector<vector<int>>> *out;
};

struct T_data_w{
  int num;
  vector<vector<vector<int>>> *in;
  vector<vector<vector<int>>> *out;
  int R_mean;
  int G_mean;
  int B_mean;
};

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer, vector<vector<vector<int>>> &image)
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          image[i][j][0] = (unsigned char)fileReadBuffer[end - count];
          break;
        case 1:
          image[i][j][1] = (unsigned char)fileReadBuffer[end - count];
          break;
        case 2:
          image[i][j][2] = (unsigned char)fileReadBuffer[end - count];
          break;
        }
        // go to the next position in the buffer
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, vector<vector<vector<int>>> &image)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][0];
          break;
        case 1:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][1];
          break;
        case 2:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][2];
          break;
        }
        // go to the next position in the buffer
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

void *smooth_thread(void *in){
  pthread_detach(pthread_self());
  //cout << "in thread" << endl;
  struct T_data *data = (struct T_data *)in;
  int start = (rows/NUM_OF_THREADS) * data->num;
  int end = start + (rows/NUM_OF_THREADS);
  for(int i = start; i < end; i++){
    for(int j = 0; j < cols; j++){
      for(int k = 0; k < 3; k++){
        if(i == 0){
          if(j == 0){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i+1][j+1][k]) / 4;
          }
          else if(j == cols - 1){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i][j-1][k] + (*data->in)[i+1][j-1][k]) / 4;
          }
          else{
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i][j-1][k] + (*data->in)[i+1][j+1][k] + (*data->in)[i+1][j-1][k]) / 6;
          }
        }
        else if(i == rows - 1){
          if(j == 0){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i-1][j+1][k]) / 4;
          }
          else if(j == cols - 1){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j-1][k] + (*data->in)[i-1][j-1][k]) / 4;
          }
          else{
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i][j-1][k] + (*data->in)[i-1][j+1][k] + (*data->in)[i-1][j-1][k]) / 6;
          }
        }
        else{
          if(j == 0){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i+1][j+1][k] + (*data->in)[i-1][j+1][k]) / 6;
          }
          else if(j == cols - 1){
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j-1][k] + (*data->in)[i+1][j-1][k] + (*data->in)[i-1][j-1][k]) / 6;
          }
          else{
            (*data->out)[i][j][k] = ((*data->in)[i][j][k] + (*data->in)[i+1][j][k] + (*data->in)[i-1][j][k] + (*data->in)[i][j+1][k] + (*data->in)[i][j-1][k] + (*data->in)[i+1][j+1][k] + (*data->in)[i+1][j-1][k] + (*data->in)[i-1][j+1][k] + (*data->in)[i-1][j-1][k]) / 9;
          }
        }
      }
    }
  }
  pthread_exit(NULL);
}

void smoothing(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &smoothed){
  vector<pthread_t> ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_t ptid;
    struct T_data *data = new struct T_data;
    data->in = &image;
    data->out = &smoothed;
    data->num = i;
    pthread_create(&ptid, &attr, &smooth_thread, data);
    ids.push_back(ptid);
  }
  void *state;
  pthread_attr_destroy(&attr);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    cout << "here" << i << endl;
    pthread_join(ids[i], &state);
  }
  cout << "pass" << endl;
}

void *sepia_thread(void *in){
  pthread_detach(pthread_self());
  struct T_data *data = (struct T_data *)in;
  int start = (rows/NUM_OF_THREADS) * data->num;
  int end = start + (rows/NUM_OF_THREADS);
  for(int i = start; i < end; i++){
    for(int j = 0; j < cols; j++){
      (*data->out)[i][j][0] = (*data->in)[i][j][0] * 0.393 + (*data->in)[i][j][1] * 0.769 + (*data->in)[i][j][2] * 0.189;
      (*data->out)[i][j][1] = (*data->in)[i][j][0] * 0.349 + (*data->in)[i][j][1] * 0.686 + (*data->in)[i][j][2] * 0.168;
      (*data->out)[i][j][2] = (*data->in)[i][j][0] * 0.272 + (*data->in)[i][j][1] * 0.534 + (*data->in)[i][j][2] * 0.131;
      for(int k = 0; k < 3; k++){
        if((*data->out)[i][j][k] > MAX){
          (*data->out)[i][j][k] = MAX;
        }
      }
    }
  }
  pthread_exit(NULL);
}

void sepia(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &sepia_out){
  vector<pthread_t> ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_t ptid;
    struct T_data *data = new struct T_data;
    data->in = &image;
    data->out = &sepia_out;
    data->num = i;
    pthread_create(&ptid, &attr, &sepia_thread, data);
    ids.push_back(ptid);
  }
  pthread_attr_destroy(&attr);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_join(ids[i], NULL);
  }
}

void *wash_thread(void *in){
  pthread_detach(pthread_self());
  struct T_data_w *data = (struct T_data_w *)in;
  int start = (rows/NUM_OF_THREADS) * data->num;
  int end = start + (rows/NUM_OF_THREADS);
  for(int i = start; i < end; i++){
    for(int j = 0; j < cols; j++){
      (*data->out)[i][j][0] = (*data->in)[i][j][0] * 0.4 + data->R_mean * 0.6;
      (*data->out)[i][j][1] = (*data->in)[i][j][1] * 0.4 + data->G_mean * 0.6;
      (*data->out)[i][j][2] = (*data->in)[i][j][2] * 0.4 + data->B_mean * 0.6;
    }
  }
  pthread_exit(NULL);
}

void washed_out(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &washed){
  int R_mean = 0, G_mean = 0, B_mean = 0;
  int R_sum = 0, G_sum = 0, B_sum = 0;
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      R_sum += image[i][j][0];
      G_sum += image[i][j][1];
      B_sum += image[i][j][2];
    }
  }
  int num = rows * cols;
  R_mean = R_sum / num;
  G_mean = G_sum / num;
  B_mean = B_sum / num;
  vector<pthread_t> ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_t ptid;
    struct T_data_w *data = new struct T_data_w;
    data->in = &image;
    data->out = &washed;
    data->num = i;
    data->R_mean = R_mean;
    data->G_mean = G_mean;
    data->B_mean = B_mean;
    pthread_create(&ptid, &attr, &wash_thread, data);
    ids.push_back(ptid);
  }
  pthread_attr_destroy(&attr);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_join(ids[i], NULL);
  }
}

void *add_line_thread(void *in){
  pthread_detach(pthread_self());
  struct T_data *data = (struct T_data *)in;
  int start = (rows/NUM_OF_THREADS) * data->num;
  int end = start + (rows/NUM_OF_THREADS);
  for(int i = start; i < end; i++){
    for(int j = 0; j < cols; j++){
      if(i == j || (i-1) == j || (i+1) == j || i == (cols-j) || (i-1) == (cols-j) || (i+1) == (cols-j)){
        (*data->out)[i][j][0] = (*data->out)[i][j][1] = (*data->out)[i][j][2] = MAX;
      }
      else{
        (*data->out)[i][j] = (*data->in)[i][j];
      }
    }
  }
  pthread_exit(NULL);
}

void add_lines(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &out){
  vector<pthread_t> ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_t ptid;
    struct T_data *data = new struct T_data;
    data->in = &image;
    data->out = &out;
    data->num = i;
    pthread_create(&ptid, &attr, &add_line_thread, data);
    ids.push_back(ptid);
  }
  pthread_attr_destroy(&attr);
  for(int i = 0; i < NUM_OF_THREADS; i++){
    pthread_join(ids[i], NULL);
  }
}

int main(int argc, char *argv[])
{
  clock_t start = clock();
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  vector<int> a(3, 0);
  vector<vector<int>> temp(rows, a);
  vector<vector<vector<int>>> image(cols, temp);
  vector<vector<vector<int>>> smoothed(cols, temp);
  vector<vector<vector<int>>> sepia_out(cols, temp);
  vector<vector<vector<int>>> washed(cols, temp);
  vector<vector<vector<int>>> line_added(cols, temp);

  // read input file
  // clock_t t1 = clock();
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer, image);
  clock_t t2 = clock();
  
  // apply filters
  smoothing(image, smoothed);
  clock_t t3 = clock();
  sepia(smoothed, sepia_out);
  clock_t t4 = clock();
  washed_out(sepia_out, washed);
  clock_t t5 = clock();
  add_lines(washed, line_added);
  clock_t t6 = clock();

  // write output file
  int n = output_address.length() + 1;
  char out_file[n];
  strcpy(out_file, output_address.c_str());
  // clock_t t7 = clock();
  writeOutBmp24(fileBuffer, out_file, bufferSize, line_added);
  clock_t final = clock();

  cout << (double)(final - start) / CLOCKS_PER_SEC * 1000 << endl;

  // for finding hotspots:
  // cout << "Time of Reading from file (ms): " << (double)(t2 - t1) / CLOCKS_PER_SEC * 1000 << endl;
  cout << "Time of Smoothing Filter (ms): " << (double)(t3 - t2) / CLOCKS_PER_SEC * 1000 << endl;
  cout << "Time of Sepia Filter (ms): " << (double)(t4 - t3) / CLOCKS_PER_SEC * 1000 << endl;
  cout << "Time of Washed Out Filter (ms): " << (double)(t5 - t4) / CLOCKS_PER_SEC * 1000 << endl;
  cout << "Time of Add Line Filter (ms): " << (double)(t6 - t5) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Time of Writing in file (ms): " << (double)(final - t7) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Serial Execution Time (ms): " << (double)(final - start) / CLOCKS_PER_SEC * 1000 << endl;
  return 0;
}