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
    for (int j = cols - 1; j >= 0; j--){
      for (int k = 0; k < 3; k++)
      {
        ////////////////
        // if(end-count < 0){
        //   cout << "file " << end-count << " , " << i << " , "<< j << endl;
        // }
        /////////////////////////
        switch (k)
        {
        case 0:
          image[i][j][0] = 128 + int(fileReadBuffer[end - count]);
          //////////
          // cout << "file " << end-count << " , " << i << " , "<< j << " : " << 128 + int(fileReadBuffer[end - count]);
          // cout << " but a : " << a[0] << endl;
          /////////////
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
  /////////////////

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
  //return image;
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
  //vector<vector<int*>> image(cols, temp);
  vector<vector<vector<int>>> image(cols, temp);
  // for(int i = 0; i < rows; i++){
  //   vector<int*> temp;
  //   for(int j = 0; j < cols; j++){
  //     int a[3]= {};
  //     temp.push_back(a); 
  //   }
  //   image.push_back(temp);
  // }

  ///////////////////////
  // cout << rows << " , " << cols << endl;
  // cout << bufferSize << endl;
  ////////////////////

  // read input file
  //int end = rows*cols*3;
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





  // write output file
  int n = output_address.length() + 1;
  char out_file[n];
  strcpy(out_file, output_address.c_str());
  writeOutBmp24(fileBuffer, out_file, bufferSize, image);




  return 0;
}