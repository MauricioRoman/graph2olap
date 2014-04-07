#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "graph_def.h"
#include "graph_io.h"
#include "graph.h"
#include "stack.h"
#include "queue.h"
#include "path.h"
#include "math.h"

/* Esta rutina itera sobre todos los nodos raíz para
 * luego navegar el grafo usando DFS. Guarda los nodos
 * en una pila (s1), y va identificando caminos, sacando nodos
 * de s1, los cuales guarda en una segunda pila (s2). Si al colocar
 * un nodo al final de s2, la última relación no es un vértice existente,
 * entonces reduce el tamaño de s2 hasta encontrar un camino válido.
 * Aún así, el camino puede ya haber sido identificado
*/
int find_paths(int sorted[], int order)
{
    int i=0, k=0;
    int actnode=-1;
    stack s1,s2, stemp;
    init_stack(&s1);
    init_stack(&s2);
    init_stack(&stemp);
    //Iteramos sobre todos los nodos fuente a lo largo de un arreglo
    //de nodos ordenados topológicamente
    while(g.indegree[sorted[i]] == 0 && i<g.nvertices && sorted[i]>=0){
        int root=sorted[i];
        int ignore=-1, new_nodes=1, path_complete;
        fprintf(flog,"Buscando caminos para nodo raiz %i - %d=%s\n", i, root, n[root].label);

        clear_stack(&s1);
        clear_stack(&s2);
        clear_stack(&stemp);

        push(&s1, root);
        // Hay por lo menos una arista viva desde el origen
        while(!empty_stack(&s1)){
            actnode = pop(&s1);
             fprintf(flog,"1. actnode = %d\n",actnode);
            push(&s2,actnode);
             fprintf(flog,"2. Sacando nodo %d del stack\n",actnode);
            /* En la última iteración, no se agregaron nuevos nodos a la pila*/
            int path_condition = 0;
            if(new_nodes == 0){
                stack_copy(&stemp, &s2); // Copiamos
                // Condicion = 1, exitosa, o =2, camino agotado
                path_condition = shorten_path(&s2);
                if(path_condition == 2){
                    stack_copy(&s2, &stemp);// Regresamos a la original
                    int tmp = pop(&s2);  //Eliminamos el último nodo
                     fprintf(flog,"stemp: "); print_stack(&stemp);
                     fprintf(flog,"s2: "); print_stack(&s2);
                } else { // Cambios para recordar caminos "raiz" ya recorridos
                    path_complete = is_path_complete(&s2);
                    if(path_complete == 0){
                        add_path(&s2, 0);   //Adicionamos el camino como incompleto
                    }
                }
            }
            //Bajo una condición de 2, abortamos
            //De otra forma, buscamos cerrar o extender el camino
            //Actnode es el último nodo del camino
            if (path_condition != 2)
            {
                //Si el último nodo tiene grado 0, es terminal, no habrá más nodos
                new_nodes=g.degree[actnode];
                 fprintf(flog,"Grado (out) de Actnode %d: %d\n",actnode, new_nodes);
                if(!new_nodes){
                    if(path_condition == 0){
                        //Revisamos si el camino es único, pues no se ha hecho antes
                        if(unique_path(&s2)){
                            add_path(&s2, 1);   //Movemos s2 al último camino
                        } else {
                             fprintf(flog, "Camino no es único\n");
                             fprintf(flog,"s2: "); print_stack(&s2);
                        }
                    } else {  //Ya revisamos si el camino era unico, no hay que hacerlo 2 veces
                            add_path(&s2, 1);   //Adicionamos el camino como terminal
                    }
                    ignore=pop(&s2); //Removemos el último miembro de s2
                                       //n++;
                } else {
                    for(k=0;k<new_nodes;k++){
                        int k_index;
                        //Posibilitamos explorar nodos en 2 ordenes diferentes
                        if(order == 1)
                            k_index = k;
                        else if (order == 2)
                            k_index = new_nodes - k - 1;

                        if (g.edges[actnode][k_index]>=0){
                            push(&s1,g.edges[actnode][k_index]);
                        } else {
                            fprintf(flog,"Error: Revisar modelo, pues nodos salientes de nodo %d (%s) no son validos!\n",actnode,n[actnode].label);
                            fprintf(flog,"k=%d, new_nodes = %d\n",k_index,new_nodes);
                            printf("Error: Revisar modelo, pues nodos salientes de nodo %d (%s) no son validos!\n",actnode,n[actnode].label);
                            output_node_detail(actnode);
                        }
                    }
                 fprintf(flog,"s2: "); print_stack(&s2);
                 fprintf(flog,"s1: "); print_stack(&s1);
                }
            }
        } // while not empty s1
        i++;
    } // while exist root nodes

    return 0;
}

/* Esta rutina reduce el tamaño del camino (pila s2) hasta que encuentra
   un camino válido. Si el camino no es único, continúa reduciendo la pila.
   Si llega a un camino mínimo, para y devuelve el status. Se espera que
   la rutina que lo llame, guarde el camino original, y si el status es 2,
   descarte el camino analizado y siga con el original.
*/
int shorten_path(stack *s2)
{
    int short_path = 1, status;

    while(short_path){
        if (valid_path_edge(s2) == -1){
             fprintf(flog,"Arista NO es valida\n");
            /* Revisamos que no eliminemos el nodo raiz */
            if(stack_size(s2) > 2){
                 fprintf(flog,"Reduciendo el tamaño del camino...\n");
                reduce_path(s2);
            } else {
                 fprintf(flog,"La pila es demasiado pequeña, no se puede correr mas\n");
                short_path = 0;
                status = 2;
            }
        } else {
            /* Arista válida...revisamos que el camino sea único */
            if(unique_path(s2)){
                 fprintf(flog,"Arista SI es valida y el camino es unico\n");
                short_path = 0;
                status = 1;
            } else {
                 fprintf(flog,"Arista SI es valida pero el camino NO es unico\n");
                 fprintf(flog,"Reduciendo el tamaño del camino...\n");
                reduce_path(s2);
            }
        }
    }

    return status;
}

/* Esta rutina quita el penúltimo nodo y corre el último nodo a su lugar
*/
int reduce_path(stack *s2){

    int actnode, ignore;
     fprintf(flog,"Corriendo el ultimo nodo\n");
    actnode=pop(s2); //Sacamos el nodo que acabamos de meter
     fprintf(flog,"2. actnode = %d\n",actnode);
    ignore=pop(s2); //Sacamos el nodo anterior (ignorado)
    push(s2,actnode); //Metemos el nodo en dicha posicion
     fprintf(flog,"Pila despues de correrse:\n");
    print_stack(s2);
}

/* Esta rutina revisa si el vértice conformado por los dos últimos nodos
   de la pila s2 es un vértice válido
*/
int valid_path_edge(stack *s2)
{
    int i;
    int valid_edge;
     fprintf(flog,"Validando enlace entre %d y %d\n",s2->s[s2->top-1],s2->s[s2->top]);
    valid_edge=get_node_degree(s2->s[s2->top-1],s2->s[s2->top]);
     fprintf(flog,"1. Valid edge=%d\n",valid_edge);
    return valid_edge;   //Retorna -1 si no es válida, o el indice en el grado
}

/* Esta rutina calcula un índice para el camino seleccionado, que consiste en
   la suma de los indices de todos sus nodos */

int compute_path_index(stack *s2)
{
    int i, path_index=0;
    path_index += s2->s[0];
    for(i=1;i<s2->count;i++){
        path_index += (int) pow(s2->s[i], (i+1)/2);
    }
    return path_index;
}

/* Esta rutina verifica si el camino albergado en s2 es un camino único,
   es decir, que ya no ha sido identificado previamente
*/
int unique_path(stack *s2)
{
    int i,j,path_index, path_complete, unique=1;
     fprintf(flog,"Revisando si la siguiente pila representa un camino unico\n");
     print_stack(s2);
    path_index = compute_path_index(s2);
    //Averiguar si el camino está completo, o si puede ser extendido
    path_complete = is_path_complete(s2);
    fprintf(flog, "Es un camino completo? %d\n", path_complete);

    for(j=0;j<g.npaths;j++){     //Iteramos sobre todos los caminos
        //fprintf(flog,"Camino %d Longitud %d Tamano Pila %d\n",j,g.path_length[j],s2->count);
        /* Comparamos miembro por miembro cuando las longitudes coinciden */
        if(g.path_index[j]==path_index){
             fprintf(flog,"Se encontro un camino (%d) con igual indice (%d)\n",j, path_index);
            int same=0;
            if(g.path_length[j]==s2->count){
                 fprintf(flog,"Dicho camino posee (%d) igual longitud\n",j);

                int same=0;
                for(i=0;i<s2->count;i++){
                     fprintf(flog, "Comparando %d con %d\n",s2->s[i],g.path[j][i]);
                    if(s2->s[i] == g.path[j][i])
                        same++;
                }
                if(same == g.path_length[j]){ //Encontramos un camino identico
                     fprintf(flog,"Se encontro un camino (%d) identico\n",j);
                    unique = 0;
                    return unique;
                }
            }
        }
    }
    return unique;
}

int is_path_complete(stack *s2)
{
    int path_size, last_node, last_node_degree;

    path_size = s2->count;
    if(path_size > 1){
        last_node = s2->s[path_size - 1];

        last_node_degree = g.degree[last_node];

        if(last_node_degree == 0)
            return 1;
        else
            return 0;
    }
    return -1;
}

/* Esta rutina agrega un camino nuevo que ha sido encontrado, a la estructura
   con los datos de los caminos.
*/
int add_path(stack *s2, int type)
{
    int i, path_number=g.npaths;
    int path_length=s2->count;

    if(path_number > MAXPATH){
        printf("Error: El numero de caminos (%d) excede el tamano de la memoria asignada (%d)\n",
               path_number,MAXPATH);
        exit(EXIT_FAILURE);
    }
    if(path_length>MAXPATHLEVEL){
        printf("Error: El numero de niveles (%d) excede el tamano de la memoria asignada (%d)\n",
               path_length,MAXPATHLEVEL);
        exit(EXIT_FAILURE);
    }

    fprintf(flog,"Agregando camino no: %d\n",path_number);
    //fprintf(flog,"Ntypes=%d\n",g.ntypes);

    //fprintf(flog,"Tamano de la pila (camino) %d\n",path_length);
    //fprintf(flog,"Se agrego el siguiente camino:\n");
    //print_stack(s2);
    for(i=0;i<path_length;i++)
        g.path[path_number][i]=s2->s[i];
    g.path_length[g.npaths] = path_length;
    g.path_type[g.npaths] = type;       //MRR - Podemos guardar 2 tipos de camino
    if(type==1){   //Solo nos interesa guardar el valor para los caminos completos
        g.path_value[g.npaths]=compute_path_value(g.npaths);
    } else {
        g.path_value[g.npaths]=0;
    }
    g.path_index[g.npaths]=compute_path_index(s2);
    g.npaths++;

    return 0;
}

/* Esta rutina calcula el valor total del flujo de dinero a través del camino
*/
double compute_path_value(int p){

    double value;
    int i=0;

    value = get_flujo(g.path[p][0],g.path[p][1],0);
     fprintf(flog,"Valor Camino tipo %d - Nodos %d,%d, flujo=%.2lf valor=%.2lf\n",g.path_type[p],g.path[p][0],g.path[p][1],get_flujo(g.path[p][0],g.path[p][1],0),value);

    for (i=1;i<g.path_length[p]-1;i++){
        value *= get_flujo(g.path[p][i],g.path[p][i+1],1);
         fprintf(flog,"Nodos %d,%d, flujo=%.6lf valor=%.2lf\n",g.path[p][i],g.path[p][i+1],get_flujo(g.path[p][i],g.path[p][i+1],1),value);
    }
    return value;
}
/* Esta rutina obtiene el flujo en una arista, puede ser en dinero o en consumo
*/

double get_flujo(int start_node, int end_node, int type){
    int j;

    for (j=0; j<g.degree[start_node]; j++)
        if(g.edges[start_node][j]==end_node){
            if(type==0)
                return g.flujo[start_node][j];          //Relación inicial: flujo en dinero
            else if (type==1)
                return g.flujo_porc[start_node][j];     //Relaciones posteriores: flujo porcentual
        }
    return -1.0;
}

bool consecutive_types()
{
    int i;
        /* El primer tipo es 0, que se reserva para los nodos fuente
        Los nodos que han sido cargados de archivo deben tener tipo
        desde 0 hasta g.ntypes. Si lo exceden, quiere decir que no son
        consecutivos */
    for(i=0;i<g.ntypes;i++)
        if(g.node_type[i]>g.ntypes || g.node_type[i]<0)
            return FALSE;

    return TRUE;
}

 int *create_path_cube(){
     int i,j;

    /* Esta función (i) valida la estructura de los niveles (tipos) y
    crea un cubo para albergarlos. Debe ser activada por un argumento
    del programa pues es opcional */

    fprintf(flog,"Iniciando Create Path Cube()\n");
    for(i=0;i<g.ntypes;i++)
        fprintf(flog,"%d \n",g.node_type[i]);

    if(g.ntypes == 0){
        printf("Error: No hay tipos de nodos almacenados\n");
        exit(EXIT_FAILURE);
    }
    //Imprimimos los tipos de nodos, y los nodos con sus tipos, a LOG
    for (i=0;i<g.ntypes;i++){
        fprintf(flog,"Tipo_id %d = %d\n",i, g.node_type[i]);
    }
    print_node_types();

    /* Revisamos los tipos existentes, que sean consecutivos */
    if (!consecutive_types()){
        printf("Error: Los tipos de nodos no son consecutivos\n");
        exit(EXIT_FAILURE);
    }

    /* Revisamos que todos los nodos tengan un tipo */
    for(i=0;i<g.nvertices;i++){
        if(n[i].tipo_nodo < 0){
            printf("Error: El nodo %s no tiene tipo definido (grado %d)\n",n[i].label,g.indegree[i]);
            exit(EXIT_FAILURE);
        }
    }

    /* Revisamos que (i) el primer elemento del camino = primer tipo
       (ii) el último elemento del camino = último tipo (=no. tipos)
       (iii) que los demás nodos del camino tengan tipos diferentes, y
       (iv) que estos se incrementen a lo largo del camino */
    print_node_types();

    //Validamos niveles pero solo para caminos completos y validos
    for(i=0;i<g.npaths;i++){
        if(g.path_type[i]==1){              // Camino completo
            if(fabs(g.path_value[i])>0.0){
                int x=g.path[i][0];
                //printf("Nx.tipo nodo = %d\n",n[x].tipo_nodo);
                if(n[x].tipo_nodo !=0){
                    printf("Error: El nodo %s (%d), que inicia un camino, tiene tipo incompatible (%d)\n",n[x].label,x,n[x].tipo_nodo);
                    exit(EXIT_FAILURE);
                }
                //fprintf(flog,"Longitud de camino %d es %d\n",i,g.path_length[i]);
                x=g.path[i][g.path_length[i]-1];
                //fprintf(flog,"Nodo final de este camino es %d\n",g.path[i][g.path_length[i]-1]);
                if(n[x].tipo_nodo !=g.ntypes-1){
                    printf("Error: El nodo %s (%d), que finaliza un camino, tiene tipo incompatible (%d), deberia ser %d\n",n[x].label,x,n[x].tipo_nodo,g.ntypes);
                    exit(EXIT_FAILURE);
                }
                for(j=1;j<g.path_length[i]-1;j++){
                    x=g.path[i][j];
                    int x_prev = g.path[i][j-1];
                    if(n[x].tipo_nodo <= n[x_prev].tipo_nodo){
                        printf("Error: los tipos de nodos en el camino %d no son incrementales:\n",i);
                        printf("Nodo:%s Nodo Anterior: %s\n",n[x].label,n[x_prev].label);
                        printf("Tipo Nodo:%d, Tipo Nodo Anterior:%d\n",n[x].tipo_nodo,n[x_prev].tipo_nodo);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    /* Si no pasa la revisión, mandamos un mensaje de error y paramos */

    /* Creamos una tabla con tamaño g.npaths x (ntipos+1)
       Usamos un tipo adicional para acomodar nodos fuente, cuyo tipo
       es 0 */
    //int table_size = g.npaths * (g.ntypes+1);
    int table_size = g.npaths * g.ntypes;
    fprintf(flog,"npaths=%d, ntypes+1=%d, malloc table size = %d\n",g.npaths, g.ntypes+1,table_size);
    int *path_cube = malloc (table_size * sizeof(int));
    for (i=0;i<table_size;i++)
        *(path_cube+i)=-1;
    fprintf(flog,"path cube %d +1 %d\n", path_cube, path_cube+1);
    /* Asignamos cada elemento de cada camino a una celda de esa tabla */

    for (i=0;i<g.npaths;i++){
        for (j=0;j<g.path_length[i];j++){
            fprintf(flog,"(%d,%d) Direccion path_cube %d, inc=%d\n",i,j,path_cube, path_cube+i*(g.ntypes+1)+n[g.path[i][j]].tipo_nodo);
 //           *(path_cube+i*(g.ntypes+1)+n[g.path[i][j]].tipo_nodo) = g.path[i][j];
            *(path_cube+i*g.ntypes+n[g.path[i][j]].tipo_nodo) = g.path[i][j];
        }
    }

    /* Devolvemos direccion de cubo */
    fprintf(flog,"Finalizando Create Path Cube()\n");
    return path_cube;
 }
