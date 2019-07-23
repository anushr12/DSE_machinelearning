/*************************************************************************************
 **
 ** dse.c
 **
 ** version 1.0 created by : BCS
 ** Date : July 15, 2013
 ** version 1.01 revised by: Dong
 ** Date : Sep 1，2015
 ** Comments: HLS  Design Space Explorer executing different heurstics
 **
 **
 **
 ************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

#include "dse.h"


/* Global Variables declarations */
CAttrlist *first_attr_list = NULL;
type_is method=genetic;
type_fu fu_method = none;
bool verbose= false;
char *in_file = 0;
char *process = 0;
char *flib = 0;
char *blib = 0;
int area_reg_outputs=0;


int exploration_time=-1;
int crossover = DEFAULT_CROSSOVER;
int mutation = DEFAULT_MUTATION;
extern int count_designs;
float percent_fu_decrease=DEFAULT_FU_DECREASE;




int main(int argc, char *argv[]){


  /* Read programs input arguments */
  read_input_arguments(argc, argv);


  /* print program version */
  print_version();

  /* Stop exploration if needed input arguments missing */
  if(first_attr_list == NULL){
    printf("\nMissing attribute library file. Specify using -alib </path/name> \n\n");
    return 1;
  }
  else if(in_file == 0){
    printf("\nMissing SystemC file. Specify using SystemC file (extension should be .sc or .cpp \n\n");
    return  1;
  }    

 else if(process == 0){
    printf("\nMissing Procss declaration. Specify using -process <name> \n\n");
    return 1;
  }    

 else if(flib == 0 || blib == 0){
    printf("\nMissing FLIB of BLIB library files -flib <name> -blib <name> \n\n");
    return 1;
  }   

  /* copy input file to current location and adjust name*/
  adjust_in_file(in_file);



  /* Main DSE function */
   dse(first_attr_list);
 


  /* Free memory for data structures created */
  //delete_data(first_design, first_attr_list);

  return 1;

}


/*************************************
** 
** copy_in_file
**
** Comments: copy input file to current location and adjust name
**
*************************************/
 void adjust_in_file(char *in_file){

   /* Variables declaration */
  char synops[MAX_CHARS_LINE];
  char *pch;
 
  //Adam 29/07 . no meaning 
  sprintf(synops,"rm -rf ./dse_%s", process);
  system(synops);
  
  sprintf(synops,"mkdir ./dse_%s", process);
  system(synops);

  pch=strchr(in_file,'/');

  if(pch != NULL){
    sprintf(synops,"cp %s .  ", in_file);
    system(synops);


    while (pch!=NULL){
      pch=strchr(pch+1,'/');
 
      if(pch != NULL)
	strcpy(synops, pch+1);
    }

    //   free(in_file);
    // in_file = (char *)malloc(sizeof(char)*(strlen(synops)+1));
    strcpy(in_file, synops);

  }
 }




/*************************************
 *************************************
** 
** read_input_arguments
**
** Comments: Reads arguments passed by user
**
**************************************
*************************************/
 void read_input_arguments(int argc, char *argv[]){

    int x=0;

    /* Read all input arguments */
    for(x=0; x< argc; x++){

      /*    printf("\nARG[%d] = %s ",x, argv[x]); */


      /* READ attribute list for the given design */
      if(!strcmp(argv[x], "-alib")){
	x++;	  
	first_attr_list = read_lib(argv[x]);

      }

      /* Read FLIB file */
      else if(!strcmp(argv[x], "-flib")){
	x++;
	flib = (char *)malloc(sizeof(char)*(strlen(argv[x])+1));
	strcpy(flib, argv[x]);
      }

      /* Read BLIB file */
      else if(!strcmp(argv[x], "-blib")){
	x++;
	blib = (char *)malloc(sizeof(char)*(strlen(argv[x])+1));
	strcpy(blib, argv[x]);
      }


      /* crossover rate */
      else if(!strcmp(argv[x], "-crossover")){
	x++;
	crossover = atoi(argv[x]);
      }

      /* mutation rate */
      else if(!strcmp(argv[x], "-mutation")){
	x++;
	mutation = atoi(argv[x]);
      }


      /* READ Heurisic or Brute force */
      else if(!strcmp(argv[x], "-brute"))
	method = brute;
      
      else if(!strcmp(argv[x], "-genetic"))
	method = genetic;


      else if(!strcmp(argv[x], "-verbose"))
	verbose = true;

      /* FU Exploration method */
      else if(!strcmp(argv[x], "-fu_each_design"))
	fu_method = each_design;

      else if(!strcmp(argv[x], "-fu_each_optimal"))
	fu_method = each_optimal;

      else if(!strcmp(argv[x], "-fu_each_prob"))
	fu_method = each_prob;


      else if(!strcmp(argv[x], "-percent_fu_decrease"))
	percent_fu_decrease=atof(argv[x]);

      /* Area of registers at output*/
      else if(!strcmp(argv[x], "-reg_out")){
	x++;
	area_reg_outputs=atoi(argv[x]);
      }


      /* Read process name */
      else if(!strcmp(argv[x], "-time")){
      x++;
      exploration_time = atoi(argv[x]);
      }


      /* Read SystemC file */
      else if(strstr(argv[x],".sc") || strstr(argv[x], ".cpp")){
	in_file = (char *)malloc(sizeof(char)*(strlen(argv[x])+1));
	strcpy(in_file, argv[x]);
      }

      else if(strstr(argv[x],".c") || strstr(argv[x], ".bdl")){
	in_file = (char *)malloc(sizeof(char)*(strlen(argv[x])+1));
	strcpy(in_file, argv[x]);
      }

      /* Read process name */
    else if(!strcmp(argv[x], "-process")){
      x++;
      process= (char *)malloc(sizeof(char)*(strlen(argv[x])+1));
      strcpy(process, argv[x]);
      }

    else if(!strcmp(argv[x], "-v")){
	print_version();
	exit(EXIT_SUCCESS);

    }
  
    else if(!strcmp(argv[x], "-h")){
	print_help();
	exit(EXIT_SUCCESS);

    }
    }
 }

/***********************
** 
** version
**
** Comments: prints the version of the tool
**
************************/
void print_version(){

  printf("\n\nHLS DSE ");
  printf("\n(c) PolyU 2014");
  printf("\nWritten by PolYU DARClab");
  printf("\nVersion 1.0\nBuild  %s %s\n\n", __TIME__, __DATE__);

}


/***********************
** 
** help menu
**
** Comments: prints the version of the tool
**
************************/
void print_help(){

  print_version();

  /* print options */
  printf("\n\n-alib <name> : attribute library file. File with all attributes to be explored");
  printf("\n-flib <name> : FLIB. Functional Unit Library needed by CWB");
  printf("\n-blib <name> : BLIB. Basic Functional Unit Library needed by CWB");
  printf("\n-process <name> : Top process to be synthesized by CWB");

  printf("\n");
  printf("\n-fu_each_design : Perform FU exploration for each synthesized design");
  printf("\n-fu_each_optimal : Perform FU exploration only for Pareto Optimal desgigns found after attr exploration");
  printf("\n-fu_each_prob : Perform FU exploration only for probabilistic Pareto Optimal designs");

  printf("\n\n");

}




/***********************
** 
** read_lib
**
** Comments: reads the data stored in lib.info (attributes names and argument
**           for the given benchmark
**
************************/
CAttrlist *read_lib(char *lib_name){


  FILE *ptr_file = NULL;
  CAttrlist *list_new = NULL;
  CAttr *attr_new = NULL;
  char line[MAX_CHARS_LINE], *token, *pch;
  int count = 0;
  

  printf("\nReading library Lib %s ...", lib_name);

  ptr_file = fopen(lib_name, "rt");
  if(ptr_file == NULL){
    printf("\nCould not open file %s \n\n", lib_name);
    exit(EXIT_FAILURE);
  }


  /* Read file data and generate structure linked list */
  while(fgets(line, sizeof line, ptr_file) != NULL){

    if(line[0] == '#')
      continue;

    count = 0;
    if(strstr(line, "attr") ){

      token = strtok(line," ");
      while (token != NULL){
       

	/* Attriubute <attr1, attr2, attr3...> */
	if(count == 0){
	  if(is_first_attr_list(first_attr_list, token)){

	    list_new = (CAttrlist*) malloc(sizeof(CAttrlist));
	    list_new->m_first_attr = NULL;
	    
	    list_new->m_name = (char *)malloc(strlen(token)+1);
	    strcpy(list_new->m_name, token);
	    list_new->m_num =0;
	    
	    list_new->m_nextp= NULL;
	    list_new->m_prevp = NULL;

	    if(first_attr_list == NULL)
	      first_attr_list = list_new;
	    else
	      insert_attr_list(first_attr_list, list_new);     
	  }
	}

	/* Attribute name */
	else if(count == 1){
	  attr_new = (CAttr *)malloc(sizeof(CAttr));
	  attr_new->m_list = list_new;	  
	  attr_new->m_name = (char*)malloc(strlen(token)+1);
	  strcpy(attr_new->m_name, token);
	  attr_new->m_min = 0;
	  attr_new->m_max = 0;

	  attr_new->m_nextp = NULL;
	  attr_new->m_prevp = NULL;

	  list_new->m_num ++;

	  if(list_new->m_first_attr  == NULL)
	    list_new->m_first_attr = attr_new;
	  else
	    insert_attr(list_new->m_first_attr, attr_new );

	}

	/* Attribute value */
	else if(count == 2){
	  if(token[strlen(token)-1]=='\n')
	    token[strlen(token)-1]='\0';

	  attr_new->m_value = (char*)malloc(strlen(token)+1);
	  strcpy(attr_new->m_value, token);
	 
	  /* Separate ranges if attribute arguments are given as a range
	  ** e.g. attr1 unroll_times 0-5 
	  */
	  if(strstr(token, "-")){
	   
	     pch=strchr(token,'-');
	     token[pch-token] ='\0';
	     attr_new->m_min = atoi(token);
	     attr_new->m_max = atoi(pch+1);

	  }
	}

	count ++;
	token = strtok(NULL," ");
      
      }
    }
  }

  fclose(ptr_file);

  return  first_attr_list;

}


/***********************
** 
** is_first_attr_list
**
** Comments: checks if attributes for this construct does not exist
**
************************/
bool is_first_attr_list(CAttrlist *first_attr_list, char *token){


  CAttrlist *list = NULL;
 
  for(list=first_attr_list; list; list=list->m_nextp){

    if(!strcmp(list->m_name, token))
      return false;
  }

  return true;

  }


/***********************
** 
** insert_attr_list
**
** Comments: Inserts a new attribute into the attributes' linked list
**
************************/
 void insert_attr_list(CAttrlist *first_attr_list, CAttrlist *list_new){

   CAttrlist *list = NULL;

   /* Go to the very last linked list element */
   for(list=first_attr_list; list->m_nextp != NULL; list=list->m_nextp);

   
   /* insert process at the end of the list */
   list->m_nextp = list_new;
   list_new->m_prevp = list;

 }
    

/***********************
** 
** insert_attr
**
** Comments: inserts the data of a single design to the process node
**
************************/
 void insert_attr(CAttr *first_attr, CAttr *attr_new){

   CAttr *attr = NULL;

   for(attr=first_attr; attr->m_nextp != NULL; attr=attr->m_nextp);

   
   /* insert process at the end of the list */
   attr->m_nextp = attr_new;
   attr_new->m_prevp = attr;

 }
    





/*****************************************************************
******************************************************************
**
** DSE
**
** Comments: Main DSE function. 
**           Includes 4 methods
**
**           Genetic Algorithm
**
**
*******************************************************************
*******************************************************************/
void dse(CAttrlist *first_attr_list){

  /* Variables declarations  */
  CDesign *first_pareto=NULL;

  time_t end,  start=time(NULL);
  double elapsed;

  /* Start writting results in log file -> In case processes is terminated
  ** before it finishes */
  start_logfile();


  //  if(method == brute)
    brute_force(first_attr_list);

  // else
  //   genetic_algorithm(first_attr_list, crossover, mutation);


     /*
     ** Compute area of design if design with smaller latency
     ** is smaller adding registers at output then dominating design
     */
     first_pareto=adjust_results(first_design);
 
     end = time(NULL);
     elapsed = difftime(end, start);

     /*
     ** print results on screen and on log file
     */ 
     results_summary(first_design,first_pareto, elapsed);
    
 }


/****************************************************************
** 
** adjust_results
**
** Comments:
**     Compute area of design if design with smaller latency
**     is smaller adding registers at output then dominating design
**
****************************************************************/
CDesign *adjust_results(CDesign *first_design){

  /* Variables declaration */
  CDesign *first_design_pareto = NULL;
 //*design= NULL, *design_new=NULL, *first_design_pareto=NULL;
 //  int area=0;

  printf("\nStarting Pareto extraction");
  fflush(stdin);

  /* delete no pareto optimal designs */
  first_design_pareto= extract_pareto(first_design);

  if(first_design_pareto == NULL)
    return NULL;


  /* sort designs based on latency */
  first_design_pareto = sort_designs(first_design_pareto);

  printf("\nSorting based on latency");
  fflush(stdin);

//adam comment on 10/08/2015 no need to use adjust
/*
  if(first_design_pareto == NULL || first_design_pareto->m_nextp == NULL)
    return (first_design_pareto);


  for(design=first_design_pareto; design->m_nextp; design=design->m_nextp){
 

    area = design->m_area + area_reg_outputs;
    

    if(design->m_nextp->m_latency > design->m_latency+1){

      // Generate design with latency = latency+1 
      // area = area+area_reg_outputs
      design_new = gen_design();

      // insert area in linked list
      design_new->m_area =area;
      design_new->m_latency = design->m_latency+1;


      design->m_nextp->m_prevp = design_new;
      design_new->m_nextp = design->m_nextp;
      design->m_nextp = design_new;
      design_new->m_prevp = design;

      design_new->m_pareto_optimal = true;
      design_new->m_interpolated = true;

    }


    else if(design->m_nextp->m_latency == design->m_latency+1)
		if(area < design->m_nextp->m_area){
			design->m_nextp->m_area = area;
			design->m_nextp->m_adjusted = true;
	
		}
  }


  printf("\nFinished adjusting");
  fflush(stdin);
*/


  return first_design_pareto;

}


/*******************************
 **
 ** extract_pareto
 **
 ** extractpareto optimal designs
 **
 *******************************/
CDesign *extract_pareto(CDesign *first_design){

  /* Variables declaration */
  CDesign *design = NULL,*design_new=NULL, *first= NULL, *design_last=NULL;

  
  for(design=first_design; design; design=design->m_nextp){

    if(design->m_pareto_optimal == true && design->m_area > 0){
		design_new = gen_design();
		copy_data_designs(design_new, design);
      

		if(first==NULL)
			first=design_new;
		else{
			design_last->m_nextp= design_new;
			design_new->m_prevp = design_last;
		}
	
		design_last = design_new;
    }
  }


  return first;
}

/*******************************
 **
 ** copy_data_designs
 **
 ** copy_designs_data
 **
 *******************************/
void copy_data_designs(CDesign *design_new, CDesign *design){

  design_new->m_count = design->m_count;
  design_new->m_pareto_optimal =design->m_pareto_optimal;
  design_new->m_adjusted = design->m_adjusted;
  design_new->m_interpolated = design->m_interpolated;

  design_new->m_area = design->m_area;
  design_new->m_latency = design->m_latency;
  design_new->m_hash = design->m_hash;

  copy_attrs_list(design,design_new, -1);


}



/*******************************
 **
 ** sort_designs
 **
 ** Sort all the designs based on latecny
 **
 *******************************/
CDesign *sort_designs(CDesign *first_design){

  /* Variables declaration */
  CDesign  *design=NULL, *design_nxt=NULL;


  if(first_design == NULL)
    return NULL;
  else if (first_design->m_nextp == NULL)
    return NULL;

  /* Sort by area from highest to lowest */
  for(design=first_design; design->m_nextp != NULL; design=design->m_nextp){
    for(design_nxt=design->m_nextp; design_nxt != NULL; design_nxt=design_nxt->m_nextp){

      if(design_nxt->m_latency < design->m_latency){
	swap_designs(design, design_nxt);
	if(design == first_design)
	  first_design = design_nxt;
    
	design = design_nxt;
      }    
    }
  }
    return first_design;

}


/*******************************
 **
 ** swap_designs
 **
 ** swap two designs
 **
 *******************************/
 void swap_designs(CDesign *design, CDesign *design_nxt){

   /* Variables declaration */
   CDesign *tmp=NULL, *tmp2 = NULL;

  if(design->m_nextp == design_nxt){
    tmp = design->m_prevp;
    
    design->m_nextp = design_nxt->m_nextp;
    
    if(design_nxt->m_nextp != NULL)
      design_nxt->m_nextp->m_prevp =  design;

    design->m_prevp = design_nxt;
    design_nxt->m_nextp = design;

    design_nxt->m_prevp = tmp;
	   
    if(tmp != NULL)
      tmp->m_nextp = design_nxt;


  }
  else{
    tmp = design->m_nextp;
    tmp2 = design->m_prevp;

    design->m_nextp = design_nxt->m_nextp;
    design->m_prevp = design_nxt->m_prevp;

    if(design->m_nextp != NULL)
      design->m_nextp->m_prevp = design;

    if(design->m_prevp != NULL)
      design->m_prevp->m_nextp = design;

    design_nxt->m_nextp = tmp;
    design_nxt->m_prevp = tmp2;

    if(tmp != NULL)
      tmp->m_prevp = design_nxt;

    if(tmp2 != NULL)
      tmp2->m_nextp = design_nxt;

  } 


 }




/****************************************************************
 ****************************************************************
** 
** start_logfile
**
** Comments: print out the results on screen and log file
**
****************************************************************
****************************************************************/
void start_logfile(){

  /* Variables declaration */
  FILE *ptr_rpt_file = NULL;
  char report[MAX_CHARS_WORD];


  sprintf(report, "%s%s%s", "log_dse_",process, ".rpt");

 

   ptr_rpt_file = fopen(report, "wt");
   if(ptr_rpt_file == NULL){
     printf("\nCannot create report file %s \n", report);
     exit(EXIT_FAILURE);
   }


   if(method == brute){
     printf("\n\nMethod BRUTE FORCE\n");
     fprintf(ptr_rpt_file,"\n#Method BRUTE FORCE\n");
   }
  
   else{

     printf("\n\nMethod GENETIC ALGORITHM\n");
     fprintf(ptr_rpt_file,"\n#Method GENETIC ALGORITHM \n");

   }


   fprintf(ptr_rpt_file,"\n#PARTIAL RESULTS while executing DSE");
   fprintf(ptr_rpt_file,"\n#Design\t\tArea\tLatency"); 



   fclose(ptr_rpt_file);


}

/****************************************************************
** 
** results_single
**
** Comments: print out single result onto report file
**
****************************************************************/
void results_single(CDesign *design){

  /* Variables declaration */
  FILE *ptr_rpt_file = NULL;
  char report[MAX_CHARS_WORD];



  sprintf(report, "%s%s%s", "log_dse_",process, ".rpt");

   ptr_rpt_file = fopen(report, "at");
   if(ptr_rpt_file == NULL){
     printf("\nCannot create report file %s \n", report);
     exit(EXIT_FAILURE);
   }


   /* Write design details to the log file */
   fprintf(ptr_rpt_file,"\nDesign %d \t %d \t %d", design->m_count, design->m_area, design->m_latency);

   if(design->m_first_attr == NULL)
     fprintf(ptr_rpt_file,"\tFU");
   else
     fprintf(ptr_rpt_file,"\tATTR");



  fclose(ptr_rpt_file);

}



/****************************************************************
** 
** results_summary
**
** Comments: print out the results on screen and log file
**
****************************************************************/
void results_summary(CDesign *first_design, CDesign *first_pareto, double elapsed){

  /* Variables declaration */
  FILE *ptr_rpt_file = NULL;

  char report[MAX_CHARS_WORD];


  sprintf(report, "%s%s%s", "log_dse_", process, ".rpt");
  
  ptr_rpt_file = fopen(report, "at");
  if(ptr_rpt_file == NULL){
    printf("\nCannot create report file %s \n", report);
    exit(EXIT_FAILURE);
  } 

   printf("\n\nRunning time %0.4f\n", elapsed);
   fprintf(ptr_rpt_file,"\n\nRunning time %0.4f\n", elapsed);

   results_summary_design(first_pareto, ptr_rpt_file);
   results_summary_design(first_design, ptr_rpt_file);

   printf("\n\nEnd of DSE \n\n");
   fclose(ptr_rpt_file);


}

/****************************************************************
** 
** results_summary_designs
**
** Comments: 
**
****************************************************************/
void results_summary_design(CDesign *first_design,FILE *ptr_rpt_file){

  /* Variables declaration */
  CDesign *design=NULL;
  CAttr *attr= NULL;



   printf("\n\nExploration results summary :");
   fprintf(ptr_rpt_file,"\nExploration results summary (Optimal designs only):");
   fprintf(ptr_rpt_file,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^");

   printf("\n\tNAME\tDATE\tTIME\tAREA\tRAM[bits]\tSTATES\tLATENCY\tTYPE");
   fprintf(ptr_rpt_file,"\n\tNAME\tDATE\tTIME\tAREA\tRAM[bits]\tSTATES\tLATENCY\tTYPE"); 
   
   /* Write design details to the log file */
   for(design=first_design;design;design=design->m_nextp){


     if(design->m_area <= 0)
       continue;

       printf("\nDesign%d\t%s\t%s\t%d\t0\t%d\t%d", design->m_count, "5/5/2014", "22:12:22", design->m_area, design->m_latency+1,design->m_latency);


       if(design->m_count < 10)
	 fprintf(ptr_rpt_file,"\nDesign00%d\t%s\t%s\t%d\t0\t%d\t%d", design->m_count, "5/5/2014", "22:12:22", design->m_area, design->m_latency+1,design->m_latency);   
       else if(design->m_count < 100)
	 fprintf(ptr_rpt_file,"\nDesign0%d\t%s\t%s\t%d\t0\t%d\t%d", design->m_count, "5/5/2014", "22:12:22", design->m_area, design->m_latency+1,design->m_latency); 
       else
	 fprintf(ptr_rpt_file,"\nDesign%d\t%s\t%s\t%d\t0\t%d\t%d", design->m_count, "5/5/2014", "22:12:22", design->m_area, design->m_latency+1,design->m_latency); 

 

     if(design->m_first_attr == NULL)
       fprintf(ptr_rpt_file,"\tFU");
     else
       fprintf(ptr_rpt_file,"\tATTR");
     

     if(design->m_adjusted == true)
       fprintf(ptr_rpt_file,"\tAdjusted");

     if(design->m_interpolated == true)
       fprintf(ptr_rpt_file,"\tInterpolated");



   }


   fprintf(ptr_rpt_file,"\n\nInput File : %s", in_file);
   fprintf(ptr_rpt_file,"\n\n# Design attributes details");   

   /* Write design details to the log file */
   //Adam note: adjust result will not carry any Attributes files so design->m_first_attr =NULL
   for(design=first_design;design;design=design->m_nextp){

     if(design->m_first_attr == NULL)
       continue;

     /* Write design details to the log file */   
     fprintf(ptr_rpt_file,"\n\nDesign %d", design->m_count);   
     fprintf(ptr_rpt_file,"\nArea %d, Latency %d", design->m_area, design->m_latency);
     if(design->m_first_attr != NULL){
       for(attr=design->m_first_attr; attr; attr=attr->m_nextp){
	 fprintf(ptr_rpt_file,"\n%s\t%s", attr->m_name,attr->m_value );   	 
       }
     }
   }



}





/********************************************************
*********************************************************
** 
** delete_data
**
** Comments:frees the allocated memory of the DSE data structure
**
*********************************************************
*********************************************************/
void delete_data(CDesign *first_design, CAttrlist *first_attr_list){


  /* Delete the generated designs */
  delete_designs(first_design);


  /* Delete teh generated attribute list */
  delete_attr_list (first_attr_list);


}


/*************************
** 
** delete_designs
**
** Comments:frees the allocated memory for the generated designs during DSE
**
**************************/
void delete_designs(CDesign *first_design){

  CDesign *design = NULL, *design_next = NULL;


  for(design=first_design; design; design=design_next){

    design_next = design->m_nextp;

    if(design->m_first_attr != NULL)
      delete_attr(design->m_first_attr);
    
    free(design);


  }


  count_designs --;

}


/*************************
** 
** delete_attr_list
**
** Comments:frees the read attribute list
**
**************************/
void delete_attr_list(CAttrlist *first_attr_list){

  CAttrlist *attrlist = NULL, *attrlist_next = NULL;

  for(attrlist=first_attr_list; attrlist; attrlist=attrlist_next){

    attrlist_next = attrlist->m_nextp;

    if(attrlist->m_first_attr != NULL)
      delete_attr(attrlist->m_first_attr);

    if(attrlist->m_name != 0)free(attrlist->m_name);
    free(attrlist);
  }


}



/*************************
** 
** delete_attr
**
** Comments:frees attr nodes
**
**************************/
void delete_attr(CAttr *first_attr){

  CAttr *attr = NULL, *attr_next = NULL;

  for(attr=first_attr; attr; attr=attr_next){

    attr_next = attr->m_nextp;

    delete_single_attr(attr);

  }
}

/*************************
** 
** delete_single_attr
**
** Comments:frees singleattr nodes
**
**************************/
void delete_single_attr(CAttr *attr){

    if(attr->m_name != 0)free(attr->m_name);
    if(attr->m_value != 0)free(attr->m_value);
    if(attr != NULL)
      free(attr);

}
