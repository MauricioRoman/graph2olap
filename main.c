/* Main.c
   Author: Mauricio Roman
   Version: 1.2
   Release: Jan 31, 2014
   Comments: Transforms a DAG into an OLAP cube for reporting
             Figures out if graph is not DAG, and shows cycles by implementing algorithm by Eades
             (This is so cycles can be removed from model)
             Prints out graph in PDF, as well as any cycles found
             TODO -- Solve case for model with reciprocity (not DAG)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <windows.h>
#include <time.h>

#include "graph_def.h"
#include "graph.h"
#include "graph_io.h"
#include "path.h"
#include "dot.h"


int main ( int argc, char *argv[] )
{
    char in_filename[4][FILENAME_MAX];
    char cp_in_filename[FILENAME_MAX];

    char out_filename1[FILENAME_MAX];
    char out_filename2[FILENAME_MAX];
    char out_filename3[FILENAME_MAX];
    char out_filename4[FILENAME_MAX];
    char out_filename5[FILENAME_MAX];
    char out_filename6[FILENAME_MAX];
    char out_filename7[FILENAME_MAX];

    char dirname1[FILENAME_MAX];
    char dirname2[FILENAME_MAX];
    char dirname3[FILENAME_MAX];
    char dirname4[FILENAME_MAX];

    int i=0, j=0;
    int sorted[MAXV];          /* List of vertices topologically ordered */
    int not_DAG;               /* Flag to detect DAG = Directed Acyclic Graph */


    char *s;

    //Set comma as decimal separator, as this is customary in Colombia
    set_comma(1);

    if ( argc != 27 ) {
        printf( "Modo de Uso:\n %s -c [siglas_cliente (max 4)] -a [ano] -m [mes] -v [version] -u [usuario] -alg [algoritmo: SEQ|REC] -pdf [S|N] -png [S|N] -cam [caminos en archivo:S|N] -cubo [cubo con camnos: S|N] -tol [tolerancia consumo] -out [generar archivos entrada: S|N] -val_min [valor minimo para relacion, en moneda]", argv[0] );
        exit(EXIT_FAILURE);
    }

    else {

         printf("\n\n");
         strncpy(p.client,argv[2],4+1);
         strncpy(p.year,argv[4],4+1);
         strncpy(p.month,argv[6],2+1);
         strncpy(p.version,argv[8],4+1);
         strncpy(p.user,argv[10],128);
         strncpy(p.algoritmo,argv[12],3+1);
         p.date=get_time();                // Save date as a parameter, useful when printing PDFs
         //Initializing variables...
         p.pdf=0;
         p.png=0;
         p.path_cube=0;
         p.path_file=0;
         p.out=0;

         if(!strncmp("S",argv[14],2))
            p.pdf=1;
         if(!strncmp("S",argv[16],2))
            p.png=1;
         if(!strncmp("S",argv[18],2))
            p.path_file=1;
         if(!strncmp("S",argv[20],2))
            p.path_cube=1;

         /* Read minimum threshold for input edge values (throw away data below this #) */
         p.tolerancia_consumo = strtof(argv[22],NULL);
         fprintf(flog,"Tolerancia consumo = %.2f\n", p.tolerancia_consumo);

         if(!strncmp("S",argv[24],2)){
            p.out=1;
         }
         /* Read minimum threshold for edge $ values (throw away data below this #) */
         p.val_min = strtof(argv[26],NULL);
         fprintf(flog,"Valor minimo para relación = %.2f\n", p.val_min);

         /* Obtain time stamp */
         time_t timer;
         struct tm* tm_info;

         time(&timer);
         tm_info = localtime(&timer);
         strftime(p.timestamp, 26, "%Y_%m_%d_%Hh%Mm", tm_info);

         /* Input Files */
         snprintf(in_filename[0], FILENAME_MAX, "%s_elementosDim_%s_%s_%s.csv", p.client,p.year, p.month, p.version);
         snprintf(in_filename[1], FILENAME_MAX, "%s_PrimariosValidados_%s_%s_%s.csv", p.client,p.year, p.month, p.version);
         snprintf(in_filename[2], FILENAME_MAX, "%s_ConsumosValidados_%s_%s_%s.csv", p.client,p.year, p.month, p.version);
         snprintf(in_filename[3], FILENAME_MAX, "%s_ReceptoresTipo_%s_%s_%s.csv", p.client,p.year, p.month, p.version);

         /* Output Files */
         snprintf(out_filename1, sizeof out_filename1, "%s_Grafo_%s_%s_%s_COP.dot",p.client, p.year, p.month, p.version);
         snprintf(out_filename2, sizeof out_filename2, "%s_Grafo_%s_%s_%s_Consumo.dot",p.client, p.year, p.month, p.version);
         snprintf(out_filename3, sizeof out_filename3, "%s_Tasas_%s_%s_%s.csv",p.client, p.year, p.month, p.version);
         snprintf(out_filename4, sizeof out_filename4, "%s_Secundarios_%s_%s_%s.csv",p.client, p.year, p.month, p.version);
         snprintf(out_filename5, sizeof out_filename5, "%s_CuboCaminos_%s_%s_%s.csv",p.client, p.year, p.month, p.version);
         snprintf(out_filename6, sizeof out_filename6, "%s_LOG_%s_%s_%s.txt",p.client, p.year, p.month, p.version);
         snprintf(out_filename7, sizeof out_filename7, "%s_ListaCaminos_%s_%s_%s.csv",p.client, p.year, p.month, p.version);

         /* Output Directories */
         snprintf(dirname1, sizeof dirname1, "Resultados_%s_%s_%s_%s_(%s)",p.client, p.year, p.month, p.version,p.timestamp);
         snprintf(dirname2, sizeof dirname2, "%s_Caminos_Inicio_%s_%s_%s",p.client, p.year, p.month, p.version);
         snprintf(dirname3, sizeof dirname3, "%s_Caminos_Fin_%s_%s_%s",p.client, p.year, p.month, p.version);
         snprintf(dirname4, sizeof dirname4, "%s_Ciclos_%s_%s_%s",p.client, p.year, p.month, p.version);

        /* Open Log File */
         printf("Log file %s\n",out_filename6);
         flog = fopen( out_filename6, "w" );
         fprintf(flog, "%s Iniciando corrida\n", p.timestamp);

         /* Load all vertices */
         initialize_graph();
         initialize_nodes();
         load_nodes(in_filename[0]);
         /* Load all edges */
         load_graph(in_filename[1], in_filename[2]);
         print_node_types();
         /* Mark all connected nodes so we can trim model */
         trim_nodes();

         int not_valid=0;
         if(g.nvalid != g.nvertices){
            printf("Hay %d nodos, pero solamente %d estan conectados\n", g.nvertices,g.nvalid);
            printf("Revise los archivos de entrada\n");
            if(p.out){
              printf("Vamos a generar archivos de RECARGA...por favor vuelva a correr usandolos\n");
              not_valid=0;
            } else {
                exit(EXIT_FAILURE);
            }
         }

         /* Create and goto directory for output results - one directory per run - */
         goto_dir(dirname1);
         /* Create and goto directory for copy of input files */
         goto_dir("CopiaArchivosEntrada");
         /* Copy input files -- for version control */
         for (i=0; i < 4; i++){
            *cp_in_filename = 0;
            strncat(cp_in_filename,"..\\..\\",FILENAME_MAX);
            strncat(cp_in_filename,in_filename[i],FILENAME_MAX);
            CopyFile(cp_in_filename,in_filename[i],1);
         }
         /* Go back to directory for output results */
         chdir("..");

         /*  This algorithm applies to a model with a DAG */
          if(!strcmp(p.algoritmo,"SEQ")){
                /* Sort vertices topologically */
                not_DAG = toposort(sorted);

                for(i=0;i<not_DAG;i++){
                    fprintf(flog,"Sorted %d = %d\n",i,sorted[i]);
                }
                fprintf(flog,"resultado toposort=%d, g.nvalid=%d, g.nvertices=%d\n", not_DAG, g.nvalid, g.nvertices);

                if(not_DAG) {       /* Cycles were found */
                    /* Figure out which vertices and edges create a cycle */
                    feedback_arc_set_eades();
                    if(g.narcs){
                        printf("\nEl algoritmo secuencial tiene reciprocidad (ciclos)\n");
                        printf("Se detectaron %d ciclos\n",g.narcs);
                        printf("Revise el directorio pdf\\%s para verlos\n",dirname4);
                        goto_dir("pdf");
                        /* Print PDF of graph */
                        print_graph(out_filename2,2);
                        /* Print PDF of cycles found */
                        print_feedback_arc(dirname4);
                        chdir("..");
                    } else {
                        for (i=0;i<not_DAG;i++){
                            for(j=0;j<g.nvertices;j++){
                                if(sorted[i]==j){
                                    n[j].flag_sorted = 1;
                                }
                            }
                        }
                        printf("No todos los nodos estan conectados entre si. Revise:\n");
                        for(j=0;j<g.nvertices;j++){
                            if(!n[j].flag_sorted && n[j].flag_valid)
                                printf("%s\n",n[j].label);
                        }
                    }

                } else {
                    /* Main calculations for DAG */

                    printf("Calculando tasas y costos secundarios\n");
                    /* 1. Calculate $ rates at each vertex, as well as secondary costs */
                    for (i=0; i < g.nvertices; i++){
                        int valid_vertex = (n[sorted[i]].congelar_tasas == 0)
                           && (n[sorted[i]].flag_valid == 1)
                           && (g.degree[sorted[i]]>0);
                        if(valid_vertex){
                            fprintf(flog,"Procesando nodo %s\n",n[sorted[i]].label);
                            process_vertex(sorted[i]);
                        }
                        output_node_detail(sorted[i]);
                    }
                    printf("Listo!\n");

                    /* 2. Find out all unique paths */
                    if(p.path_file){
                        printf("Calculando los diversos caminos\n");
                        find_paths(sorted, 1);
                        find_paths(sorted, 2);
                        printf("Listo!\n");
                    }

                    /* Print summary of paths found and accumulated values */
                    print_graph_summary();

                    /* Create an OLAP cube with each dimension as a level in the path */
                    int *path_cube_dir;
                    if (p.path_cube){
                        printf("Creando cubo con caminos\n");
                        path_cube_dir = create_path_cube();
                        printf("Listo!\n");
                    }
                    /* Output */

                    /* Save files that can be used to upload information to OLAP (in our case, Jedox OLAP)*/
                    printf("Almacenando tasas y costos secundarios\n");
                    goto_dir("CargaJedox");
                    save_graph(out_filename3, out_filename4);
                    if(p.path_file){
                        save_path_flow_file(out_filename7);
                    }
                    chdir("..");
                    printf("Listo!\n");
                    /* Save $ flows according to their dimensions/levels */
                    if (p.path_cube){
                        goto_dir("CargaJedox");
                        printf("Almacenando datos de cubo con caminos\n");
                        save_path_flow(out_filename5,path_cube_dir);
                        free(path_cube_dir);
                        printf("Listo!\n");
                        chdir("..");
                    }

                    /* Print total graph as a PDF file
                    (last and optional step, as it takes a while...) */
                    if (p.pdf || p.png){
                        printf("Imprimiendo archivos PDF y/o PNG\n");
                        goto_dir("PDF");
                        //print_graph(out_filename1,1);  //Mostrando flujos de dinero
                        print_graph(out_filename2,2);  //Mostrando consumos
                        /* Grabar todos los caminos en PDF */
                        //Estos son sumamente demorados, se omiten...
                        //print_all_paths(dirname2,0);  //Organiza grafos por carpetas desde el inicio
                        //print_all_paths(dirname3,1);  //Organiza grafos por carpetas desde el fin
                        chdir("..");
                        printf("Listo!\n");
                    }

                    /* This functionality will summarize the input files into a shorter version
                       that eliminates redundancy...it is in beta */
                    if(p.out){
                        printf("Imprimiendo archivos de RECARGA - consolidados y filtrados\n");
                        goto_dir("RecargaArchivosEntrada");
                        print_recarga(in_filename[0],in_filename[2]);
                         chdir("..");
                        printf("Listo!\n");
                    }


                    printf("Proceso con algoritmo secuencial terminado exitosamente.\n");
                }

          /* This solves for the case when the cost model is not a DAG, but has cycles...
             it has not been developed, but is just the skeleton for future code */
          } else {  //Algoritmo recíproco
                not_DAG = toposort(sorted);
                if(not_DAG) {

                   char filename[FILENAME_MAX];
                    *filename = 0;

                    //int gap=2*TOLERANCIA, j=0;
                    feedback_arc_set_eades();
                    strcpy(filename, "1_");
                    strcat(filename,out_filename2);
                    print_graph(filename,2);  //Mostrando consumos
                    remove_feedback_arcs();
                    strcpy(filename, "2_");
                    strcat(filename,out_filename2);
                    print_graph(filename,2);  //Mostrando consumos
                    not_DAG = toposort(sorted);
                    if(not_DAG) {
                            printf("Problemas encontrados en algoritmo reciproco\n");
                            exit(EXIT_FAILURE);
                    } else {
                        for (i=0; i < g.nvertices; i++)
                            process_vertex(sorted[i]);
                        strcpy(filename, "3_");
                        strcat(filename,out_filename2);
                        print_graph(filename,2);  //Mostrando consumos
                        strcpy(filename, "3_");
                        strcat(filename,out_filename1);
                        print_graph(filename,1);  //Mostrando dinero

                        for (i=0; i < g.narcs; i++)     // Procesar las aristas que cierran ciclos
                            process_vertex_arc(i);
                        strcpy(filename, "4_");
                        strcat(filename,out_filename2);
                        print_graph(filename,2);  //Mostrando consumos
                        strcpy(filename, "4_");
                        strcat(filename,out_filename1);
                        print_graph(filename,1);  //Mostrando dinero

                        clear_free_nodes(); //Limpia todos los nodos, excepto los congelados

                        for (i=0; i < g.nvertices; i++)
                            process_vertex(sorted[i]);
                        strcpy(filename, "5_");
                        strcat(filename,out_filename2);
                        print_graph(filename,2);  //Mostrando consumos
                        strcpy(filename, "5_");
                        strcat(filename,out_filename1);
                        print_graph(filename,1);  //Mostrando dinero
                    }
/*
                    while (gap > TOLERANCIA && j < 2){
                        for (i=0; i < g.narcs; i++){     // Procesar las aristas que cierran ciclos
                            process_vertex_arc(g.feedback_arc[i][0],g.feedback_arc[i][1]); //se necesita nueva func???
                            bfs(g.feedback_arc[i][1],1);
                            calculate_gap();
                            sprintf(filename, "%d_",i+2);
                            strcat(filename,out_filename1);
                            print_graph(filename,1);  //Mostrando flujos de dinero
                            j++;
                        }
                    }*/
                    printf("Proceso con algoritmo reciproco terminado exitosamente.\n");
                } else {
                    printf("El algoritmo con reciprocidad no encontro ciclos...use el algoritmo secuencial (SEQ)\n");
                }
          }
    /* Regresar al directorio original */
    chdir("..");
    fclose(flog);
    return 0;
    }
}
