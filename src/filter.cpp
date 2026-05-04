#include "config.h"
#include "filter.h"

int filter(int data[]) { //median filter
  int dataClean[DATATIMES];
  int k = 0;
  //pulizia dei dati eliminando quelli non validi (ND)
  for(int i = 0; i < DATATIMES; i++){
    if(data[i] != ND){ //se l'elemento è valido, aggiungilo all'array dei dati puliti
     dataClean[k] = data[i];
     k++;
    }
  }

  if (k == 0) return ND; // Se non ci sono dati validi, restituisci ND

  //ordinamento dei dati puliti con insertion sort
  int size = k;
  for (int i = 1; i < size; ++i) {
    int key = dataClean[i];
    int j = i - 1;
    while (j >= 0 && dataClean[j] > key) {
      dataClean[j + 1] = dataClean[j];
      j = j - 1;
    }
    dataClean[j + 1] = key;
  }

  return dataClean[size / 2]; //restituzione del valore mediano
}