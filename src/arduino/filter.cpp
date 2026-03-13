#include "config.h"
#include "filter.h"

int filter(int data[]) { //median filter
  int size = DATATIMES;
  for (int i = 1; i < size; ++i) {
    int key = data[i];
    int j = i - 1;
    while (j >= 0 && data[j] > key) {
      data[j + 1] = data[j];
      j = j - 1;
    }
    data[j + 1] = key;
  }
  return data[size / 2];
}