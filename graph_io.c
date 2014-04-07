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


int print_recarga(char *filename1,char *filename2)
{
	/* filename1 = apuntador a archivo para recarga de nodos
 	* filename2 = apuntador a archivo para recarga de secundarios
 	*/
    int i=0, j=0;
    FILE *file1 = fopen( filename1, "w" );
    FILE *file2 = fopen( filename2, "w" );
    int omitir_nodos=0;
	/* 1. Imprimir archivo de relaciones secundarias */

	/* Iterar por todos los nodos */
	fprintf(file2,"Sender;Receiver;Q001;Q002\n");
	for(i=0;i<g.nvertices;i++){
	/* Imprimir sólo las relaciones cuyo valor absoluto es mayor al indicado */
		omitir_nodos=0;
		double suma_consumo=0.0;
		/* Revisamos si alguna relación está por debajo del límite mínimo */
		for(j=0;j<g.degree[i];j++){
			if (g.tipo_consumo[i][j] >= 1) {
				if (fabs(g.flujo[i][j]) > p.val_min){
					suma_consumo += g.consumo[i][j];
				} else {
					omitir_nodos++;
				}
			}
		}
		/* Volvemos a repasar las relaciones y recalculamos los consumos */
		for(j=0;j<g.degree[i];j++){
            if (g.tipo_consumo[i][j] >= 1){
                if (fabs(g.flujo[i][j]) > p.val_min)  {
                    fprintf(file2,"%s;%s;",
                        n[i].label,n[g.edges[i][j]].label);

                    /* Recalcular consumo */
                    double consumo_nuevo = g.consumo[i][j] * n[i].capacidad / suma_consumo;
                    if(g.tipo_consumo[i][j] == 1){
                        fprintf(file2,"%.6lf;0,0\n",consumo_nuevo);
                    } else if (g.tipo_consumo[i][j] == 2){
                        fprintf(file2,"0,0;%.6lf\n",consumo_nuevo);
                    }
                } else {
                    printf("Omitiendo relación -  %d->%d, flujo=%.2lf",i,j,g.flujo[i][j]);
                }
	    	}
		}
	}
	fclose(file2);

		/* 1. Imprimir archivo con nodos */

	/* Iterar por todos los nodos */
	fprintf(file1,"ID_Nodo;Etiqueta_Nodo;Flag_Consolidado;Flag_Capacidad;Y001;Nivel\n");
	for(i=0;i<g.nvertices;i++){
	/* Imprimir sólo los nodos válidos */
		if (n[i].flag_valid){
		    /* Si todas las relaciones salientes han sido omitidas,
		    y el nodo no posee valor... */
            if((g.degree[i] == omitir_nodos) && (n[i].valor_nodo==0.0)){
                printf("Ignorando nodo %d\n",i);
            } else {
                fprintf(file1,"%d;%s;%d;%d;%.6lf;%d\n",
                    n[i].indice,n[i].label,n[i].flag_consolidado,n[i].flag_capacity,
                    n[i].capacidad,n[i].tipo_nodo);
            }
		}
	}
	fclose(file1);
	return 0;
}

int print_graph_summary()
{
    int i,j;
    int nodos_utilizacion=0;
    int npaths=0;
	double costo_primario_fijo=0.0;
	double costo_primario_proporcional=0.0;
	double costo_primario_fijo_final=0.0;
	double costo_primario_proporcional_final=0.0;
	double costo_secundario_fijo=0.0;
	double costo_secundario_proporcional=0.0;
	double utilizacion = 0.0;
	double consumo=0.0;
	/* Usamos double para acumuladores, para evitar errores de redondeo */
    double path_value=0.0;
	double flujo_salida=0.0;
	double flujo_llegada=0.0;


    for(i=0;i<g.nvertices;i++){
          costo_primario_fijo+= n[i].costo_primario_fijo;
          costo_primario_proporcional+= n[i].costo_primario_proporcional;
          if(g.degree[i]==0){    // Nodo terminal
                costo_primario_fijo_final += n[i].costo_primario_fijo;
                costo_primario_proporcional_final += n[i].costo_primario_proporcional;
                costo_secundario_fijo+= n[i].costo_secundario_fijo;
                costo_secundario_proporcional+= n[i].costo_secundario_proporcional;
                // Calculamos el total de los flujos de llegada
                for(j=0;j<g.indegree[i];j++){
                    int start_node = g.back_edges[i][j];
                    int end_node_index = get_node_degree(start_node, i);
                    flujo_llegada+=g.flujo[start_node][end_node_index];
                }
          }
          //Calculamos el total de los flujos de salida
          if(g.indegree[i]==0){
                for(j=0;j<g.degree[i];j++){
                    flujo_salida+=g.flujo[i][j];
                }
          }
          //Calculamos el consumo total
          for(j=0;j<g.degree[i];j++){
            consumo += g.consumo[i][j];
          }
          //Calculamos la utilizacion de capacidad promedio
          if(n[i].capacidad > 0.0){
            if(n[i].utilizacion == 0){
                printf("Nodo %s tiene capacidad pero no tiene utilizacion\n",
                       n[i].label);
            }
            utilizacion += n[i].utilizacion;
            nodos_utilizacion++;
          }


    } //End for i

    for(i=0;i<g.npaths;i++){
        if(g.path_type[i]==1){              // Camino completo
            if(fabs(g.path_value[i])>0.0){
                path_value+=g.path_value[i];
                //fprintf(flog,"%.4f;%.4f\n",g.path_value[i],path_value);
                npaths++;
            }
        }
    }
    printf("TRZ v 1.1 - Programa de trazabilidad de datos\n");
    printf("Creado por Mauricio Roman Rojas\n");
    printf("***RESUMEN***\n");
    printf("Entradas:\n");
    printf("No. Nodos: %d\n",g.nvertices);
    printf("No. Relaciones Unicas: %d\n",g.nedges);
    printf("Costo Primario Fijo Total: %.2f MM\n",(costo_primario_fijo/1e6) );
    printf("Costo Primario Proporcional Total: %.2f MM\n",(costo_primario_proporcional/1e6) );
    printf("Costo Primario Fijo Total (Nodos Terminales): %.2f MM\n",(costo_primario_fijo_final/1e6) );
    printf("Costo Primario Proporcional Total (Nodos Terminales): %.2f MM\n",(costo_primario_proporcional_final/1e6) );

    printf("Consumo Secundario Total: %.2f\n",consumo);
    printf("Salidas:\n");
    printf("Costo Secundario Fijo Total (Nodos Terminales): %.2f MM\n",(costo_secundario_fijo/1e6) );
    printf("Costo Secundario Proporcional Total (Nodos Terminales): %.2f MM\n",(costo_secundario_proporcional/1e6) );
    printf("Flujo Total Salida (COP MM): %.2f\n",(flujo_salida/1e6));
    printf("Flujo Total Llegada (COP MM): %.2f\n",(flujo_llegada/1e6));
    printf("Relacion Flujo Llegada:Salida %.2f%%\n", (flujo_llegada / flujo_salida *100));
    printf("Utilizacion Total: %.2f%\n",utilizacion);
    printf("Nodos con Capacidad: %d\n",nodos_utilizacion);
    printf("Utilizacion Promedio: %.2f\n",(utilizacion/(double)nodos_utilizacion));
    printf("Numero de Caminos Encontrados: %d\n",g.npaths);
    printf("Numero de Caminos con Valor Mayor a 0: %d\n",npaths);
    printf("Valor Total del Flujo a Traves de los Caminos: %.2f\n",(path_value/1e6));
    printf("Relacion Flujo Caminos:Llegada %.2f%%\n", (path_value / flujo_llegada *100));
}


char * deblank(char *str)
{
  char *out = str, *put = str;

  for(; *str != '\0'; ++str)
  {
    if(*str != ' ')
      *put++ = *str;
  }
  *put = '\0';

  return out;
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int set_comma(bool option)
{
    struct lconv *lcPtr;
    /* set comma as decimal separator */
    if (option==1){
        //printf( "Localidad: \"%s\"\n\n", setlocale( LC_ALL, "C" ) );

        //Optamos por reemplazar la coma por el punto en la funcion cvs_put_space
        lcPtr = localeconv();
        lcPtr->decimal_point = ",";
        //lcPtr->thousands_sep = ".";
        //lcPtr->grouping = "3";
        //printf( "decimal_point = \"%s\"\n",     lcPtr->decimal_point );
        //printf( "thousands_sep = \"%s\"\n",     lcPtr->thousands_sep );
        //printf( "grouping = \"%s\"\n",          lcPtr->grouping );
    } else {
        lcPtr = localeconv();
        lcPtr->decimal_point = ".";
    }
    return 1;
}



char *get_time(void)
{
    time_t current_time;
    char *c_time_string;

    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to compute the current time.");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.");
        exit(EXIT_FAILURE);
    }

    /* Print to stdout. */
    printf("Current time is %s", c_time_string);
    return c_time_string;
}

int DirectoryExists( const char* absolutePath ){

    if( _access( absolutePath, 0 ) == 0 ){

        struct stat status;
        stat( absolutePath, &status );

        return (status.st_mode & S_IFDIR) != 0;
    }
    return false;
}

int concatenate_path_string(char *dir, char *strBuf, int i, int origin, char *pdf_file) {

    size_t totalLength = 0;
    int j;
    //int pos=0;

    for(j=0;j<g.path_length[i];j++){
        totalLength += (strlen(n[g.path[i][j]].label)+1);
        //printf("totalLength %d label %s\n",totalLength,n[g.path[i][j]].label);
    }

    totalLength+=4;
    //printf("Longitud de nombre de archivo es %d - origin es %d\n", totalLength, origin);

    if (totalLength >= FILENAME_MAX){
        printf("Tamano de nombre de archivo (%d) para camino %d excede maximo permitido (%d)\n", totalLength, i, FILENAME_MAX);
        exit(EXIT_FAILURE);
    }

    if(origin==0){
        strncat(dir,n[g.path[i][0]].label,FILENAME_MAX);
    } else {
        strncat(dir,n[g.path[i][g.path_length[i]]].label,FILENAME_MAX);
    }

    //printf("dir es %s\n", dir);

    for(j=0;j<g.path_length[i];j++){
        strncat(strBuf,n[g.path[i][j]].label,totalLength);
        if(j!=g.path_length[i]-1) strncat(strBuf,"_",totalLength);
    }
    strncpy(pdf_file, strBuf, totalLength-4);

    strncat(strBuf,".dot",totalLength);
    strncat(pdf_file,".pdf",totalLength);

    //printf("Nombres de archivo: %s %s\n", strBuf, pdf_file);

    return 0;

}

int csv_put_space(char s[]) {

    int j = 0;

    while (s[j] != '\0'){
        if (s[j] == ';')
            s[j]='\t';
        j++;
    }
    return 0;
}

int csv_replace_comma(char s[]) {

    int j = 0;

    while (s[j] != '\0'){
        if (s[j] == '.')
            s[j]=',';
        j++;
    }
    return 0;
}


int initialize_nodes()
{
		int i;							/* contador */

		for (i=0; i<= MAXV; i++){

			//Campos cargados de archivo
			n[i].indice = 0;
			n[i].tipo_nodo = -1;
			sprintf(n[i].label,"\n");
            n[i].flag_capacity = 0;
            n[i].flag_consolidado = 0;
            n[i].flag_sorted = 0;
            n[i].flag_valid = 0;
            n[i].flag_tasa=0;
			n[i].capacidad = 0.0;

			//Campos calculados por el programa
			n[i].costo_primario_fijo = 0.0;
			n[i].costo_primario_proporcional = 0.0;
			n[i].utilizacion = 0.0;
			n[i].costo_secundario_fijo = 0.0;
			n[i].costo_secundario_proporcional = 0.0;
			n[i].tasa_fija = 0.0;
			n[i].tasa_proporcional = 0.0;
            n[i].congelar_tasas = 0;
            n[i].total_entradas = 0.0;
            n[i].total_salidas = 0.0;
            n[i].valor_nodo = 0.0;

            g.nvertices=0;
		}
    return 0;
}

int initialize_graph()
{
		int i,j;						/* contador */

		for (i=0; i<=MAXV; i++){
            for (j=0; j<=MAXDEGREE; j++){
                g.edges[i][j] = -1;
                g.back_edges[i][j] = -1;
                g.consumo[i][j] = 0.0;
                g.tipo_consumo[i][j] = -1;
                g.flujo[i][j] = 0.0;
                g.flujo_fijo[i][j]=0.0;
                g.flujo_prop[i][j]=0.0;
                g.flujo_porc[i][j]=0.0;
                g.discarded[i][j]=0;
                g.path[i][j]=0;
            }
            g.path_type[i]=-1;
            g.path_length[i]=-1;
            g.path_value[i]=0.0;
            g.degree[i] = 0;
            g.indegree[i] = 0;
            g.feedback_arc[i][0] = -1;
            g.feedback_arc[i][1] = -1;
            g.feedback_arc_tipo_consumo[i]=-1;
            g.feedback_arc_consumo[i]=0.0;
            g.node_type[i]=-1;
		}
        g.narcs=0;
        g.nedges=0;
        g.npaths=0;
        g.ntypes=0;
        g.nvalid=0;
		return 0;
}

int load_nodes(char *filename)
{

        char col1[MAXLABEL],col2[MAXLABEL],col3[MAXLABEL],col4[MAXLABEL];
        char col5[MAXLABEL],col6[MAXLABEL];;
        *col5=0;

        FILE *file1 = fopen( filename, "r" );

        /* fopen devuelve 0, puntero NULL, si fracasa */
        if ( file1 == 0 )
        {
            printf( "No se pudo abrir el archivo %s\n",filename);
            exit(EXIT_FAILURE);
        }
        else {
            char linea[255], node_label[MAXLABEL];
            int node_id=-1;


            /* Leer archivo de elementos de las dimensiones (nodos)
             * Dim = cuentas contables y receptores
             * Este archivo viene de GetDim.php
             * ID_Nodo;Etiqueta_Nodo;Tipo_Nodo;Bandera_Capacidad;Capacidad(Y001);
             * Leer la primera linea del archivo de nodos */
            fgets(linea, MAXLINE, file1);
            strcpy(linea, deblank(linea));
            csv_put_space(linea);
            sscanf(linea, "%s %s %s %s %s %s", col1,col2,col3,col4, col5, col6);
            if(!strncmp(col5,"Y001",MAXLABEL)){
                ;           //Col 5 tiene capacidades
            } else {
                printf("Formato de archivo de Nodos es incompatible: Col. 5 debe ser Y001\n");
                exit(EXIT_FAILURE);
            }

            while  ( fgets(linea, MAXLINE, file1) != NULL )
            {
                int flag_consolidado=0, flag_capacity=0;
                double capacidad=-1.0 ;
                int node_level=-1;
                strcpy(linea, deblank(linea));
                csv_put_space(linea);
                csv_replace_comma(linea);
                sscanf(linea, "%d %s %d %d %lf %d", &node_id, node_label, &flag_consolidado, &flag_capacity, &capacidad, &node_level);
                /* En caso de que no se haya colocado capacidad, ésta será el default de -1.0 */
                if(capacidad < 0.0)
                    capacidad=0.0;
                if(node_id && strlen(node_label)){
                    if(flag_consolidado){
                        fprintf(flog, "Insertando nodo consolidado\n");
                        fprintf(flog, "node_id %d node_label: %s flag_consolidado %d flag_capacity:%d capacidad %.5f nivel %d\n", node_id, node_label, flag_consolidado, flag_capacity, capacidad, node_level );
                    } else {
                        fprintf(flog, "Insertando nodo base\n");
                        fprintf(flog, "node_id %d node_label: %s flag_consolidado %d flag_capacity:%d capacidad %.5f nivel %d\n", node_id, node_label, flag_consolidado, flag_capacity, capacidad, node_level );
                    }
                    //Ingresar filtro para no insertar nodos con flag cap = 0 y cap
                    insert_node(node_id,node_label,flag_consolidado, flag_capacity, capacidad);
                    insert_node_type(node_id,node_label, node_level);
                } else {
                    fprintf(flog, "Nodo %d con etiqueta=%s no fue insertado", node_id, node_label);
                }
            }
            fclose( file1 );
        }
		return 0;
}
//Esta función es obsoleta

int load_node_types(char *filename)
{

        FILE *file = fopen( filename, "r" );

        /* fopen devuelve 0, puntero NULL, si fracasa */
        if ( file == 0 )
        {
            printf( "No se pudo abrir el archivo %s\n",filename );
            exit(EXIT_FAILURE);
        }
        else {
            char linea[255], node_label[64];
            int node_id=0,node_type=-1;

            //Leer archivo de elementos Receptores
            while  ( fscanf(file, "%s", linea) != EOF )
            {
                csv_put_space(linea);
                sscanf(linea, "%d %s %d", &node_id, node_label, &node_type);
                printf( "node_id %d node_label %s node_type:%d \n", node_id, node_label, node_type);
                if(node_type <= 0 && node_type > MAXV){
                    printf("Error: Tipo %d no permitido para nodo %s\n",node_type,node_label);
                    exit(EXIT_FAILURE);
                } else
                    insert_node_type(node_id,node_label, node_type);
            }
            fclose( file );
        }
        printf("Finalizando load_node_types()\n");
        print_node_types();
		return 0;
}

int valid_edge_input(char *a, char *b, double x, double y)
{
    if(strlen(a) && strlen(b) && (fabs(x) > 0.0 || fabs(y) > 0.0)){
        return 1;
    } else {
        return 0;
    }

}

int load_graph(char *filename1, char *filename2)
{
        FILE *file1 = fopen( filename1, "r" );
        FILE *file2 = fopen( filename2, "r" );

        /* fopen devuelve 0, puntero NULL, si fracasa */
        if ( file1 == 0 ){
            printf( "No se pudo abrir el archivo %s\n",filename1 );
            exit(EXIT_FAILURE);
        }
        else if ( file2 == 0 ){
            printf( "No se pudo abrir el archivo %s\n",filename2 );
            exit(EXIT_FAILURE);
        }

        char linea[MAXLINE+1];
        char cuenta[MAXLABEL], emisor[MAXLABEL], receptor[MAXLABEL];
        int  tipo_consumo=0;
        double consumo=0.0, consumo_fijo=0.0, consumo_prop=0.0;
        char col1[MAXLABEL],col2[MAXLABEL],col3[MAXLABEL],col4[MAXLABEL];
        *col3=0;*col4=0;

        /* Leer primera línea de archivo de Primarios */
        //fscanf(file1, "%s", linea);
        fgets(linea, MAXLINE, file1);
        strcpy(linea, deblank(linea));
        csv_put_space(linea);
        sscanf(linea, "%s %s %s %s", col1,col2,col3,col4);

        if(strncmp(col3,"C001",MAXLABEL) || strncmp(col4,"C002",MAXLABEL)){
            printf("\nEn la primera fila de archivo de Primarios\nLas columnas 3 y 4 deben ser C001 y C002\n");
            printf("Son %s y %s\n",col3,col4);
            exit(EXIT_FAILURE);
        }

        while  ( fgets(linea, MAXLINE, file1) != NULL )
        {
            double costo_primario_fijo=0.0, costo_primario_prop=0.0;
            *cuenta = 0;
            *receptor = 0;
            fprintf(flog,"%s\n",linea);
            strcpy(linea, deblank(linea));
            csv_put_space(linea);

            fprintf(flog,"%s\n",linea);
            sscanf(linea, "%s %s %lf %lf", cuenta, receptor, &costo_primario_fijo, &costo_primario_prop);
            fprintf(flog, "cuenta: %s receptor %s costo_primario_fijo: %.2f costo_primario_prop %.2f\n", cuenta, receptor, costo_primario_fijo, costo_primario_prop);
            if(valid_edge_input(cuenta, receptor,costo_primario_fijo, costo_primario_prop)){
                insert_edge(get_node_index_from_label(cuenta),
                        get_node_index_from_label(receptor),
                        costo_primario_fijo, costo_primario_prop,0,1);
            } else {
                fprintf(flog, "WARNING: Arista %s->%s con costos (%.2f,%.2f) no es valida y no fue insertada\n",cuenta,
                        receptor, costo_primario_fijo, costo_primario_prop);
            }
        }
        fclose( file1 );

        /* Leer primera línea del archivo de secundarios */
        *col3=0;*col4=0;
        int tipo_formato_consumo=0;

        fscanf(file2, "%s", linea);
        //printf(linea);
        csv_put_space(linea);
        sscanf(linea, "%s %s %s %s", col1,col2,col3,col4);
        if(!strncmp(col3,"Q001",MAXLABEL) || !strncmp(col4,"Q002",MAXLABEL)){
            tipo_formato_consumo=1;         //Q001 y Q002
            tipo_consumo=-1;                // Por definir
        } else if(!strncmp(col3,"Q001",MAXLABEL)){
            tipo_formato_consumo=2;         //Q001 en 3a fila
            tipo_consumo=1;
        } else if(!strncmp(col3,"Q002",MAXLABEL)){
            tipo_formato_consumo=3;         //Q002 en 3a fila
            tipo_consumo=2;
        } else {
            printf("Formato de archivo de Consumo Secundario es incompatible\n");
            exit(EXIT_FAILURE);
        }
        int i=2,k=0;
        while  ( fscanf(file2, "%s", linea) != EOF) {
            consumo=0.0;
            csv_put_space(linea);
            if(tipo_formato_consumo==1){
                tipo_consumo=-1;
                sscanf(linea, "%s %s %lf %lf", emisor, receptor, &consumo_fijo, &consumo_prop);
                if (consumo_fijo > 0.0 && consumo_prop > 0.0){
                    printf("Error: relacion %s -> %s no puede tener consumo fijo (%.2f) y proporcional (%.2f)\n",emisor, receptor,consumo_fijo, consumo_prop);
                    exit(EXIT_FAILURE);
                } else if (consumo_fijo > 0.0){
                    tipo_consumo = 1;
                    consumo=consumo_fijo;
                } else if (consumo_prop > 0.0){
                    tipo_consumo = 2;
                    consumo=consumo_prop;
                }
                //printf("tipo formato consumo 1");
            } else if (tipo_formato_consumo == 2) {
                sscanf(linea, "%s %s %lf", emisor, receptor, &consumo_fijo);
                //printf("tipo formato consumo 1");
            } else if (tipo_formato_consumo == 3) {
                sscanf(linea, "%s %s %lf", emisor, receptor, &consumo_prop);
                //printf("tipo formato consumo 1");
            }

            //printf("Receptor %s Consolidado? %d\n",receptor,n[get_node_index_from_label(receptor)].flag_consolidado);
            // Si el receptor es un nodo consolidador - parar y notificar
            if(n[get_node_index_from_label(receptor)].flag_consolidado){
                printf("En la linea %d, el nodo %s es receptor, y consolidador\n",(i-1), receptor);
                exit(EXIT_FAILURE);
            }

            if(n[get_node_index_from_label(emisor)].flag_consolidado){
                printf("En la linea %d, el nodo %s es emisor, y consolidador\n",(i-1), emisor);
                exit(EXIT_FAILURE);
            }

            double metrica = fabs(consumo_fijo)+ fabs(consumo_prop);
            //printf("fijo %.5f prop %.5f metrica %.5f\n",consumo_fijo, consumo_prop, metrica);
            fprintf(flog, "emisor: %s receptor %s consumo fijo: %.8f consumo prop: %.8f tipo consumo: %d\n", emisor, receptor, consumo_fijo, consumo_prop, tipo_consumo);
//            if(metrica>CONSUMO_TOLERANCIA){
            if(metrica>p.tolerancia_consumo){
                fprintf(flog,"Analizando arista %s->%s con consumo %.4f,%.4f - tipo %d\n",emisor, receptor, consumo_fijo, consumo_prop, tipo_consumo);
                insert_edge(get_node_index_from_label(emisor),
                            get_node_index_from_label(receptor),
                            consumo_fijo, consumo_prop, tipo_consumo,2);
            } else {
                fprintf(flog,"Ignorando relacion en linea %d: %s -> %s Valores: (%.5f,%.5f)\n",
                        i, emisor,receptor,consumo_fijo, consumo_prop);
                k++;
            }
            i++;
        }
        if(k>0){
            printf("Se ignoraron %d relaciones de consumo pues sus valores estan debajo de %.5f\n",k,p.tolerancia_consumo);
        }
        fclose( file2 );
		return 0;
}

int insert_edge(int x, int y, double c1, double c2, int tipo_c,int type_edge)
{
    // type_edge=1 (Primario):
    // c1 = costo primario fijo
    // c2 = costo primario proporcional
    // tipo_c - no usada
    // type_edge=2 (Secundario)
    // c1 = consumo fijo
    // c2 = consumo prop
    // tipo_c = tipo consumo [fijo | proporcional]

        //int sender = get_node_index(x);
        //int receiver = get_node_index(y);

		if(g.degree[x] > MAXDEGREE || g.indegree[y] > MAXDEGREE){
			printf("Error: La inserción de la relación (%d,%d) excede el grado máximo\n", x, y);
            printf("Nodo1: %s Nodo 2:%s\n", n[x].label, n[y].label);
            printf("Nodo1 (Outdegree): %d Nodo 2 (Indegree): %d\n",g.degree[x],g.indegree[y]);
            exit(EXIT_FAILURE);
        }
        /* Como puede haber muchas relaciones repetidas, solo creamos una
           nueva arista si la relación es única */
        if(get_unique_vertex(x,y)){
            fprintf(flog,"Insertando nueva arista %s->%s\n",n[x].label,n[y].label);
            g.edges[x][g.degree[x]] = y;
            g.back_edges[y][g.indegree[y]] = x;
            g.degree[x]++;
            g.indegree[y]++;
            g.nedges++;
        } else {
            fprintf(flog,"Se ignoro vertice %s -> %s pues esta repetido\n",n[x].label, n[y].label );
        }

        /* Como puede haber muchas relaciones repetidas, se van sumando
           los valores, tanto en el nodo como en la arista */
        if(type_edge == 1){                                 // Primario
            n[y].costo_primario_fijo += c1;
            n[y].costo_primario_proporcional += c2;
            g.flujo[x][get_node_degree(x,y)] += (c1 + c2);           // Acumula
            fprintf(flog,"Agregando costo (%.2f;%.2f) a la arista %s->%s\n",c1,c2,n[x].label,n[y].label);
        } else if(type_edge == 2){
            double consumo=0.0;
            if(tipo_c==1){                          // Secundario
                consumo=c1;
            } else if(tipo_c==2){
                consumo=c2;
            }
            g.consumo[x][get_node_degree(x,y)] += consumo;
            g.tipo_consumo[x][get_node_degree(x,y)] = tipo_c;
            fprintf(flog,"Agregando consumo (%.4f;%d) a la arista %s->%s\n",consumo,tipo_c,n[x].label,n[y].label);
            fprintf(flog,"Consumo total %.4f\n",g.consumo[x][get_node_degree(x,y)]);
            fprintf(flog,"Grado de relación es %d\n",get_node_degree(x,y));
        }

		return 0;
}
int insert_node(int node,char *label, bool flag_consolidado, bool flag_capacity, double capacidad)
{
    int i=g.nvertices++;
    n[i].indice = node;
    sprintf(n[i].label,label);
	n[i].flag_capacity = flag_capacity;
	n[i].flag_consolidado = flag_consolidado;
    n[i].capacidad = capacidad;

    return 0;
}

int get_unique_vertex(int x1, int x2)
{
    int i,j,unique=1;
    //printf("Revisando si el vertice es unico\n",j);
    int same=0;
    for(i=0;i<g.degree[x1];i++){
        if(g.edges[x1][i] == x2){
            unique = 0;
            return unique;
        }
    }
    return unique;
}

//deprecated
int insert_node_capacity(char *label, double capacity)
{
        int index = get_node_index_from_label(label);
        if (index>=0){
            n[index].capacidad = capacity;
            return 0;
        } else {
            printf("Error: No se encontró indice para el nodo %s\n",label);
            //exit(EXIT_FAILURE);
        }
        return 1;
}

bool new_node_type(int node_type)
{
    int i;

    for(i=0;i<g.ntypes;i++)
        if(node_type==g.node_type[i])
            return FALSE;

    return TRUE;
}

int insert_node_type(int node, char *label, int node_type)
{
        int index = get_node_index(node);
        if (index>=0){
            n[index].tipo_nodo = node_type;
            if (new_node_type(node_type)){
                fprintf(flog,"Se detecto nuevo tipo de nodo, %d\n",node_type);
                g.node_type[g.ntypes]=node_type;
                g.ntypes++;
                print_node_types();
            }

            return 0;
        } else {
            printf("Error: No se encontro indice para el nodo %s\n",label);
            exit(EXIT_FAILURE);
        }
        return 1;
}

int get_node_index(int node)
{
    int i;
    for (i=0;i<g.nvertices;i++)
        if (n[i].indice == node)
            return i;
    return -1;
}
int get_node_index_from_label(char *label)
{
    int i;

    for (i=0;i<g.nvertices;i++){
        if (!strncmp(n[i].label,label,MAXLABEL)){
            return i;
        }
    }
    return -1;
}
int get_node_degree(int node1, int node2)
{
    int i;
    if(node1 > MAXV){
        printf("Error: el indice del nodo %d excede el maximo permitido\n",node1);
        exit(EXIT_FAILURE);
    }
    for (i=0;i<g.degree[node1];i++){
        if(g.edges[node1][i] == node2){
            return i;
        }
    }
    return -1;
}

int save_graph(char *filename1,char *filename2)
{
        int i=0, j=0;
        FILE *file1 = fopen( filename1, "w" );
        FILE *file2 = fopen( filename2, "w" );


        fprintf(file1, "Y001;Y002;RT01;RT03;Emisor\n");
        for (i=0; i < g.nvertices; i++){
            if(g.degree[i]>0 && g.indegree[i]>0){
                fprintf(file1, "%.2f;%.2f;%.5f;%.5f;%s\n",n[i].capacidad,n[i].utilizacion,n[i].tasa_fija,n[i].tasa_proporcional,n[i].label);
                if (n[i].flag_capacity && !n[i].flag_tasa){
                   fprintf(flog, "Error: No se proceso el nodo %s\n",n[i].label);
                }
            }
        }
        fclose(file1);



        fprintf(file2, "SC01;SC03;Emisor;Receptor\n");
		for (i=0; i < g.nvertices; i++)
            for (j=0; j < g.degree[i]; j++)
                if(g.flujo_fijo[i][j]!=0.0 || g.flujo_prop[i][j]!=0.0)
                    fprintf(file2, "%.2f;%.2f;%s;%s\n",g.flujo_fijo[i][j],g.flujo_prop[i][j],n[i].label,n[g.edges[i][j]].label);
        fclose(file2);

		return 0;
}

int save_path_flow(char *filename, int *path_cube_dir)
{
    int i,j;
    FILE *file = fopen( filename, "w" );

//Año;Mes;Version;Nivel_0;Nivel_1;Nivel_2;Nivel_3;Nivel_4;Medida;#Value

    fprintf(file,"Año;Mes;Version");
    for(i=0;i<g.ntypes;i++)
        fprintf(file,";Nivel_%d",i);
    fprintf(file,";Medida;#Value\n");

    for(i=0;i<g.npaths;i++){
        if(g.path_type[i]==1){              // Camino completo
            if(fabs(g.path_value[i])>0.0){ // Caminos con valor positivo
            fprintf(file,"%s;%s;%s",p.year, p.month, p.version);
                for(j=0;j<g.ntypes;j++){
                    int x = *(path_cube_dir+i*g.ntypes+j);
                    if(x==-1){
                        fprintf(file,";Dummy");
                    } else {
                        fprintf(file,";%s",n[x].label);   //Etiqueta del nodo
                    }
                }
            fprintf(file,";valor;%.2lf\n",g.path_value[i]);
            }
        }
    }

    fclose(file);
    return 0;
}

int save_path_flow_file(char *filename)
{
    int i,j;

    FILE *file = fopen( filename, "w" );

    fprintf(file,"valor;nodo1;nodo2;etc.");
    fprintf(file,"\n");

    for(i=0;i<g.npaths;i++){
        if(g.path_type[i]==1){              // Camino completo
            if(fabs(g.path_value[i])>0.0){ // Caminos con valor positivo
                fprintf(file,"%.2f",g.path_value[i]);
                for(j=0;j<g.path_length[i];j++){
                    if(g.path[i][j]>=0){
                        fprintf(file,";%s",n[g.path[i][j]].label);   //Etiqueta del nodo
                    } else {
                        fprintf(file,";{%d}",g.path[i][j]);
                    }
                }
                fprintf(file,"\n");
            }
        }
    }

    fclose(file);
    return 0;
}


int goto_dir(char *dirname)
{
    int rc;

    if(!DirectoryExists(dirname)){
        rc= mkdir (dirname);
        if (rc) {
            printf ("Error creando directorio %s\n",dirname);
            exit(EXIT_FAILURE);
        }
    }
    chdir (dirname);
    return 0;
}

int print_feedback_arc(char *dirname)
{
    int i;
    char filename[FILENAME_MAX], pdf_filename[FILENAME_MAX];

    goto_dir(dirname);
    for(i=0;i<g.narcs;i++){
        //Iniciamos por el final del arco para encontrar todos los nodos conectados
        int nstart=g.feedback_arc[i][1], nend=g.feedback_arc[i][0];
        initialize_search();
        bfs(nstart,0);

        *filename = 0;
        *pdf_filename = 0;
        strncat(filename,n[nend].label,FILENAME_MAX);
        strncat(filename,"_",FILENAME_MAX);
        strncat(filename,n[nstart].label,FILENAME_MAX);
        strncat(filename,".dot",FILENAME_MAX);
        strncpy(pdf_filename,filename,FILENAME_MAX);
        strncpy(&pdf_filename[strlen(filename)-3],"pdf",3);

        FILE *file = fopen( filename, "w" );
        fprintf(file,"digraph {\n rankdir=LR;\n graph [label=\"Empresa: %s Ano:%s Mes: %s Version: %s Usuario: %s Algoritmo: %s Fecha: %s\" labelloc=\"b\" labeljust=\"l\" fontsize=8];\n edge  [fontsize=10];\n node  [fontsize=12];\n", p.client, p.year,p.month,p.version,p.user,p.algoritmo,p.date);

        int j=0;
        while (nstart!=nend && j<MAXV){
            print_node_dot(file, nend);
            print_edge_dot(file, s.parent[nend],get_node_degree(s.parent[nend],nend),2);
            nend = s.parent[nend];
            j++;
        }
        print_node_dot(file, nstart);
        print_edge_dot(file, g.feedback_arc[i][0],get_node_degree(g.feedback_arc[i][0],g.feedback_arc[i][1]),2);

        fprintf(file,"}\n");
        fclose(file);
        if(p.pdf)
            dot_to_pdf (pdf_filename, filename);
    }
    chdir("..");
    return 0;
}

int output_node_detail(int x)
{
    fprintf(flog,"nodo i: %d \n",x);
    fprintf(flog,"indice: %d \n",n[x].indice);
    fprintf(flog,"tipo_nodo: %d \n",n[x].tipo_nodo);
    fprintf(flog,"label: %s \n",n[x].label);
    fprintf(flog,"flag_capacity: %d \n",n[x].flag_capacity);
    fprintf(flog,"flag_valid: %d \n",n[x].flag_valid);
    fprintf(flog,"congelar_tasas: %d \n",n[x].congelar_tasas);
    fprintf(flog,"capacidad: %.4f \n",n[x].capacidad);
    fprintf(flog,"utilizacion: %.4f \n",n[x].utilizacion);
    fprintf(flog,"costo_primario_fijo: %.4f \n",n[x].costo_primario_fijo);
    fprintf(flog,"costo_primario_proporcional: %.4f \n",n[x].costo_primario_proporcional);
    fprintf(flog,"costo_secundario_fijo: %.4f \n",n[x].costo_secundario_fijo);
    fprintf(flog,"costo_secundario_proporcional: %.4f \n",n[x].costo_secundario_proporcional);
    fprintf(flog,"tasa_fija: %.4f \n",n[x].tasa_fija);
    fprintf(flog,"tasa_proporcional: %.4f \n",n[x].tasa_proporcional);
    fprintf(flog,"valor total_entradas: %.4f \n",n[x].total_entradas);
    fprintf(flog,"valor total_salidas: %.4f \n",n[x].total_salidas);
    fprintf(flog,"Grado IN: %d \n",g.indegree[x]);
    fprintf(flog,"Grado OUT: %d \n",g.degree[x]);
    fprintf(flog,"valor_nodo: %.4f \n",n[x].valor_nodo);

    return 0;
}

int print_node_types()
{
    int i;
    fprintf(flog,"Lista de Nodos\n");
    fprintf(flog,"Ntypes = %d\n",g.ntypes);
    for (i=0;i<g.ntypes;i++)
        fprintf(flog,"Tipo_id %d = %d\n",i, g.node_type[i]);
    for (i=0;i<g.nvertices;i++)
        fprintf(flog,"Nodo: %s (%d) Tipo %d\n",n[i].label, i, n[i].tipo_nodo);
    return 0;
}
