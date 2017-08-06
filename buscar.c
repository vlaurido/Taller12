#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 10000000

//Variables globales que nos van a servir
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int npalabras;
int *num_palabras;
char **palabras;

int numero_lineas(char *ruta, int *tam_lineas) {
	if (ruta!= NULL) {
		FILE* ar = fopen(ruta, "r");
		int lineas = 0;
		int tam_linea;
		while(!feof(ar)) {
			tam_linea++;
			char c = getc(ar);
			if (c=='\n') {
				if(tam_lineas != NULL)
					tam_lineas[lineas] = tam_linea;
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
	int lines_to_read;
	int start;
	char *ruta;
} estructura;

//Funcion que recibe cada hilo
void *funcionHilo(void *arg) {
	pthread_mutex_lock(&mutex);

	estructura *argumentos = (estructura *)arg;
	char *ruta = argumentos->ruta;

	FILE *fp = fopen(ruta,"r");
	if (fp == NULL) {
		printf("Error al abrir el archivo\n");
		exit(-1);
	}

	if (fseek(fp,argumentos->start,SEEK_SET)==0) {
		int cont = 0;
		while (cont<argumentos->lines_to_read) {
			char *line = (char *)malloc(MAX*sizeof(char));
			char *token;

			fgets(line,MAX,fp);
			char *rest = line;
			token = strtok_r(rest," ,.!?;:\e\\\?\f\n\t-_",&rest);

			while (token != NULL) {
				for (int i=0;i<npalabras;i++) {
					if (strcmp(token,palabras[i])==0)
						num_palabras[i]++;
				}
				token = strtok_r(NULL," ,.!?;:\e\\\?\f\n\t-_",&rest);
			}

			free(line);
			cont++;
		}
	}

	fclose(fp);
	pthread_mutex_unlock(&mutex);

	return (void *)0;
}

//Funcion para imprimir por segundo el numero de palabras encontradas
void * printxSec(void * arg)
{
	while(1)
  	{
    		pthread_mutex_lock(&mutex);
    		for(int i = 0; i<npalabras; i++){
      			printf("%i: %s  aparece %i veces\n",i+1,palabras[i],num_palabras[i]);
    		}
		pthread_mutex_unlock(&mutex);
    		sleep(1);
  	}
}

//Main
int main(int argc, char *argv[]) {

	int numOfThreads;
	char *ruta;
	double linesxth;

	if (argc < 4)
		printf("Uso: ./buscar <ruta> <numero de hilos> <palabra a buscar 1> ... <palabra a buscar n>\n");
	else {
		ruta = argv[1];
		numOfThreads = atoi(argv[2]);
		npalabras = argc - 3;

		palabras = (char **)malloc(npalabras*sizeof(char*));
		for (int i=0;i<npalabras;i++)
			palabras[i] = argv[i+3];
		num_palabras = (int *)malloc(npalabras*sizeof(int));
		pthread_t *ids = (pthread_t *)malloc(numOfThreads*sizeof(pthread_t));
		
		int *tam_lineas = (int *)malloc(MAX*sizeof(int));
		int numOfLines = numero_lineas(ruta,tam_lineas);

		linesxth = numOfLines/numOfThreads;

		if ((numOfLines % numOfThreads) != 0) {
			linesxth = ceil(linesxth);
			linesxth++;
		}
		
		//Hilo para imprimir numero de palabras encontradas por segundo 
		pthread_t idPrintThread;
		pthread_create(&idPrintThread, NULL, printxSec, NULL);

		int inicio = 0;
		for (int i=0;i<numOfThreads;i++) {
			estructura *structarg = malloc(sizeof(estructura));
			if (i == numOfThreads-1)
				structarg->lines_to_read = numOfLines-inicio;
			else
				structarg->lines_to_read = linesxth;
			structarg->start = inicio;
			structarg->ruta = ruta;
			pthread_create(&ids[i],NULL,funcionHilo,(void *)structarg);
			inicio+=linesxth;
		}
		
		//Funciona para guardar el valor que retorna el hilo
		for (int i = 0; i < numOfThreads; i++)
		{
			void *valor_retorno = malloc(sizeof(void *));		
			pthread_join(ids[i], &valor_retorno);
		}

		//Mostrando conteo final
		for(int i = 0; i<npalabras; i++){
			printf("%i: %s  aparece %i veces\n",i+1,palabras[i],num_palabras[i]);
		}

		pthread_mutex_destroy(&mutex);
	}
	return 0;
}
