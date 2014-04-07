#ifndef _GRAPH_DEF_H_
#define _GRAPH_DEF_H_

#include <stdbool.h>
#include <locale.h>

/* CONSTANTES */
#define MAXV 			7000	/* número máximo de nodos */
#define MAXARC      	7000    /* número máximo de arcos - caminos circulares */
#define MAXDEGREE 		600     /* número máximo de aristas entrando o saliendo de un nodo */
#define MAXPATHLEVEL 	10		/* número máximo de niveles en un grafo */
#define MAXPATH     	700000  /* número máximo de caminos diferentes */
#define MAXLABEL    	128		/* tamaño maximo de la cadena para etiquetas */
#define MAXLINE     	128	
#define TRUE			1
#define FALSE			0

#define STACKSIZE   	3000    	  /* Tamaño máximo de la pila para almacenar nodos por visitar */

#define CUENTA_TIPO   		0         /* Por default, el tipo asignado a cuentas */
#define NO_CAPACITY   		FALSE
#define TOLERANCIA    		0.001	  /* Nivel de tolerancia para igualar capacidad y con la suma */
#define CONSUMO_TOLERANCIA 	0 		  /* Por debajo de este número ignoramos las relaciones de consumo */

/* ESTRUCTURAS */

/* Relaciones, o aristas, en el grafo con información de
   (a) Relaciones de consumo
   (b) Caminos
   (c) Arcos (caminos circulares)
*/
struct graph {
	int nvertices;					       		/* número de nodos en el grafo */
	int nedges;						       		/* número de aristas en el grafo */
	int npaths;                             	/* número de caminos en el grafo */
	int narcs;                              	/* número de vertices que forman ciclos */
    int ntypes;                             	/* Número de tipos diferentes de nodos */
    int nvalid;                             	/* Número de nodos válidos */
	// (a) Relaciones de consumo
	int degree [MAXV+1];						/* número de salidas del nodo  */
	int indegree [MAXV+1];						/* número de entradas del nodo  */
	int edges[MAXV+1][MAXDEGREE];  	        	/* información de adyacencia -- nodos salientes */
	int back_edges[MAXV+1][MAXDEGREE];  		/* información de adyacencia -- nodos entrantes */
	int tipo_consumo[MAXV+1][MAXDEGREE];        /* 1= fijo 2=prop */
	double consumo[MAXV+1][MAXDEGREE];      	/* cantidad del recurso consumida por cada nodo receptor*/
	double flujo[MAXV+1][MAXDEGREE];            /* flujo total de dinero a través de la relación */
	double flujo_fijo[MAXV+1][MAXDEGREE];       /* flujo de dinero a través de la relación via SC1*/
	double flujo_prop[MAXV+1][MAXDEGREE];       /* flujo de dinero a través de la relación via SC3*/
	double flujo_porc[MAXV+1][MAXDEGREE];       /* flujo porcentual de dinero (en relación a capacidad)*/
 	// (b) Caminos
    int discarded[MAXV+1][MAXDEGREE];           /* Aristas descartadas durante proceso de encontrar caminos */
    int path[MAXPATH+1][MAXPATHLEVEL];          /* Caminos en el grafo */
    double path_value[MAXPATH+1];               /* Valor de dinero que pasa por el camino */
    int path_type[MAXPATH+1];                   /* Tipo de camino 0=incompleto 1=completo */
    int path_length[MAXPATH+1];                 /* Número de niveles en cada camino */
    int path_index[MAXPATH+1];                  /* Indice con la suma de los indices de nodos en cada camino */
    int node_type[MAXV+1];                      /* Tipos de nodos diferentes */
	//  (c) Arcos
    int feedback_arc[MAXARC+1][2];	       		/* conjunto de vertices que forman ciclos  */
    int feedback_arc_tipo_consumo[MAXARC+1];  	/* 1= fijo 2=prop */
	double feedback_arc_consumo[MAXARC+1];     	/* cantidad del recurso consumida por cada nodo receptor*/
} g;

// Nodos, o vértices, del grafo
struct node {
	int  indice;						/* indice generado por el proceso de carga */
	int  tipo_nodo;						/* asignado según archivo de niveles */
	char label[MAXLABEL];
	bool  flag_capacity;
    bool  flag_consolidado;
    bool  flag_sorted;
    bool  flag_valid;
    bool  flag_tasa;                    // Nodo tiene tasa
	double capacidad;
	double utilizacion;
	double costo_primario_fijo;
	double costo_primario_proporcional;
	double costo_secundario_fijo;
	double costo_secundario_proporcional;
	double tasa_fija;
	double tasa_proporcional;
	bool  congelar_tasas;
	double total_entradas;
	double total_salidas;
	double valor_nodo;
} n[MAXV+1];

// Parámetros comunes a todo el programa
struct params {
    char client[4+1];
    char year[4+1];
    char month[2+1];
    char version[4+1];
    char user[128];
    char algoritmo[3+1];
    char *date;
    char timestamp[26];
    bool pdf;
    bool png;
    bool path_cube;
    bool path_file;
    bool out;                       //Genera archivos de carga, sintetizados
    double tolerancia_consumo;
    double val_min;
} p;

FILE *flog;							/* Apuntador para archivo con LOG */

//Experimental
struct graph_search{
	bool discovered[MAXV];  /* nodos que han sido encontrados */
	int  parent[MAXV];		/* relación de descubrimiento */
} s;
#endif
