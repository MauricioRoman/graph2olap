/* Springer Verlag Programming Challenges (2003)
   Funciones para procesar nodos y vértices en un grafo
   Algoritmos de cálculo navegando grafos
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "graph_def.h"
#include "graph.h"
#include "graph_io.h"

int trim_nodes()
{
    int i,j;

    //printf("N vertices = %d\n",g.nvertices);
    for(i=0;i<g.nvertices;i++){
        //printf("IN %d OUT %d\n",g.indegree[i], g.degree[i]);
        if(g.indegree[i] != 0 || g.degree[i] != 0){
            n[i].flag_valid = 1;
            g.nvalid++;
            fprintf(flog,"se encontro nodo valido - %d\n");
        } else {
            printf("Nodo %s no esta conectado\n",n[i].label);
            fprintf(flog,"Nodo %s no esta conectado\n",n[i].label);
        }
    }
    return 1;
}

int process_vertex(int v)
{
		int i;
		bool valid_vertex=0;
        /* Evitamos los cálculos en el nodo si éste ha sido marcado como "congelado" (Alg. Rec.)*/
        /* Igualmente, no calculamos los nodos invalidos */

            /* Acumula el consumo total (Y002) por los receptores y guarda dicho valor en el nodo */
            for (i=0; i< g.degree[v]; i++){
                fprintf(flog,"Agregando consumo para nodo %s\n",n[v].label);
                n[v].utilizacion += g.consumo[v][i];
                fprintf(flog,"Utilizacion = %.4f\n",n[v].utilizacion);
            }

            /* Revisa que la utilizacion no sea mayor que la capacidad cuando existe capacidad limitante */
            if(n[v].flag_capacity){
                // Reducimos la capacidad para evitar fugas de dinero
                // Usando fabs() estaba generando una alerta ??
                double diff = abs(n[v].utilizacion - n[v].capacidad) / n[v].capacidad;
                if( diff < TOLERANCIA){
                    fprintf(flog,"Ajustando capacidad de nodo %d de %.2f a %.2f\n",v,n[v].capacidad,n[v].utilizacion);
                    n[v].capacidad = n[v].utilizacion;
                } else {
                    printf("Nodo %s tiene capacidad no utilizada.\nUtilizacion: %.2f\nCapacidad: %.2f\n",
                           n[v].label, n[v].utilizacion, n[v].capacidad);
                }

                if (n[v].utilizacion > n[v].capacidad) {
                    printf("Utilizacion %.2f excede capacidad %.2f para nodo %d con capacidad limitante\n",n[v].utilizacion,n[v].capacidad,v);
                    exit(EXIT_FAILURE);
                }
            } else {     // Nodo no tiene capacidad limitante -- su utilización es su capacidad
                n[v].capacidad = n[v].utilizacion;
            }

            /* Calcula tasa fija (RT01) y proporcional (RT02) para el nodo y guarda dicho valor en el nodo */
            if(n[v].capacidad){
                n[v].tasa_fija 			= (n[v].costo_primario_fijo + n[v].costo_secundario_fijo) / n[v].capacidad;
            }

            if(n[v].utilizacion) {
                n[v].tasa_proporcional 	= (n[v].costo_primario_proporcional + n[v].costo_secundario_proporcional) / n[v].utilizacion;
            }
        /* Procesamos las aristas que salen del nodo */
		for (i=0; i< g.degree[v]; i++)
			if (valid_edge(v,i))			/* Verifica que el consumo sea mayor a cero */
					process_edge(v,i);		/* Calcula costos secundarios y los acumula en la estructura del nodo receptor */

		fprintf(flog,"Se proceso el nodo %d (congelado=%d)\n",v,n[v].congelar_tasas);
		n[v].flag_tasa=1;
		return 0;
}

int valid_edge(int v, int i)
{
    if((edge_not_arc(v,g.edges[v][i]) == TRUE) && (g.consumo[v][i] > 0.0))
    //if(g.consumo[v][i] > 0.0)
        return TRUE;
    else
        return FALSE;
}

int process_edge(int x, int i)
{
    switch(g.tipo_consumo[x][i]){
			case 1:
                g.flujo_fijo[x][i] = g.consumo[x][i] * (n[x].tasa_fija + n[x].tasa_proporcional);
				n[g.edges[x][i]].costo_secundario_fijo	+= g.flujo_fijo[x][i] ;
                g.flujo[x][i] = g.flujo_fijo[x][i];
                if(n[x].capacidad > 0.0){
                    g.flujo_porc[x][i] = g.consumo[x][i] / n[x].capacidad;
                }
				break;

			case 2:
			    g.flujo_fijo[x][i] = g.consumo[x][i] * n[x].tasa_fija;
			    g.flujo_prop[x][i] = g.consumo[x][i] * n[x].tasa_proporcional;
				n[g.edges[x][i]].costo_secundario_fijo 	+= g.flujo_fijo[x][i];
				n[g.edges[x][i]].costo_secundario_proporcional 	+= g.flujo_prop[x][i];
                g.flujo[x][i] = g.flujo_fijo[x][i] + g.flujo_prop[x][i];
                if(n[x].capacidad > 0.0){
                    g.flujo_porc[x][i] = g.consumo[x][i] / n[x].capacidad;
                }
				break;
	}

	fprintf(flog,"Se proceso la arista (%d,%d) con consumo tipo %d\n",x,i,g.tipo_consumo[x][i]);
	return 0;
}

int toposort(int sorted[])
{
		int indegree[MAXV];							/* Grado INDEGREE de cada nodo */
		queue zeroin;								/* Nodos de INDEGREE 0 */
		int x=0,y=0;									/* Nodo actual y siguiente */
		int i=0,j=0;									/* Contadores */

		for (i=0; i< g.nvertices; i++)
			indegree[i] = g.indegree[i];

		init_queue(&zeroin);
		for (i=0; i< g.nvertices; i++)
			if ((indegree[i] == 0) && (n[i].flag_valid)) enqueue (&zeroin, i);

		j=0;
		while (is_empty(&zeroin) == FALSE) {
			j = j+1;
			//printf("x: %d\n",x);
			x = dequeue (&zeroin);
			sorted[j-1] = x;
			fprintf(flog,"Toposort %d,%d=%s\n",j,x,n[x].label);
			for (i=0; i< g.degree[x]; i++){
				y = g.edges[x][i];
				indegree[y]--;
				if(indegree[y] == 0) enqueue(&zeroin,y);
			}
            fprintf(flog,"j: %d n.valid: %d\n", j, g.nvalid);
		}
        if (j != g.nvalid)				//Grafo no es Acíclico
				return j;

		return 0;
}

//int compute_indegrees()
//{
//		int i,j;

//		for (i=0; i < g.nvertices; i++)
//            g.indegree[i] = 0;

//		for (i=0; i < g.nvertices; i++)
//			for (j=0; j<g.degree[i]; j++)
//                g.indegree[g.edges[i][j]]++;

//		return 0;
//}

int find_node_types()
{
    int i;
    for(i=0;i<g.nvertices;i++){
            if(g.indegree[i] == 0 && g.degree[i]>0)
                n[i].tipo_nodo = 1;                 // Fuente (ej. Cuenta Contable)
            else if(g.indegree[i] > 0 && g.degree[i] == 0)
                 n[i].tipo_nodo = 2;                 // Destino (ej. Producto o Cliente)
            else if(n[i].flag_capacity)
                 n[i].tipo_nodo = 3;                // Recurso (posee capacidad limitante)
            else
                 n[i].tipo_nodo = 4;                // Proceso
    }
    return 0;
}

/**
 * Solves the feedback arc set problem using the heuristics of Eades et al.
 * Rutina basada en la que se encontro en libreria igraph.c
 */
int feedback_arc_set_eades()
{
  int i, j, v, no_of_nodes, nodes_left;
  queue sources, sinks;
  int* ordering;
  int order_next_pos = 0, order_next_neg = -1;
  int diff, maxdiff;

  no_of_nodes=g.nvertices;
  ordering = malloc(no_of_nodes * sizeof (int));

  int indegree[MAXV];             /* Grado INDEGREE de cada nodo */
  int outdegree[MAXV];            /* Grado OUTDEGREE de cada nodo */

  for (i=0; i< g.nvertices; i++)
      indegree[i] = g.indegree[i];

  for (i=0; i< g.nvertices; i++)
      outdegree[i] = g.degree[i];

  init_queue(&sources);
  init_queue(&sinks);
  /* Find initial sources and sinks */
  //nodes_left = no_of_nodes;
  nodes_left = g.nvertices;
  for (i = 0; i < no_of_nodes; i++) {
    if (g.indegree[i] == 0) {
      if (g.degree[i] == 0) {
        /* Isolated vertex, we simply ignore it */
        nodes_left--;
        ordering[i] = order_next_pos++;
        indegree[i] = outdegree[i] = -1;
      } else {
        /* This is a source */
        enqueue (&sources, i);
        //igraph_dqueue_push(&sources, i);
      }
    } else if (g.degree[i] == 0) {
      /* This is a sink */
      //igraph_dqueue_push(&sinks, i);
      enqueue (&sinks, i);
    }
  }

  /* While we have any nodes left... */
  while (nodes_left > 0) {
    /* (1) Remove the sources one by one */
    while (is_empty(&sources) == FALSE) {
      i=dequeue(&sources);
      /* Add the node to the ordering */
      ordering[i] = order_next_pos++;
      /* Exclude the node from further searches */
      indegree[i] = outdegree[i] = -1;

      /* Get the neighbors and decrease their degrees */
      for (j = 0; j < g.degree[i]; j++){
        if (indegree[g.edges[i][j]] <= 0)
           /* Already removed, continue */
          continue;
        indegree[g.edges[i][j]]--;
        if (indegree[g.edges[i][j]] == 0)
          enqueue (&sources, g.edges[i][j]);
      }
      nodes_left--;
    }

    /* (2) Remove the sinks one by one */
    while (is_empty(&sinks) == FALSE) {
      i=dequeue(&sinks);
      /* Maybe the vertex became sink and source at the same time, hence it
       * was already removed in the previous iteration. Check it. */
      if (indegree[i] < 0)
        continue;
      /* Add the node to the ordering */
      ordering[i] = order_next_neg--;
      /* Exclude the node from further searches */
      indegree[i] = outdegree[i] = -1;
      /* Get the neighbors and decrease their degrees */
      for (j = 0; j < g.indegree[i]; j++){
        if (outdegree[g.back_edges[i][j]] <= 0)
           /* Already removed, continue */
          continue;
        outdegree[g.back_edges[i][j]]--;
        if (outdegree[g.back_edges[i][j]] == 0)
          enqueue (&sinks, g.back_edges[i][j]);
      }
      nodes_left--;
    }

    /* (3) No more sources or sinks. Find the node with the largest
     * difference between its out-strength and in-strength */
    v = -1; maxdiff = -MAXV;
    for (i = 0; i < no_of_nodes; i++) {
      if (outdegree[i] < 0)
        continue;
      diff = outdegree[i]-indegree[i];
      if (diff > maxdiff) {
        maxdiff = diff;
        v = i;
      }
    }
    if (v >= 0) {
      /* Remove vertex v */
      ordering[v] = order_next_pos++;
      /* Remove outgoing edges */
      for (j = 0; j < g.degree[v]; j++){
        if (indegree[g.edges[v][j]] <= 0)
           /* Already removed, continue */
          continue;
        indegree[g.edges[v][j]]--;
        if (indegree[g.edges[v][j]] == 0)
          enqueue (&sources, g.edges[v][j]);
      }
      /* Remove incoming edges */
      for (j = 0; j < g.indegree[v]; j++){
        if (outdegree[g.back_edges[v][j]] <= 0)
           /* Already removed, continue */
          continue;
        outdegree[g.back_edges[v][j]]--;
        if (outdegree[g.back_edges[v][j]] == 0 && outdegree[g.back_edges[v][j]] > 0)
          enqueue (&sinks, g.back_edges[v][j]);
      }
      indegree[v] = outdegree[v] = -1;
      nodes_left--;
    }
  }

  /* Tidy up the ordering */
  for (i = 0; i < no_of_nodes; i++) {
    if (ordering[i] < 0)
      ordering[i] += no_of_nodes;
  }

  /* Find the feedback edges based on the ordering */
  j = g.nedges;
  for (i = 0; i < g.nvertices; i++){
    for (j = 0; j < g.degree[i]; j++){
      int from = i, to = g.edges[i][j];
      if (from == to || ordering[from] > ordering[to]){
        g.feedback_arc[g.narcs][0] = from;
        g.feedback_arc[g.narcs][1] = to;
        g.narcs++;
      }
    }

  }
  return 0;
}

int initialize_search()
{
	int i;

	for(i=0; i < g.nvertices; i++){
		s.discovered[i] = FALSE;
		s.parent[i] = -1;
	}
	return 0;
}

int bfs(int start, bool process_flag)
{

	queue q;			/* cola con nodos a visitar */
	int v;				/* nodo actual */
	int i;				/* contador */

	init_queue(&q);
	enqueue(&q, start);
	s.discovered[start] = TRUE;

	while(is_empty(&q) == FALSE){
		v = dequeue(&q);
		if (process_flag)
            process_vertex(v);
		for (i=0; i < g.degree[v]; i++){
			if(edge_not_arc(v,g.edges[v][i]) == TRUE) {
				if (s.discovered[g.edges[v][i]] == FALSE) {
					enqueue(&q,g.edges[v][i]);
					s.discovered[g.edges[v][i]] = TRUE;
					s.parent[g.edges[v][i]] = v;
				}
			}
		}
	}
	return 0;
}

int edge_not_arc(int x, int y)
{
    int i;
    for(i=0;i<g.narcs;i++)
        if((x == g.feedback_arc[i][0]) && (y == g.feedback_arc[i][1]))
            return FALSE;
    return TRUE;
}

int process_vertex_arc(int x)
{
        int i;
        int v=g.feedback_arc[x][0];
        int z=g.feedback_arc[x][1];

        for (i=0; i< g.degree[v]; i++)
            n[v].utilizacion += g.consumo[v][i];
        /* Le agregamos la utilización causada por el arco */
        n[v].utilizacion += g.feedback_arc_consumo[v];

        /* Revisa que la utilizacion no sea mayor que la capacidad cuando existe capacidad limitante */
        if(n[v].flag_capacity){
            if(abs(n[v].utilizacion - n[v].capacidad) < TOLERANCIA)
                n[v].utilizacion = n[v].capacidad;
            else if (n[v].utilizacion > n[v].capacidad) {
                printf("Utilizacion %.2f excede capacidad %.2f para nodo %d con capacidad limitante\n",n[v].utilizacion,n[v].capacidad,v);
                exit(EXIT_FAILURE);
            }
        } else {     // Nodo no tiene capacidad limitante -- su utilización es su capacidad
            n[v].capacidad = n[v].utilizacion;
        }

        /* Calcula tasa fija (RT01) y proporcional (RT02) para el nodo y guarda dicho valor en el nodo */
        if(n[v].capacidad)
            n[v].tasa_fija 			= (n[v].costo_primario_fijo + n[v].costo_secundario_fijo) / n[v].capacidad;
        if(n[v].utilizacion)
            n[v].tasa_proporcional 	= (n[v].costo_primario_proporcional + n[v].costo_secundario_proporcional) / n[v].utilizacion;

        /* Congelamos tasas para que no sean alteradas cuando se reprocese el grafo */
        n[v].congelar_tasas = 1;
        n[z].congelar_tasas = 1;
        /* Procesamos solo las aristas que sale del nodo correspondiente al ARCO */
        switch(g.feedback_arc_tipo_consumo[x]){
			case 1:
				n[z].costo_secundario_fijo	+= g.feedback_arc_consumo[x] * (n[v].tasa_fija + n[v].tasa_proporcional);
				break;
			case 2:
				n[z].costo_secundario_fijo 	+= g.feedback_arc_consumo[x] * n[v].tasa_fija;
				n[z].costo_secundario_proporcional 	+= g.feedback_arc_consumo[x] * n[v].tasa_proporcional;
				break;
        }
        if(n[z].capacidad)
            n[z].tasa_fija 			= (n[z].costo_primario_fijo + n[z].costo_secundario_fijo) / n[z].capacidad;
        if(n[z].utilizacion)
            n[z].tasa_proporcional 	= (n[z].costo_primario_proporcional + n[z].costo_secundario_proporcional) / n[z].utilizacion;

        printf("Se proceso la arista arco (%d,%d) con consumo tipo %d\n",v,z,g.feedback_arc_tipo_consumo[x]);
		printf("Se proceso el nodo %d (congelado=%d)\n",v,n[v].congelar_tasas);
    return 0;
}

int calculate_gap()
{
    return  0;
}

int remove_feedback_arcs()
{
    int i,j,k,found_arc=0,arc_degree=-1;
    /* Iteramos sobre todos los arcos */
    for(i=0;i<g.narcs;i++){
        /* Buscamos la ubicación de la arista correspondiente al arco */
        for(j=0;j<g.degree[g.feedback_arc[i][0]];j++)
            if (g.edges[g.feedback_arc[i][0]][j] == g.feedback_arc[i][1]){
                found_arc=1;
                arc_degree=j;
                printf("Se encontro arco para nodo %d \n",g.feedback_arc[i][0] );
            }
        if(found_arc==0)
            printf("Error: No se encontraron arcos\n");
        /* Guardamos el consumo de la arista que forma el arco, en una variable aparte */
        g.feedback_arc_tipo_consumo[i] = g.tipo_consumo[g.feedback_arc[i][0]][arc_degree];
        g.feedback_arc_consumo[i] = g.consumo[g.feedback_arc[i][0]][arc_degree];
        /* Empujamos las demás aristas salientes, para tomar su lugar */
        for(k=arc_degree;k<g.degree[g.feedback_arc[i][0]]-1;k++){
            g.edges[g.feedback_arc[i][0]][k]=g.edges[g.feedback_arc[i][0]][k+1];
            g.consumo[g.feedback_arc[i][0]][k]=g.consumo[g.feedback_arc[i][0]][k+1];
            g.tipo_consumo[g.feedback_arc[i][0]][k]=g.consumo[g.feedback_arc[i][0]][k+1];
        }
        g.edges[g.feedback_arc[i][0]][g.degree[g.feedback_arc[i][0]]-1]=-1;
        g.consumo[g.feedback_arc[i][0]][g.degree[g.feedback_arc[i][0]]-1]=0.0;
        g.tipo_consumo[g.feedback_arc[i][0]][g.degree[g.feedback_arc[i][0]]-1]=-1;
        /* Disminuimos el grado del nodo */
        g.degree[g.feedback_arc[i][0]]--;
         /* Repetimos el proceso para el nodo destino */
        found_arc=0;
        arc_degree=-1;
        for(j=0;j<g.indegree[g.feedback_arc[i][1]];j++)
            if (g.back_edges[g.feedback_arc[i][1]][j] == g.feedback_arc[i][0]){
                found_arc=1;
                arc_degree=j;
            }
        if(found_arc==0)
            printf("Error: No se encontraron arcos\n");
        /* Empujamos las demás aristas salientes, para tomar su lugar */
        for(k=arc_degree;k<g.indegree[g.feedback_arc[i][1]]-1;k++){
            g.back_edges[g.feedback_arc[i][1]][k]=g.back_edges[g.feedback_arc[i][1]][k+1];
        }
        g.back_edges[g.feedback_arc[i][1]][g.indegree[g.feedback_arc[i][1]]-1]=-1;
        /* Disminuimos el grado del nodo */
        g.indegree[g.feedback_arc[i][1]]--;
        g.nedges--;
    }
    return 0;
}

int clear_free_nodes()
{
		int i;							/* contador */

		for (i=0; i<= MAXV; i++){
            if(n[i].congelar_tasas == 0){
			//Campos calculados por el programa
                n[i].costo_primario_fijo = 0.0;
                n[i].costo_primario_proporcional = 0.0;
                n[i].utilizacion = 0.0;
                n[i].costo_secundario_fijo = 0.0;
                n[i].costo_secundario_proporcional = 0.0;
                n[i].tasa_fija = 0.0;
                n[i].tasa_proporcional = 0.0;
                n[i].total_entradas = 0.0;
                n[i].total_salidas = 0.0;
                n[i].valor_nodo = 0.0;
            }
		}
    return 0;
}
