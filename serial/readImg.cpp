#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>

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
          image[i][j][0] = 128 + int(fileReadBuffer[end - count]);
          break;
        case 1:
          image[i][j][1] = 128 + int(fileReadBuffer[end - count]);
          break;
        case 2:
          image[i][j][2] = 128 + int(fileReadBuffer[end - count]);
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
          fileBuffer[bufferSize - count] = image[i][j][0] - 128;
          break;
        case 1:
          fileBuffer[bufferSize - count] = image[i][j][1] - 128;
          break;
        case 2:
          fileBuffer[bufferSize - count] = image[i][j][2] - 128;
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
            //smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k]) /2;  //+ image[i][j][k] + image[i][j][k] + image[i][j][k] + image[i][j][k] + image[i][j][k] + image[i][j][k] + image[i][j][k]) / 9;
            //cout << "s : " << smoothed[i][j][k] << " , i : " << image[i][j][k] << " , i+1 : " << image[i+1][j][k] << endl;
            smoothed[i][j][k] = (image[i][j][k] + image[i+1][j][k] + image[i-1][j][k] + image[i][j+1][k] + image[i][j-1][k] + image[i+1][j+1][k] + image[i+1][j-1][k] + image[i-1][j+1][k] + image[i-1][j-1][k]) / 9;
          }
        }
      }
    }
  }
}

int main(int argc, char *argv[])
{
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

  // read input file
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer, image);

  ///////////////
  // for(int i = 0; i < rows; i++){
  //   for(int j = 0; j < cols; j++){
  //     cout << "vec " << i << " , "<< j << " : " << image[i][j][0]
  //     << " , " << image[i][j][1] << " , " << image[i][j][2] << endl; 
  //   }
  // }
  // int r, g, b;
  // for(int i = 0; i < rows; i++){
  //   for(int j = 0; j < cols; j++){
  //     if(image[i][j][0] != r){
  //       r = image[i][j][0];
  //       cout << "vec " << i << " , "<< j << " : " << image[i][j][0]
  //       << " , " << image[i][j][1] << " , " << image[i][j][2] << endl;
  //     } 
  //     if(image[i][j][1] != g){
  //       g = image[i][j][1];
  //       cout << "vec " << i << " , "<< j << " : " << image[i][j][0]
  //       << " , " << image[i][j][1] << " , " << image[i][j][2] << endl;
  //     }
  //     if(image[i][j][2] != b){
  //       b = image[i][j][2];
  //       cout << "vec " << i << " , "<< j << " : " << image[i][j][0]
  //       << " , " << image[i][j][1] << " , " << image[i][j][2] << endl;
  //     }
  //   }
  // }


  /////////////////



  // apply filters
  smoothing(image, smoothed);




  // write output file
  int n = output_address.length() + 1;
  char out_file[n];
  strcpy(out_file, output_address.c_str());
  writeOutBmp24(fileBuffer, out_file, bufferSize, smoothed);

  return 0;
}