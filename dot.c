/* Springer Verlag Programming Challenges (2003)
   Funciones para inicializar, cargar grafo, nodos, vértices,
   e imprimir grafo
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>  // For stat()
#include <sys/stat.h>   // For stat()
#include <time.h>
#include <math.h>

#include "graph_def.h"
#include "graph_io.h"
#include "graph.h"
#include "dot.h"

int print_graph(char *filename,int mode)
{
		int i,j;						/* contadores */
        char pdf_filename[FILENAME_MAX];
        char png_filename[FILENAME_MAX];
        //printf("No. Nodos: %d No. Aristas: %d\n",g.nvertices,g.nedges);
        FILE *file = fopen( filename, "w" );
        //Imprimir nodos en formato DOT
        //fprintf(file,"digraph {\n rankdir=LR;\n graph [label=\"Empresa: %s Ano:%s Mes: %s Version: %s Usuario: %s Algoritmo: %s Fecha: %s\" labelloc=\"b\" labeljust=\"l\" fontsize=8];\n edge  [fontsize=10];\n node  [fontsize=12];\n", p.client, p.year,p.month,p.version,p.user,p.algoritmo,p.date);
        fprintf(file,"digraph {\n bgcolor=black rankdir=LR  ratio=fill ranksep = \"10 equally\" size = \"1100,1100\";\n graph [label=\"Empresa: %s Ano:%s Mes: %s Version: %s Usuario: %s Algoritmo: %s Fecha: %s Tolerancia Consumo: %lf\" labelloc=\"b\" labeljust=\"l\" fontsize=48 fontcolor=white];\n edge  [fontsize=10];\n node  [fontsize=12];\n", p.client, p.year,p.month,p.version,p.user,p.algoritmo,p.date,p.tolerancia_consumo);

        for (i=0; i < g.nvertices; i++)
            if(n[i].flag_valid){
                print_node_dot(file, i);
            }

		for (i=0; i < g.nvertices; i++){
            if(n[i].flag_valid){
                for (j=0; j < g.degree[i]; j++){
                    if(fabs(g.flujo[i][j])>0.0){
                        int color = set_color(fabs(g.flujo[i][j]));
                        print_edge_dot(file,i,j,mode,color);
                    }
                }
            }
		}

		for (i=0; i < g.ntypes; i++){
            fprintf(file, "{ rank=same");
            for (j=0;j< g.nvertices; j++){
                if(n[j].tipo_nodo == g.node_type[i]){
                        fprintf(file, ";%s",n[j].label);
                }
            }
            fprintf(file,"}\n");
		}


        fprintf(file, "}\n");
        fclose(file);

        strncpy(pdf_filename,filename,FILENAME_MAX);
        strncpy(&pdf_filename[strlen(filename)-3],"pdf",3);
        strncpy(png_filename,filename,FILENAME_MAX);
        strncpy(&png_filename[strlen(filename)-3],"png",3);
        //printf("File %s PDF File %s\n",filename,pdf_filename);

        if(p.pdf){
            dot_to_pdf (pdf_filename, filename);
        }
        if(p.png){
            dot_to_png (png_filename, filename);
        }
		return 0;
}

int set_color(double value)
{

    if(value < 100000.0){      //0,1MM
        return 1;
    }
    if(value < 1000000.0){      //1MM
        return 2;
    }
    else if(value < 10000000.0){     //10MM
        return 3;
    }
    else if(value < 50000000.0){     //50MM
        return 4;
    }
    else if(value < 100000000.0){    //100MM
        return 5;
    }
    else if(value < 250000000.0){    //250MM
        return 6;
    }
    else if(value < 500000000.0){    //500 MM
        return 7;
    }
    else
        return 8;

}

int print_node_dot(FILE *file, int x)
{
    double *in_out;
    int categoria_nodo=-1;

    in_out = calcular_entradas_salidas(x);

//definir categoria_nodo
    if( in_out[0] == 0.0 && in_out[1] == 0.0){   //Nodo no tiene valor, ignoremolo
        categoria_nodo=-1;
    } else if(g.indegree[x]==0){            // Nodo fuente
        categoria_nodo=0;
    } else if(g.degree[x]==0){              // Nodo destino
        categoria_nodo=1;
    } else {
        categoria_nodo=2;                   // Nodo intermedio
    }

    switch(categoria_nodo){
    case 0:
        fprintf(file, "\"%s\" [width=.1 shape=box color=purple fontcolor=purple fontsize=11 label = \"%s \\[Sale $%.2lf MM\\]\"];\n", n[x].label, n[x].label,(in_out[1]/1e6));
        break;
    case 1:
        fprintf(file, "\"%s\" [shape=box style=rounded color=red fontcolor=red label = \"%s \\[Entra $%.2lf MM\\]\"];\n", n[x].label, n[x].label,(in_out[0]/1e6));
        break;
    case 2:
        //if(fabs(n[x].utilizacion - n[x].capacidad)/n[x].capacidad > TOLERANCIA)
        //    print_node_item_dot(file, x, in_out, "Mrecord", "red");
        //else
            print_node_item_dot(file, x, in_out, "Mrecord", "white");
        break;
    default:
        break;
    }
    return 0;
}

double *calcular_entradas_salidas(int x)
{
    int i,start_node=0;
    double in=0.0,out=0.0;
    double *buf = malloc (sizeof (double) * 2);

    for(i=0;i<g.indegree[x];i++){
            start_node = g.back_edges[x][i];
            in+=g.flujo[start_node][get_node_degree(start_node,x)];
    }

    for(i=0;i<g.degree[x];i++)
            out+=g.flujo[x][i];

    *buf = in;
    *(buf+1) = out;

    return buf;

}

int print_node_item_dot(FILE *file, int x, double *in_out, char *shape, char *color)
{
    if(in_out[0]!=0.0 || in_out[1]!=0.0)
        fprintf(file, "\"%s\" [shape=%s fontsize=10 color=%s fontcolor=%s label = \"%s | {Tasa Fija=%.2lf | Tasa Prop=%.2lf} | {Cap=%.1lf | Util=%.1lf} | {Entra $%.2lf MM | Sale $%.2lf MM} | {Rel. Ent. %d | Rel. Sal. %d}\"];\n", n[x].label, shape, color, color, n[x].label, n[x].tasa_fija,n[x].tasa_proporcional,n[x].capacidad,n[x].utilizacion,(in_out[0]/1e6),(in_out[1]/1e6), g.indegree[x], g.degree[x]);
    return 0;
}

int print_edge_dot(FILE *file, int i, int j, int mode,int color)
{
    if(mode==1){                //Imprimir solo flujos monetarios
        fprintf(file,"%s -> %s[arrowhead=vee label=\"$%.2f\"];\n",n[i].label,n[g.edges[i][j]].label,g.flujo[i][j]);
    } else if (mode==2){        //Imprimir consumos y sus tipos
        switch (g.tipo_consumo[i][j]){
            case 1:
                if(edge_not_arc(i, g.edges[i][j]))
                    fprintf(file,"%s -> %s[colorscheme=greens8 color=%d ];\n",n[i].label,n[g.edges[i][j]].label,color);
                else
                    fprintf(file,"%s -> %s[color =red label=\"%.2fF\"];\n",n[i].label,n[g.edges[i][j]].label,g.consumo[i][j]);
                break;
            case 2:
                if(edge_not_arc(i, g.edges[i][j]))
                    fprintf(file,"%s -> %s[colorscheme=blues8 color=%d ];\n",n[i].label,n[g.edges[i][j]].label,color);
                else
                    fprintf(file,"%s -> %s[color =red label=\"%.2fP\"];\n",n[i].label,n[g.edges[i][j]].label,g.consumo[i][j]);
                break;
            default:
                fprintf(file,"%s -> %s[colorscheme=oranges8 color=%d arrowhead=vee ];\n",n[i].label,n[g.edges[i][j]].label,color);
        }
    }

    return 0;
}

int dot_to_pdf (char *pdf_filename, char *filename)
{
        char command[3*FILENAME_MAX];

        *command = 0;

//Imprimir pdf
        strcpy (command, "\"C:\\Program Files (x86)\\Graphviz2.34\\bin\\dot.exe\"");
        strcat (command, " -Tpdf -o ");
        strcat (command, pdf_filename);
        strcat (command, " ");
        strcat (command, filename);
        fprintf (flog,"Ejecutando %s\n", command);
        system (command);

        //TODO Handle error from system command

/*
        if(remove(filename) == 0){
            printf("Grabando: %s\n",pdf_filename);
            fprintf(flog,"Archivo %s  borrado.\n", filename);
            return 0;
        } else {
            printf("Error borrando archivo %s\n", filename);
            return 1;
        }
*/
        return 0;
}

int dot_to_png (char *png_filename, char *filename)
{
        char command[3*FILENAME_MAX];

        *command = 0;

//Imprimir png
        strcpy (command, "\"C:\\Program Files (x86)\\Graphviz2.34\\bin\\sfdp.exe\"");
        strcat (command, " -Gsize=67! -Goverlap=prism -Tpng -o ");
        strcat (command, png_filename);
        strcat (command, " ");
        strcat (command, filename);
        fprintf (flog,"Ejecutando %s\n", command);
        system (command);

        return 0;
}

int print_all_paths(char *dirname, int origin)
{
    int i,j;
    char filename[FILENAME_MAX], pdf_filename[FILENAME_MAX], node_dir[FILENAME_MAX];

    //Creamos carpeta para todos los archivos con grafos de caminos individuales
    goto_dir(dirname);

    for(i=0;i<g.npaths;i++){        //Recorremos todos los caminos
        if(g.path_type[i]==1){              // Camino completo
            *filename = 0;
            *node_dir = 0;
            *pdf_filename = 0;
            concatenate_path_string(node_dir,filename,i,origin, pdf_filename);
            goto_dir(node_dir);

            FILE *file = fopen( filename, "w" );
            fprintf(file,"digraph {\n rankdir=LR;\n graph [label=\"Empresa: %s Ano:%s Mes: %s Version: %s Usuario: %s Algoritmo: %s Fecha: %s\" labelloc=\"b\" labeljust=\"l\" fontsize=8];\n edge  [fontsize=10];\n node  [fontsize=12];\n", p.client, p.year,p.month,p.version,p.user,p.algoritmo,p.date);
            for(j=0;j < g.path_length[i]; j++)
                print_node_dot(file, g.path[i][j]);
            for(j=1;j < g.path_length[i]; j++)
                print_edge_dot(file,g.path[i][j-1],get_node_degree(g.path[i][j-1],g.path[i][j]),1,1);
            fprintf(file,"}\n");
            fclose(file);
            dot_to_pdf (pdf_filename, filename);
            chdir("..");
        }
    }
    chdir("..");

    return 0;
}

