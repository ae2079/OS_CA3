#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <time.h>

#define MAX 255

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

void smoothing(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &smoothed){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      for(int k = 0; k < 3; k++){
        if(i == 0){
          if(j == 0){
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i][j+1][k] + image[i+1][j+1][k]) / 4;
          }
          else if(j == cols - 1){
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i][j-1][k] + image[i+1][j-1][k]) / 4;
          }
          else{
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i][j+1][k] + image[i][j-1][k] + image[i+1][j+1][k] + image[i+1][j-1][k]) / 6;
          }
        }
        else if(i == rows - 1){
          if(j == 0){
            smoothed[i][j][k] = (image[i][j][k] + image[i-1][j][k] + image[i][j+1][k] + image[i-1][j+1][k]) / 4;
          }
          else if(j == cols - 1){
            smoothed[i][j][k] = (image[i][j][k] + image[i-1][j][k] + image[i][j-1][k] + image[i-1][j-1][k]) / 4;
          }
          else{
            smoothed[i][j][k] = (image[i][j][k] + image[i-1][j][k] + image[i][j+1][k] + image[i][j-1][k] + image[i-1][j+1][k] + image[i-1][j-1][k]) / 6;
          }
        }
        else{
          if(j == 0){
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i-1][j][k] + image[i][j+1][k] + image[i+1][j+1][k] + image[i-1][j+1][k]) / 6;
          }
          else if(j == cols - 1){
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i-1][j][k] + image[i][j-1][k] + image[i+1][j-1][k] + image[i-1][j-1][k]) / 6;
          }
          else{
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i-1][j][k] + image[i][j+1][k] + image[i][j-1][k] + image[i+1][j+1][k] + image[i+1][j-1][k] + image[i-1][j+1][k] + image[i-1][j-1][k]) / 9;
          }
        }
      }
    }
  }
}

void sepia(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &sepia_out){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      sepia_out[i][j][0] = image[i][j][0] * 0.393 + image[i][j][1] * 0.769 + image[i][j][2] * 0.189;
      sepia_out[i][j][1] = image[i][j][0] * 0.349 + image[i][j][1] * 0.686 + image[i][j][2] * 0.168;
      sepia_out[i][j][2] = image[i][j][0] * 0.272 + image[i][j][1] * 0.534 + image[i][j][2] * 0.131;
      for(int k = 0; k < 3; k++){
        if(sepia_out[i][j][k] > 255){
          sepia_out[i][j][k] = 255;
        }
      }
    }
  }
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
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      washed[i][j][0] = image[i][j][0] * 0.4 + R_mean * 0.6;
      washed[i][j][1] = image[i][j][1] * 0.4 + G_mean * 0.6;
      washed[i][j][2] = image[i][j][2] * 0.4 + B_mean * 0.6;
    }
  }
}

void add_lines(vector<vector<vector<int>>> &image, vector<vector<vector<int>>> &out){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      if(i == j || (i-1) == j || (i+1) == j || i == (cols-j) || (i-1) == (cols-j) || (i+1) == (cols-j)){
        out[i][j][0] = out[i][j][1] = out[i][j][2] = MAX;
      }
      else{
        out[i][j] = image[i][j];
      }
    }
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
  // clock_t t2 = clock();
  
  // apply filters
  smoothing(image, smoothed);
  // clock_t t3 = clock();
  sepia(smoothed, sepia_out);
  // clock_t t4 = clock();
  washed_out(sepia_out, washed);
  // clock_t t5 = clock();
  add_lines(washed, line_added);
  // clock_t t6 = clock();

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
  // cout << "Time of Smoothing Filter (ms): " << (double)(t3 - t2) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Time of Sepia Filter (ms): " << (double)(t4 - t3) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Time of Washed Out Filter (ms): " << (double)(t5 - t4) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Time of Add Line Filter (ms): " << (double)(t6 - t5) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Time of Writing in file (ms): " << (double)(final - t7) / CLOCKS_PER_SEC * 1000 << endl;
  // cout << "Serial Execution Time (ms): " << (double)(final - start) / CLOCKS_PER_SEC * 1000 << endl;
  return 0;
}