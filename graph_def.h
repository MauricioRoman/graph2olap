#ifndef _GRAPH_DEF_H_
#define _GRAPH_DEF_H_

#include <stdbool.h>
#include <locale.h>

/* CONSTANTES */
#define MAXV 			7000	/* n�mero m�ximo de nodos */
#define MAXARC      	7000    /* n�mero m�ximo de arcos - caminos circulares */
#define MAXDEGREE 		600     /* n�mero m�ximo de aristas entrando o saliendo de un nodo */
#define MAXPATHLEVEL 	10		/* n�mero m�ximo de niveles en un grafo */
#define MAXPATH     	700000  /* n�mero m�ximo de caminos diferentes */
#define MAXLABEL    	128		/* tama�o maximo de la cadena para etiquetas */
#define MAXLINE     	128	
#define TRUE			1
#define FALSE			0

#define STACKSIZE   	3000    	  /* Tama�o m�ximo de la pila para almacenar nodos por visitar */

#define CUENTA_TIPO   		0         /* Por default, el tipo asignado a cuentas */
#define NO_CAPACITY   		FALSE
#define TOLERANCIA    		0.001	  /* Nivel de tolerancia para igualar capacidad y con la suma */
#define CONSUMO_TOLERANCIA 	0 		  /* Por debajo de este n�mero ignoramos las relaciones de consumo */

/* ESTRUCTURAS */

/* Relaciones, o aristas, en el grafo con informaci�n de
   (a) Relaciones de consumo
   (b) Caminos
   (c) Arcos (caminos circulares)
*/
struct graph {
	int nvertices;					       		/* n�mero de nodos en el grafo */
	int nedges;						       		/* n�mero de aristas en el grafo */
	int npaths;                             	/* n�mero de caminos en el grafo */
	int narcs;                              	/* n�mero de vertices que forman ciclos */
    int ntypes;                             	/* N�mero de tipos diferentes de nodos */
    int nvalid;                             	/* N�mero de nodos v�lidos */
	// (a) Relaciones de consumo
	int degree [MAXV+1];						/* n�mero de salidas del nodo  */
	int indegree [MAXV+1];						/* n�mero de entradas del nodo  */
	int edges[MAXV+1][MAXDEGREE];  	        	/* informaci�n de adyacencia -- nodos salientes */
	int back_edges[MAXV+1][MAXDEGREE];  		/* informaci�n de adyacencia -- nodos entrantes */
	int tipo_consumo[MAXV+1][MAXDEGREE];        /* 1= fijo 2=prop */
	double consumo[MAXV+1][MAXDEGREE];      	/* cantidad del recurso consumida por cada nodo receptor*/
	double flujo[MAXV+1][MAXDEGREE];            /* flujo total de dinero a trav�s de la relaci�n */
	double flujo_fijo[MAXV+1][MAXDEGREE];       /* flujo de dinero a trav�s de la relaci�n via SC1*/
	double flujo_prop[MAXV+1][MAXDEGREE];       /* flujo de dinero a trav�s de la relaci�n via SC3*/
	double flujo_porc[MAXV+1][MAXDEGREE];       /* flujo porcentual de dinero (en relaci�n a capacidad)*/
 	// (b) Caminos
    int discarded[MAXV+1][MAXDEGREE];           /* Aristas descartadas durante proceso de encontrar caminos */
    int path[MAXPATH+1][MAXPATHLEVEL];          /* Caminos en el grafo */
    double path_value[MAXPATH+1];               /* Valor de dinero que pasa por el camino */
    int path_type[MAXPATH+1];                   /* Tipo de camino 0=incompleto 1=completo */
    int path_length[MAXPATH+1];                 /* N�mero de niveles en cada camino */
    int path_index[MAXPATH+1];                  /* Indice con la suma de los indices de nodos en cada camino */
    int node_type[MAXV+1];                      /* Tipos de nodos diferentes */
	//  (c) Arcos
    int feedback_arc[MAXARC+1][2];	       		/* conjunto de vertices que forman ciclos  */
    int feedback_arc_tipo_consumo[MAXARC+1];  	/* 1= fijo 2=prop */
	double feedback_arc_consumo[MAXARC+1];     	/* cantidad del recurso consumida por cada nodo receptor*/
} g;

// Nodos, o v�rtices, del grafo
struct node {
	int  indice;						/* indice generado por el proceso de carga */
	int  tipo_nodo;						/* asignado seg�n archivo de niveles */
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

// Par�metros comunes a todo el programa
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
	int  parent[MAXV];		/* relaci�n de descubrimiento */
} s;
#endif
