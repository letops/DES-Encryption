/*  Author: Daniel Alejandro Ortíz Pérez - A01204096
            Denisse Lizbeth Maldonado Flores - A01062721
    Program: Final Proyect - DES  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXSTR 256
#define KVAL 50

//Crear nueva llave, recordar que es inverso su valor, es decir el bit en DES[7] es el más
// grande de todos
unsigned int DES_KEY[8] = {128, 128, 128, 128, 128, 128, 128, 128};

int expansionPermutationTable[32][2] = {{ 2,48}, { 3,-1}, { 4,-1}, { 5, 7}, { 6, 8}, { 9,-1}, {10,-1}, {11,13},
                                        {12,14}, {15,-1}, {16,-1}, {17,19}, {18,20}, {21,-1}, {22,-1}, {23,25},
                                        {24,26}, {27,-1}, {28,-1}, {29,31}, {30,32}, {33,-1}, {34,-1}, {35,37},
                                        {36,38}, {39,-1}, {40,-1}, {41,43}, {42,44}, {45,-1}, {46,-1}, {47, 1}};

int shiftingBits[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int choicePermutationTable[56]={ 5,24, 7,16, 6,10,20,18,-1,12, 3,15,23, 1,
                                 9,19, 2,-1,14,22,11,-1,13, 4,-1,17,21, 8,
                                47,31,27,48,35,41,-1,46,28,-1,39,32,25,44,
                                -1,37,34,43,29,36,38,45,33,26,42,-1,30,40};

unsigned long int subkeys[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int initialPermutationTable[64]={40,8,48,16,56,24,64,32,
                                 39,7,47,15,55,23,63,31,
                                 38,6,46,14,54,22,62,30,
                                 37,5,45,13,53,21,61,29,
                                 36,4,44,12,52,20,60,28,
                                 35,3,43,11,51,19,59,27,
                                 34,2,42,10,50,18,58,26,
                                 33,1,41,9,49,17,57,25};

int finalPermutationTable[64]=  {58,50,42,34,26,18,10,2,
                                 60,52,44,36,28,20,12,4,
                                 62,54,46,38,30,22,14,6,
                                 64,56,48,40,32,24,16,8,
                                 57,49,41,33,25,17,9,1,
                                 59,51,43,35,27,19,11,3,
                                 61,53,45,37,29,21,13,5,
                                 63,55,47,39,31,23,15,7};

int subBox[8][4][16]={{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},

                    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},

                    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},

                    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},

                    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},

                    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},

                    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},

                    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

int pBox[32]= { 9,17,23,31,13,28, 2,18,
               24,16,30, 6,26,20,10, 1,
                8,14,25, 3, 4,29,11,19,
               32,12,22, 7, 5,27,15,21};

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
  printf("\nNombre del archivo a leer?\n");
  scanf("%s", OriginName);
  Origin = fopen(OriginName, "r+b");
  if (Origin != NULL){
    //Solicitar y abrir el destino
    printf("\nNombre del archivo a escribir?\n");
    scanf("%s", DestinyName);
    Destiny = fopen(DestinyName, "w+b");
    if(Destiny!=NULL){

      //Menu simple sobre lo que se desea hacer
      printf("\nSeleccione la opción\n");
      printf("1.- Encriptar\n");
      printf("2.- Desencriptar\n");
      printf("3.- Salir");

      while(exit_bool == 0){
        scanf("%c", &option);
        //option = fgetc(stdin);

        if (option == '3'){
          exit(0);
        }else if(option == '1'){
          printf("\nEncriptando...\n");
          exit_bool = 1;
        }else if(option == '2'){
          printf("\nDesencriptando...\n");
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
