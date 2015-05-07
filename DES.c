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

typedef unsigned long int ulongint;
typedef unsigned int uint;
typedef unsigned char uchar;

//Función simple para imprimir todas las localidades de un uint array
void printIntArray(uint * array, int size){
  int i=0;
  printf("\nArray:\n");
  for (i=0; i<size; i++){
    printf("%d ", array[i]);
  }
  printf("\n");
}

//Función simple para imprimir todas las localidades de un ulongint array
void printLongArray(ulongint * array, int size){
  int i=0;
  printf("\nLArray:\n");
  for (i=0; i<size; i++){
    printf("%ld , ", array[i]);
  }
  printf("\n");
}

ulongint arrayToLong(uint * array, int arraySize, int cellSize){
  int i;
  unsigned long result = 0;
  for(i=0; i<arraySize; i++){

    ulongint temp = array[i];
    temp = temp << (cellSize*i);
    result = result | temp;
  }
  return result;
}

uchar * longsToArray(ulongint value1, ulongint value2, int valueBits, uchar * data, int arraySize){
  int i=0;
  for(i=0; i<(valueBits*2); i++){
    if (i<valueBits){
      data[i/8] |= (value1 & (1 << i));
    }
    else{
      data[i/8] |= (value2 & (1 << (i - valueBits)));
    }
  }
  return data;
}

//Función para eliminar el MSB de cada byte de la llave
uint * keyTrim(uint * key, int size){
  int i=0;
  int mask = 127;
  uint temporal_value = 0;
  uint temporal_carrier = 0;
  uint *result = malloc(7 * sizeof(uint));
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
ulongint permutateKey(ulongint key){
  int i;
  int position;
  ulongint new_key = 0;
  ulongint mask = 0;
  ulongint value = 0;
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
void keyMixing(uint * trimmedKey, int size){
  ulongint new_key = arrayToLong(trimmedKey, 7, 8);
  ulongint mask_left = 72057593769492480;
  ulongint mask_right = 268435455;
  ulongint msb_mask = 0;
  ulongint carry = 0;
  int i=0;
  for(i=0; i<16; i++){
    ulongint left = (new_key & mask_left) >> 28;
    ulongint right = new_key & mask_right;

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
void invKeyMixing(uint * trimmedKey, int size){
  ulongint new_key = arrayToLong(trimmedKey, 7, 8);
  ulongint mask_left = 72057593769492480;
  ulongint mask_right = 268435455;
  ulongint lsb_mask = 0;
  ulongint carry = 0;
  int i=0;
  for(i=0; i<16; i++){
    ulongint left = (new_key & mask_left) >> 28;
    ulongint right = new_key & mask_right;

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

void calculateSubkeys(){
  uint * trimmed;
  trimmed = keyTrim(DES_KEY, 8);
  keyMixing(trimmed, 7);
  printLongArray(subkeys, 16);
}

//Función para permutar arrays de información usando una tabla de permutaciones.
uchar * permutateArrayData(uchar * data, int size, int * permutationTable ){
  int i=0;
  uchar result[size];
  uint mask = 0;
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
  for(i=0; i<size;i++){
    data[i] = result[i];
  }
  return data;
}

//Función para permutar ulongints de información usando una tabla de permutaciones.
ulongint permutateLongData(ulongint data, int size, int * permutationTable ){
  int i=0;
  ulongint result =0;
  uint mask = 0;
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

//Función para permutar ulongints de información usando una tabla de permutaciones.
ulongint expansionLongPermutation(ulongint data, int size, int permutationTable[32][2] ){
  int i=0;
  int j=0;
  ulongint result =0;
  uint mask = 0;
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

ulongint substitutionLong(ulongint data, int size){
  ulongint msb_mask = 1<<6;
  ulongint lsb_mask = 1;
  ulongint middle_mask = 30;
  ulongint result = 0;
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

ulongint Feistel(ulongint right, int iteration){
  right = expansionLongPermutation(right, 32, expansionPermutationTable);
  right ^= subkeys[iteration];
  right = substitutionLong(right, 32);
  right = permutateLongData(right, 32, pBox);
  return right;
}

uchar * encrypt(uchar * data){
  int i=0;
  ulongint middle;
  data = permutateArrayData(data, 8, initialPermutationTable);

  uint left_array[4] = {data[0], data[1], data[2], data[3]};
  uint right_array[4] = {data[4], data[5], data[6], data[7]};
  ulongint left = arrayToLong(left_array, 4, 8);
  ulongint right = arrayToLong(right_array, 4, 8);

  for(i=0; i<16; i++){
    middle = right;
    right = Feistel(right, i);
    right ^= left;
    left = middle;
  }

  data = longsToArray(right, left, 32, data, 8);
  data = permutateArrayData(data, 8, finalPermutationTable);
  return data;
}

uchar * decrypt(uchar * data){
  int i=0;
  ulongint middle;
  data = permutateArrayData(data, 8, initialPermutationTable);

  uint left_array[4] = {data[0], data[1], data[2], data[3]};
  uint right_array[4] = {data[4], data[5], data[6], data[7]};
  ulongint left = arrayToLong(left_array, 4, 8);
  ulongint right = arrayToLong(right_array, 4, 8);

  for(i=15; i>=0; i--){
    middle = right;
    right = Feistel(right, i);
    right ^= left;
    left = middle;
  }

  data = longsToArray(right, left, 32, data, 8);
  data = permutateArrayData(data, 8, finalPermutationTable);
  return data;
}


// Main... No se necesita explicación
int main(int argc, char const *argv[]) {
  FILE * Origin;
  FILE * Destiny;
  char OriginName[MAXSTR];
  char DestinyName[MAXSTR];
  uchar buffer[8];
  int bytes_read = 0;
  uchar option = '0';
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
      printf("\nWriting headers...");
      uchar headers[54];
      fread(&headers, 1, 54, Origin);
      fwrite(&headers, 1, 54, Destiny);

      //Calcular todos los subkeys

      printf("\n\nCalculating subkeys...");
      calculateSubkeys();
      printf("\ncalculated");

      //Leer de 64 bits en 64s
      bytes_read = fread(&buffer, 1, 8, Origin);
      while(bytes_read != 0){
        uchar * buffer_result;

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
      printf("\n\nFINISH!!\n");
    }else{
      printf("\nDESTINY: The file couldn't be opened/created.\n");
    }
    //cerrar el archivo origen
    fclose(Origin);
  }else{
    printf("\nORIGIN: The file couldn't be opened.\n");
  }

}
