# graph2olap
Algorithm in C that converts a DAG with monetary flows into an OLAP cube for granular reporting,
mapping each level as a dimension

This program is used in Colombia at a Financial Payments company as well as an Insurance company.
The financial payments company wanted to better understand the financial flows in their cost model, which
consisted of a 4 level DAG, mainly for validating their model as it changes dynamically. 

The insurance company uses it to also provide visibility into its cost model, mainly to satisfy regulatory
constraints.
