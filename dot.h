#ifndef _DOT_H_
#define _DOT_H_

int print_all_paths(char *dirname, int origin);
int print_node_dot(FILE *file, int x);
int print_edge_dot(FILE *file, int i, int j, int mode,int color);
int print_graph(char *filename, int mode);
int print_node_item_dot(FILE *file, int x, double *in_out, char *color, char *shape);
int dot_to_pdf (char *pdf_filename, char *filename);
int dot_to_png (char *png_filename, char *filename);
int set_color(double value);
double *calcular_entradas_salidas(int x);

#endif

