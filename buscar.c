#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define MAX 10000000

int numero_lineas(char *ruta, int *tam_lineas){
	if (ruta!= NULL){
		FILE* ar = fopen(ruta, "r");
		int lineas = 0;
		int tam_linea;
		while(!feof(ar)){
			tam_linea++;
			char c = getc(ar);
			if (c=='\n'){
				if(tam_lineas != NULL){
					tam_lineas[lineas] = tam_linea;
				}
				lineas++;
				tam_linea = 0;
			}
		}
		fclose(ar);
		return lineas;
	}
	return -1;
}

//cuantas lineas va a leer
//desde donde va a leer
//la ruta del archivo
//fseek (cada hilo abre el archivo)

//Declaramos una estructura
typedef struct mi_estructuraTDA{
	int reader;
	int star;
	char *ruta;
} estructura;

//Funcion que recibe cada hilo para realizar la suma del subarreglo
void *funcionHilo(void *arg) {
	estructura *argumentos = (estructura *) arg;
	int *temp = argumentos->array;
	int suma = 0;

	for (int i=argumentos->ini; i<=argumentos->fin;i++)
		suma += *(temp+i);

	return (void *)suma;
}

//Main
int main(int argc, char *argv[]) {
	if (argc < 4)
		printf("Uso: ./buscar <ruta> <numero de hilos> <palabra a buscar 1> ... <palabra a buscar n>\n");
	else {
		
	}
}
