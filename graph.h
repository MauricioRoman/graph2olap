#ifndef _GRAPH_H_
#define _GRAPH_H_

int trim_nodes();
int process_vertex(int v);
int process_edge(int x, int y);
int valid_edge(int v, int i);
int toposort(int sorted[]);
//int compute_indegrees();
int find_node_types();
int feedback_arc_set_eades();
int initialize_search();
int bfs(int start, bool process_flag);
int edge_not_arc(int x, int y);
int process_vertex_arc(int x);
int calculate_gap();
int remove_feedback_arcs();

#endif
