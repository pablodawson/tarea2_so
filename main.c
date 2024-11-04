// Tarea 2: Ordenamiento con hilos
// Pablo Dawson A.
// Compilar con: gcc main.c -pthread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// Estructura para pasar argumentos a los hilos
typedef struct {
    char input_file[50];  // Nombre del archivo de entrada
    char output_file[50]; // Nombre del archivo de salida
    int algorithm;        // 0 para QuickSort, 1 para BubbleSort
} ThreadArgs;

// Funcion para Bubble Sort
void bubble_sort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                // Intercambiar arr[j] y arr[j+1]
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

// Funciones para Quick Sort
void swap(int* a, int* b) {
    int t = *a; 
    *a = *b; 
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    int j = 0;

    for (j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

// Función que ejecutarán los hilos
void* sort_file(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    FILE *input_file, *output_file;
    int numbers[1000];  // Asumimos máximo 1000 números
    int count = 0;

    // Abrir archivo de entrada
    input_file = fopen(thread_args->input_file, "r");
    if (input_file == NULL) {
        printf("Error al abrir archivo %s\n", thread_args->input_file);
        pthread_exit(NULL);
    }

    // Leer números del archivo
    while (fscanf(input_file, "%d", &numbers[count]) != EOF && count < 1000) {
        count++;
    }
    fclose(input_file);

    // Ordenar según el algoritmo especificado
    if (thread_args->algorithm == 0) {
        quick_sort(numbers, 0, count - 1);
    } else {
        bubble_sort(numbers, count);
    }

    // Escribir resultado en archivo de salida
    output_file = fopen(thread_args->output_file, "w");
    if (output_file == NULL) {
        printf("Error al crear archivo %s\n", thread_args->output_file);
        pthread_exit(NULL);
    }

    int i = 0;
    for (i = 0; i < count; i++) {
        fprintf(output_file, "%d\n", numbers[i]);
    }
    fclose(output_file);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[4];
    ThreadArgs thread_args[4];
    
    // Configurar argumentos para cada hilo
    int i = 0;
    for (i = 0; i < 4; i++) {
        sprintf(thread_args[i].input_file, "Datos%d.txt", i+1);
        sprintf(thread_args[i].output_file, "Out%d.txt", i+1);
        thread_args[i].algorithm = (i < 2) ? 0 : 1;  // 0=QuickSort, 1=BubbleSort
    }

    // Crear los hilos
    for (i = 0; i < 4; i++) {
        printf("Creando hilo %d para ordenar %s usando %s\n", 
               i+1, 
               thread_args[i].input_file,
               thread_args[i].algorithm == 0 ? "QuickSort" : "BubbleSort");
               
        if (pthread_create(&threads[i], NULL, sort_file, (void*)&thread_args[i]) != 0) {
            perror("Error al crear hilo");
            exit(1);
        }
    }

    // Esperar a que todos los hilos terminen y mostrar resultados
    for (i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
        
        // Leer y mostrar archivo de salida
        printf("\nContenido del archivo %s:\n", thread_args[i].output_file);
        FILE *output_file = fopen(thread_args[i].output_file, "r");
        if (output_file == NULL) {
            printf("Error al abrir archivo %s\n", thread_args[i].output_file);
            continue;
        }

        int number;
        while (fscanf(output_file, "%d", &number) != EOF) {
            printf("%d ", number);
        }
        printf("\n");
        fclose(output_file);
    }

    return 0;
}
