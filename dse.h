
#ifndef DSE_FUNC_H
#define DSE_FUNC_H


#define DEFAULT_CROSSOVER 80
#define DEFAULT_MUTATION 25
#define DEFAULT_FU_DECREASE 0.1

#define MAX_CHARS_WORD 256
#define MAX_CHARS_LINE 1028
#define MAX_PARENTS_TRIES 200
#define MAX_OFFSPRINGS_TRIES 100
#define MAX_OFFSPRINGS 20
#define MAX_PROB_FU_XPLORE 0.3
#define MAX_INSIDE 5



/* Attribute list structure declaration */
typedef struct cattrlist_struct{

  char *m_name;
  int m_num; /* stores the number of attribute types*/

  struct cattr_struct *m_first_attr;

  struct cattrlist_struct *m_prevp;
  struct cattrlist_struct *m_nextp;

}CAttrlist;



/* Attribute node structure declaration */
typedef struct cattr_struct{

  char *m_name;
  char *m_value;

  int m_min;
  int m_max;

  CAttrlist *m_list;

  struct cattr_struct *m_nextp;
  struct cattr_struct *m_prevp;

}CAttr;


/* Design structure declaration */
typedef struct cdesign_struct{

  int m_count;
  bool m_pareto_optimal;
  bool m_adjusted;
  bool m_interpolated;

  int m_area;
  int m_latency;
  int m_hash;

  int m_prob_max; //probability of pareto optimal design 
  int m_prob;     // adjusted probability based on other exploration designs

  CAttr *m_first_attr;
  struct cdesign_struct *m_design_fu_single;

  struct cdesign_struct *m_prevp;
  struct cdesign_struct *m_nextp;

}CDesign;


/* Global Variables declaration */
typedef enum type_is {brute, genetic}type_is;
/* Global Variables declaration */
typedef enum type_fu {each_design, each_optimal, each_prob, none}type_fu;


extern CDesign *first_design;
extern int crossover;
extern int mutation;
extern char *in_file;
extern char *process;
extern char *flib;
extern char *blib;
extern type_fu fu_method;
extern float percent_fu_decrease;


/* functions declarations */


/* A */
void adjust_in_file(char *);
CDesign *adjust_results(CDesign *);

/* B */
void fu(CAttrlist *);
void brute_force(CAttrlist *);

/* C */
CAttr *copy_attr(CAttr *);
bool check_design_unique(CDesign *, CDesign *);
int count_attributes(CAttrlist *);
void copy_attrs_list(CDesign *, CDesign *, int);
void cross_attrs_list(CDesign *, CDesign *, CDesign *, int);
void crossover_offspring(CDesign *, CDesign *, CDesign *, int, int);
bool copy_fu_and_reduce(CDesign *,char *, int, bool);
void copy_xplore_files(CDesign *, char *);
void copy_data_designs(CDesign *, CDesign *);

/* D */
void dse(CAttrlist *);
void delete_data(CDesign *, CAttrlist *);
void delete_designs(CDesign *);
void delete_attr_list (CAttrlist *);
void delete_attr(CAttr *); 
void delete_single_attr(CAttr *);


/* E */
CDesign *extract_pareto(CDesign *);

/* F */
CDesign *fu_dse(CDesign *, char *, bool);
void fu_dse_optimal(CDesign *,char*);
void fu_dse_mallest_only(CDesign *,char*);
void fu_dse_prob(CDesign *,char*);
void fu_dse_prob_adj(CDesign *, char *);

/* G */
void genetic_algorithm(CAttrlist *, int, int);
CDesign *gen_random_design(CAttrlist *);
CDesign *gen_random_design_single(CAttrlist *);
CDesign *gen_design();
void gen_attr_file(CDesign *);

/* H */
int hash_design(CAttr *);
int hash_attr(CAttr *, int);
int hash_string(char *strg);


/* I */
void insert_attr_list(CAttrlist*, CAttrlist *);
void insert_attr(CAttr *, CAttr *);
bool is_first_attr_list(CAttrlist *, char *);
void insert_design(CDesign *, CDesign *);

/* M */
void mutation_offspring(CDesign *,int);


/* N */
CAttr* new_attr_rnd(CAttr *);


/* O */
CDesign *offspring_better(CDesign *, CDesign *,CDesign*);
CDesign *offspring_gen(CDesign *, CDesign *, int, int, int);
void optimal_designs(CDesign *);
bool optimal_single(CDesign *, CDesign *);

/* P */
void print_version();
void print_help();
void probability_optimal(CDesign *);
int probability_optimal_single(CDesign *,CDesign *);
int prob_box(CDesign *designs_inside[MAX_INSIDE], CDesign *);


/* Q */


/* R */ 
void read_input_arguments(int argc, char *argv[]);
CAttrlist *read_lib(char *);
CAttrlist  *read_lib(char *);
void results_summary(CDesign *,CDesign *, double);
void read_bdltran_syn_results(CDesign *, char *);
void results_single(CDesign *);
void results_summary_design(CDesign *,FILE *);

/* S */
void synthesize(CDesign *);
void synthesize_fu(CDesign *,char *, int);
void store_results(CDesign *, char *);
void start_logfile();
void sort_designs_box(CDesign *designs_inside[MAX_INSIDE]);
CDesign *sort_designs(CDesign *);
void swap_designs(CDesign *, CDesign *);


/* V */



#endif
