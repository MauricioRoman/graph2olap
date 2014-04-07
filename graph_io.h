#ifndef _GRAPH_IO_H_
#define _GRAPH_IO_H_

int initialize_nodes();
int initialize_graph();

int load_graph(char *filename1, char *filename2);
int load_nodes(char *filename);
int insert_edge(int x, int y, double c1, double c2, int tipo_c,int type_edge);
int insert_node(int node,char *label, bool flag_consolidado, bool flag_capacity, double capacidad);
//deprecated
int insert_node_capacity(char *label, double capacity);
int insert_node_type(int node, char *label, int node_type);
bool new_node_type(int node_type);

char *deblank(char* input);
char *trimwhitespace(char *str);
char *get_time(void);
int DirectoryExists( const char* absolutePath );
int goto_dir(char *dirname);
int concatenate_path_string(char *dir, char *strBuf, int i, int origin, char *pdf_filename);
int csv_put_space(char s[]);
int csv_replace_comma(char s[]);
int set_comma(bool option);

int valid_edge_input(char *a, char *b, double x, double y);
int get_node_degree(int node1, int node2);
int get_node_index(int node);
int get_node_index_from_label(char *label);
int get_unique_vertex(int node1, int node2);

int save_graph(char *filename1,char *filename2);
int save_path_flow(char *filename, int *path_cube_dir);
int save_path_flow_file(char *filename);

int print_recarga(char *filename1,char *filename2);
int print_graph_summary();
int print_feedback_arc(char *filename);
int print_node_types();
int output_node_detail(int x);

#endif
