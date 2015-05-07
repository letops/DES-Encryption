/*  Author: Daniel Alejandro Ortíz Pérez - A01204096
            Denisse Lizbeth Maldonado Flores - A01062721
    Program: Final Proyect - DES  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "DES_Tables.h"

#define MAXSTR 256
#define KVAL 50

typedef unsigned long int uli;
typedef unsigned int uint;

//Función simple para imprimir todas las localidades de un unsigned int array
void printIntArray(unsigned int * array, int size){
  int i=0;
  printf("\nArray:\n");
  for (i=0; i<size; i++){
    printf("%d ", array[i]);
  }
  printf("\n");
}

//Función simple para imprimir todas las localidades de un unsigned long int array
void printLongArray(unsigned long int * array, int size){
  int i=0;
  printf("\nLArray:\n");
  for (i=0; i<size; i++){
    printf("%ld , ", array[i]);
  }
  printf("\n");
}

unsigned long int arrayToLong(unsigned int * array, int arraySize, int cellSize){
  int i;
  unsigned long result = 0;
  for(i=0; i<arraySize; i++){

    unsigned long int temp = array[i];
    temp = temp << (cellSize*i);
    result = result | temp;
  }
  return result;
}

unsigned char * longsToArray(unsigned long int value1, unsigned long int value2, int valueBits, int arraySize){
  unsigned char *result = malloc(arraySize * sizeof(unsigned int));
  int i=0;
  for(i=0; i<(valueBits*2); i++){
    if (i<valueBits){
      result[i/8] |= (value1 & (1 << i));
    }
    else{
      result[i/8] |= (value2 & (1 << (i - valueBits)));
    }
  }
  return result;
}

//Función para eliminar el MSB de cada byte de la llave
unsigned int * keyTrim(unsigned int * key, int size){
  int i=0;
  int mask = 127;
  unsigned int temporal_value = 0;
  unsigned int temporal_carrier = 0;
  unsigned int *result = malloc(7 * sizeof(unsigned int));
  for (i=0; i<size; i++){
    temporal_value = key[i] & mask;
    if (i>0){
      temporal_carrier = temporal_value & ((int)pow(2, i)-1);
      result[i-1] = result[i-1] | (temporal_carrier << (8 - i));
    }
    if(i<size-1){
      result[i] = temporal_value >> i;
    }
  }
  return result;
}

//Función que realiza la permutación de una llave con respecto a "choicePermutationTable"
unsigned long int permutateKey(unsigned long int key){
  int i;
  int position;
  unsigned long int new_key = 0;
  unsigned long int mask = 0;
  unsigned long int value = 0;
  for(i=0; i<56; i++){
    position = choicePermutationTable[i];
    if (position != -1){
      mask = 1<<i;
      value = key & mask;
      if(position == i){
        new_key = new_key | value;
      }else if(position > i){
        new_key = new_key | (value << (position - i + 1));
      }else{
        new_key = new_key | (value >> (i - position + 1));
      }
    }
  }
  return new_key;
}

//Función que tiene el objetivo de llenar el arreglo "subkeys" a partir de la funcionalidad
//  DES de Key Mixing para la creación de una subllave que será utilizada para cada iteración
void keyMixing(unsigned int * trimmedKey, int size){
  unsigned long int new_key = arrayToLong(trimmedKey, 7, 8);
  unsigned long int mask_left = 72057593769492480;
  unsigned long int mask_right = 268435455;
  unsigned long int msb_mask = 0;
  unsigned long int carry = 0;
  int i=0;
  for(i=0; i<16; i++){
    unsigned long int left = (new_key & mask_left) >> 28;
    unsigned long int right = new_key & mask_right;

    int shifts = shiftingBits[i];
    msb_mask = ((int)pow(2, shifts)-1) << 28;
    left = left << shifts;
    carry = left & msb_mask;
    left = left & mask_right;
    left = left | (msb_mask >> 28);

    right = right << shifts;
    carry = right & msb_mask;
    right = right & mask_right;
    right = right | (msb_mask >> 28);

    subkeys[i] = right | (left << 28);
    subkeys[i] = permutateKey(subkeys[i]);
    new_key = subkeys[i];

  }
}

//Función que tiene el objetivo de llenar el arreglo "subkeys" a partir de la funcionalidad
//  DES de Key Mixing para la creación de una subllave que será utilizada para cada iteración
void invKeyMixing(unsigned int * trimmedKey, int size){
  unsigned long int new_key = arrayToLong(trimmedKey, 7, 8);
  unsigned long int mask_left = 72057593769492480;
  unsigned long int mask_right = 268435455;
  unsigned long int lsb_mask = 0;
  unsigned long int carry = 0;
  int i=0;
  for(i=0; i<16; i++){
    unsigned long int left = (new_key & mask_left) >> 28;
    unsigned long int right = new_key & mask_right;

    int shifts = shiftingBits[i];
    lsb_mask = ((int)pow(2, shifts)-1);
    carry = left & lsb_mask;
    left = left & mask_right;
    left = left >> shifts;
    left = left | (carry >> 28);

    carry = right & lsb_mask;
    right = right & mask_right;
    right = right >> shifts;
    right = right | (carry << 28);

    subkeys[i] = right | (left << 28);
    subkeys[i] = permutateKey(subkeys[i]);
    new_key = subkeys[i];

  }
}

void calculateSubkeys(unsigned char option){
  unsigned int * trimmed;
  trimmed = keyTrim(DES_KEY, 8);

  if(option == '1'){
    keyMixing(trimmed, 7);
    printf("Encrypted keys");
  }else{
    invKeyMixing(trimmed, 7);
    printf("Decrypted keys");
  }

  printLongArray(subkeys, 16);
}

//Función para permutar arrays de información usando una tabla de permutaciones.
unsigned char * permutateArrayData(unsigned char * data, int size, int * permutationTable ){
  int i=0;
  unsigned char *result = malloc(size * sizeof(unsigned char));
  unsigned int mask = 0;
  int position = 0;
  int data_bit = 0;

  for(i=0; i<(size*8); i++){
    mask = 1 << (i%8);
    position = permutationTable[i]; //Obtener la posicion a la que se permutara el bit
    if(position != -1){
      data_bit = (data[i/8]) & mask;
      data_bit = data_bit >> i%8; //Mover el bit a la posicion 0 para no tener que hacer un if
      data_bit = data_bit << position%8; //Mover el bit a la posicion deseada
      result[position/8] = data_bit;
    }
  }
  return result;
}

//Función para permutar unsigned long ints de información usando una tabla de permutaciones.
unsigned long int permutateLongData(unsigned long int data, int size, int * permutationTable ){
  int i=0;
  unsigned long int result =0;
  unsigned int mask = 0;
  int position = 0;
  int data_bit = 0;

  for(i=0; i<size; i++){
    mask = 1 << i;
    position = permutationTable[i]; //Obtener la posicion a la que se permutara el bit
    if(position != -1){
      data_bit = data & mask;
      if(i==position){
        result = result | data_bit;
      }else if(i>position){
        result = result | (data_bit >> (i-position));
      }else{
        result = result | (data_bit << (position-i));
      }
    }
  }
  return result;
}

//Función para permutar unsigned long ints de información usando una tabla de permutaciones.
unsigned long int expansionLongPermutation(unsigned long int data, int size, int permutationTable[32][2] ){
  int i=0;
  int j=0;
  unsigned long int result =0;
  unsigned int mask = 0;
  int position = 0;
  int data_bit = 0;

  for(i=0; i<size; i++){
    mask = 1 << i;
    for(j=0; j<2; j++){
      position = permutationTable[i][j]; //Obtener la posicion a la que se permutara el bit
      if(position != -1){
        data_bit = data & mask;
        if(i==position){
          result = result | data_bit;
        }else if(i>position){
          result = result | (data_bit >> (i-position));
        }else{
          result = result | (data_bit << (position-i));
        }
      }
    }
  }
  return result;
}

unsigned long int substitutionLong(unsigned long int data, int size){
  unsigned long int msb_mask = 1<<6;
  unsigned long int lsb_mask = 1;
  unsigned long int middle_mask = 30;
  unsigned long int result = 0;
  int i=0;
  int position, row_msb, row_lsb, row, col;
  for(i=0; i<(size/6); i++){
    position = i*6;
    row_msb = (data & (msb_mask<<position));
    row_lsb = (data & (lsb_mask<<position));
    row = (row_msb >> position) | (row_lsb >> position);
    col = (data & (middle_mask << position));
    col = (col>>position)>>1;
    result |= ((subBox[i][row][col]) << (i*4));
  }
  return result;
}

unsigned long int Feistel(unsigned long int right, int iteration){
  right = expansionLongPermutation(right, 32, expansionPermutationTable);
  right ^= subkeys[iteration];
  right = substitutionLong(right, 32);
  right = permutateLongData(right, 32, pBox);
  return right;
}

unsigned char * encrypt(unsigned char * data){
  int i=0;
  unsigned long int middle;
  data = permutateArrayData(data, 8, initialPermutationTable);

  unsigned int left_array[4] = {data[0], data[1], data[2], data[3]};
  unsigned int right_array[4] = {data[4], data[5], data[6], data[7]};
  unsigned long int left = arrayToLong(left_array, 4, 8);
  unsigned long int right = arrayToLong(right_array, 4, 8);

  for(i=0; i<16; i++){
    middle = right;
    right = Feistel(right, i);
    right ^= left;
    left = middle;
  }

  data = longsToArray(right, left, 32, 8);
  data = permutateArrayData(data, 8, finalPermutationTable);
  return data;
}

unsigned char * decrypt(unsigned char * data){
  int i=0;
  unsigned long int middle;
  data = permutateArrayData(data, 8, finalPermutationTable);

  unsigned int left_array[4] = {data[0], data[1], data[2], data[3]};
  unsigned int right_array[4] = {data[4], data[5], data[6], data[7]};
  unsigned long int left = arrayToLong(left_array, 4, 8);
  unsigned long int right = arrayToLong(right_array, 4, 8);

  for(i=15; i>=0; i--){
    middle = right;
    right = Feistel(right, i);
    right ^= left;
    left = middle;
  }

  data = longsToArray(right, left, 32, 8);
  data = permutateArrayData(data, 8, initialPermutationTable);
  return data;
}


// Main... No se necesita explicación
int main(int argc, char const *argv[]) {
  FILE * Origin;
  FILE * Destiny;
  char OriginName[MAXSTR];
  char DestinyName[MAXSTR];
  unsigned char buffer[8];
  int bytes_read = 0;
  unsigned char option = '0';
  int exit_bool = 0;
  int actual_byte = 0;
  int i=0;

  //Solicitar y abrir el origen
  printf("\nInsert the name of the ORIGIN file:\n");
  scanf("%s", OriginName);
  Origin = fopen(OriginName, "r+b");
  if (Origin != NULL){
    //Solicitar y abrir el destino
    printf("\nInsert the name of the DESTINY file:\n");
    scanf("%s", DestinyName);
    Destiny = fopen(DestinyName, "w+b");
    if(Destiny!=NULL){

      //Menu simple sobre lo que se desea hacer
      printf("\nSelect an option:\n");
      printf("1.- Encrypt\n");
      printf("2.- Decrypt\n");
      printf("3.- Exit");

      while(exit_bool == 0){
        scanf("%c", &option);
        //option = fgetc(stdin);

        if (option == '3'){
          exit(0);
        }else if(option == '1'){
          printf("\nEncrypting...\n");
          exit_bool = 1;
        }else if(option == '2'){
          printf("\nDecrypting...\n");
          exit_bool = 1;
        }
      }

      //Saltar los primeros 54 bytes del documento (headers)
      printf("\nJump Headers");
      fseek(Origin, 54, SEEK_SET);

      //Calcular todos los subkeys

      printf("\n\nCalculating subkeys...");
      calculateSubkeys(option);
      printf("\ncalculated");

      //Leer de 64 bits en 64s
      bytes_read = fread(&buffer, 1, 8, Origin);
      while(bytes_read != 0){
        unsigned char * buffer_result;

        if(bytes_read < 8){
          printf("\nBytes read < 8");
          for(i=bytes_read-1; i<8; i++){
            buffer[i] = 255;
          }
        }

        if(option == '1'){
          buffer_result = encrypt(buffer);
        }else{
          buffer_result = decrypt(buffer);
        }
        fwrite(&buffer_result, 1, 8, Destiny);
        bytes_read = fread(&buffer, 1, 8, Origin);
      }
      printf("\nAFTER READ");
      //cerrar el archivo destino
      fclose(Destiny);
      printf("\nProceso Terminado...\n");
    }else{
      printf("\nDestino: No se pudo abrir el archivo.\n");
    }
    //cerrar el archivo origen
    fclose(Origin);
  }else{
    printf("\nOrigen: No se pudo abrir el archivo.\n");
  }

}
