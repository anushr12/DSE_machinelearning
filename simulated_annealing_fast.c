



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include "dse.h"

/*Global variables*/
bool fix_attr = false;
//bool no_synthesis = false;
float min_count[],max_count[];
CAttrlist *tree_list = NULL;
CAttrlist *tree_first_list = NULL;
CDesign *first_design  ;

CAttr *attr = NULL;
Croot_node *first_root = NULL;
Croot_node *root = NULL;
Ctree_node *child = NULL;



CDesign *simulated_annealing_f(CAttrlist *first_attr_list, char *heuristic , char *process,char *header , char *define_header, char *in_file, char *analyse_command , char *test_vector,int temperature,int cycles){

 int i;

 first_design = NULL;
 row = 0;
 
  attr_random_assign(first_attr_list);

   /*** to check the total possible designs ***/
  for( i=0; i != list_cnt ; i++)
    num_of_designs = num_of_designs * attr_cnt[i];
  

 if(temperature >= num_of_designs){
    printf("\n simulated annealing not possible, please enter correct temperature\n");
    return NULL;
  }


 new_design_s = (CDesign *) malloc(sizeof(CDesign));
 initialize_design();
  row_i=0;

    srand(time(NULL));
  rand_design_create(first_attr_list);/* the first design randomly created*/

  insert_design(new_design_s , countdes);/*first_design is obtained here*/
  countdes++;

  synthesize_design_f(new_design_s , heuristic,process ,header,define_header, in_file, analyse_command,test_vector,cycles );  /*synthesize and extract area, delay information  1 STEP*/

  global_cost_function =  (cost_function_calculate(new_design_s)) / 2000;  /* STEP 2 Scale down ranges to 0-10*/
  global_cost_function_prev = global_cost_function;      /* store to previous value*/
  min_area = first_design->m_area;
  min_latency = first_design->m_latency ; 



  annealing_algorithm_f(first_attr_list , heuristic, process, header,define_header, in_file , analyse_command , test_vector,cycles,temperature);

  printf("\n the no new design counter and temperature %d %d", no_new_des_cntr,temperature);

  prev_design_s=NULL;
  design_attr_s=NULL;

return first_design;
}


/** normal simulated annealing**/
void annealing_algorithm_f(CAttrlist *first_attr_list, char *heuristic , char *process,char *header , char *define_header, char *in_file, char *analyse_command,char *test_vector,int cycles,int temperature){

CDesign *desk = NULL;
CDattrdes *atr = NULL ;
CDattrdes  *atr1 = NULL;
float delta_cost_function;
double acceptance_prob_value;
float random_prob;
double func_value ;
int cntr , cntr1=0;
int  threshold;


 
  if(prev_design_s== NULL){

   
    prev_design_s = new_design_s;  /* transfer to previous design*/
  }



  new_design_s = (CDesign *)malloc(sizeof(CDesign)); /*malloc for new  design*/
  initialize_design();
  
  design_attr_s = prev_design_s->m_first_attr;

 
  rand_design_create(first_attr_list);/* the first design randomly created*/

/************ check if it is a unique design ************/
  for(desk = first_design ; desk != NULL ; desk = desk->m_nextp){
    cntr = 0;
    cntr1 = 0;
    for(atr = new_design_s->m_first_attr ,atr1 = desk->m_first_attr;atr != NULL,atr1 != NULL; atr=atr->m_nextp ,atr1=atr1->m_nextp ){
      cntr1++;
      if( (atr->d_name  == atr1->d_name) && (atr->d_value == atr1->d_value) )
	cntr++;
    }
      if(cntr ==cntr1) {
      new_design_s = NULL;
      new_design_s = (CDesign *)malloc(sizeof(CDesign)); /*malloc for new  design*/
      initialize_design();
      design_attr_s = prev_design_s->m_first_attr;
      rand_design_create(first_attr_list);
      desk = first_design;
     }
     else
      continue;
  }

  insert_design(new_design_s , countdes);

  countdes++;

  synthesize_design_f(new_design_s , heuristic,process ,header,define_header, in_file, analyse_command,test_vector,cycles );  /*synthesize and extract area, delay information  1 STEP*/


  global_cost_function =  (cost_function_calculate(new_design_s)) / 2000;  /* STEP 2 scaling down ranges to 0-10 */
 
  delta_cost_function = global_cost_function - global_cost_function_prev;  /**** the Delta function GCF2 - GCF1 *****/
 
  random_prob = (rand() / (float)RAND_MAX) ; /** state probability **/

  /**** Inner loop criterion*******/
	if(delta_cost_function < 0){   /*** the current design is better than the previous design ****/
	prev_design_s = new_design_s;
     	global_cost_function_prev = global_cost_function; /* store current cost function to prev cost function variable*/
	 if(no_new_des_cntr > 0) no_new_des_cntr=0;
   	}

 	else if(delta_cost_function > 0){
     		func_value  = -(delta_cost_function)/ temperature;
     		acceptance_prob_value = exp(func_value);
     		if( random_prob <= acceptance_prob_value ){
         	prev_design_s = new_design_s;
        	global_cost_function_prev = global_cost_function; /* store current cost function to prev C.F variable*/
		 if(no_new_des_cntr > 0) no_new_des_cntr=0;
      		}
     		else
		no_new_des_cntr++;
   	}
	else
	no_new_des_cntr++;	

    global_counter++; 




	if((new_design_s->m_area < (new_design_s->m_prevp)->m_area)&& (new_design_s->m_latency < (new_design_s->m_prevp)->m_latency)){   /*now check only for area*/
	  better_des_cntr++;
	  if(better_des_cntr %4 == 0)              /*decrease the temperature when the design becomes better for 6 iterations*/
	    temperature = 0.95* temperature;
     }




	if(countdes < (num_of_designs/2))
	  annealing_algorithm_f(first_attr_list , heuristic, process, header,define_header, in_file , analyse_command,test_vector,cycles,temperature);

else

{
  

  /******* Deciding pareto optimal designs ************/

  
     	for(desk = first_design ; desk != NULL ; desk = desk->m_nextp){
       		if(((desk->m_area < min_area) && (desk->m_latency > min_latency)) || ((desk->m_area > min_area) && (desk->m_latency < min_latency)))
	 	
		desk->m_pareto_optimal = true;

		else
		  { if((( desk->m_area >= min_area) && (desk->m_latency > min_latency)) || (( desk->m_area > min_area) && (desk->m_latency >= min_latency)))

	 		desk->m_pareto_optimal = false;
         		else

	   		{min_area = desk->m_area;
	   		min_latency = desk->m_latency;
	   		desk->m_pareto_optimal = true;
	   		}
		}
     	}


  
	//	  sorting_list();

/** list sorted, so the first design has the minimum area / latency **/

//	threshold = first_design->m_latency + (first_design->m_latency/5);

	for(desk = first_design ; desk != NULL; desk = desk->m_nextp){
	  if(desk->m_pareto_optimal==true)
	    desk->m_hash = true;
	  else
	    desk->m_hash = false;
                                        
	}


	create_decisiontree(first_design, first_attr_list , NULL);

	parse_tree();

	delete_tree();
	
	fast_annealing(first_attr_list , heuristic, process, header,define_header, in_file , analyse_command,test_vector,cycles,temperature);
	
	print_to_file(process);
	
	delete_treelist();
 }
}





void fast_annealing(CAttrlist *first_attr_list, char *heuristic , char *process,char *header , char *define_header, char *in_file, char *analyse_command,char *test_vector,int cycles,int temperature){

float delta_cost_function;
double acceptance_prob_value;
float random_prob;
double func_value ;
CDesign *desk = NULL;
CDattrdes *atr = NULL ;
CDattrdes  *atr1 = NULL;
int cntr,cntr1=0, trials =0;


	new_design_s = (CDesign *) malloc(sizeof(CDesign));
	initialize_design();

	design_attr_s = prev_design_s->m_first_attr;

	rand_design_create(first_attr_list);
	fix_attr_from_tree();

	 /************ check if it is a unique design ************/
  	for(desk = first_design ; desk != NULL ; desk = desk->m_nextp){
		cntr = 0;
		cntr1 = 0;
	for(atr = new_design_s->m_first_attr ,atr1 = desk->m_first_attr;atr != NULL,atr1 != NULL; atr=atr->m_nextp ,atr1=atr1->m_nextp ){
			cntr1++;
			if( (atr->d_name  == atr1->d_name) && (atr->d_value == atr1->d_value) )
			cntr++;
		 }
		if(cntr ==cntr1) {
		trials++;
		if(trials == num_of_designs) break;
		new_design_s = NULL;
		new_design_s = (CDesign *)malloc(sizeof(CDesign)); /*malloc for new  design*/
		design_attr_s = prev_design_s->m_first_attr;
		initialize_design();
		rand_design_create(first_attr_list);
		fix_attr_from_tree();
		desk = first_design;
		
		}
		else
		continue;
	}

	if(trials < num_of_designs){
	insert_design(new_design_s , countdes);

	countdes++;

	//	if(no_synthesis == false)

	synthesize_design_f(new_design_s , heuristic,process ,header,define_header, in_file, analyse_command,test_vector,cycles );  /*synthesize and extract area, delay information  1 STEP*/
	
/* 	else */
/* 	  { */
/* 	    new_design_s->m_area = first_design->m_area; */
/* 	    new_design_s->m_latency = first_design->m_latency; */
/* 	  } */
       

/* 	no_synthesis = false; */

	global_cost_function =  (cost_function_calculate(new_design_s)) / 2000;  /* STEP 2 scaling down ranges to 0-10 */
   

	delta_cost_function = global_cost_function - global_cost_function_prev;  /**** the Delta function GCF2 - GCF1 *****/

	random_prob = (rand() / (float)RAND_MAX) ; /** state probability **/

/**** Inner loop criterion*******/
	if(delta_cost_function < 0){   /*** the current design is better than the previous design ****/
		prev_design_s = new_design_s;
		global_cost_function_prev = global_cost_function; /* store current cost function to prev C.F variable*/
		 if(no_new_des_cntr > 0) no_new_des_cntr=0;
	}

	else if(delta_cost_function > 0){
		func_value  = -(delta_cost_function)/ temperature;
		acceptance_prob_value = exp(func_value);
		if( random_prob <= acceptance_prob_value ){
			prev_design_s = new_design_s;
			global_cost_function_prev = global_cost_function; /* store current cost function to prev C.F variable*/
			 if(no_new_des_cntr > 0) no_new_des_cntr=0;
		}
		else
		no_new_des_cntr++;
	}
	else
	no_new_des_cntr++;   

	global_counter++; 

	if((new_design_s->m_area < (new_design_s->m_prevp)->m_area)&&(new_design_s->m_latency < (new_design_s->m_prevp)->m_latency)){
		better_des_cntr++;
		if(better_des_cntr %4 == 0)              /*decrease the temperature when the design becomes better for 6 iterations*/
		temperature = 0.95* temperature;
	}

	 if(no_new_des_cntr < 15 && temperature > 0 )
	   fast_annealing(	first_attr_list , heuristic, process, header,define_header, in_file , analyse_command,test_vector,cycles,temperature);	
	}
	else
	return;
}



void fix_attr_from_tree(void){

  bool check=false,flag = false;
  int count = 0 ,midpoint, i=0;
  //float sum=0;
  CAttrlist *head = tree_first_list;
  CDattrdes *atr = new_design_s->m_first_attr;
  CAttr *at = NULL;
  // float sq_values = 0;

  while(head){
    count++;
    
    head = head->m_nextp;
  }


  

  if(count % 2 == 0)
    midpoint = count / 2;
  else
    midpoint = (count/2) + 1;

  head = tree_first_list;

  while(i< midpoint && atr){
    at = head->m_first_attr;
   /*  if( at->entropy == 0) flag = true; */
/*     else */
/*       if((at->pos/ (at->pos + at->neg))>0.5) */
/* 	flag = true; */
/*       else */
/* 	{flag = false;check = true;} */

    if(((at->m_list)->m_name == (atr->d_list)->m_name) ){
      
      if(at->m_name == atr->d_name && at->m_name == atr->d_name)
	{
	  check = true;
	  // no_synthesis = true;
	}
      else
	{
	  atr->d_name = at->m_name;
	  atr->d_value = at->m_value;
	  check = true;
	  //	no_synthesis = false;
	}
    }
    	
    
    else
      atr = atr->m_nextp;
    if(check == true)
      {
	head = head->m_nextp;
	i++;
	atr = new_design_s->m_first_attr;
	check = false;
	
      }
  }
  
}


void initialize_design(void){

new_design_s->m_count = 0;
new_design_s->m_pareto_optimal = false;
new_design_s->m_area = 0;
new_design_s->m_latency = 0;
new_design_s->m_first_attr = NULL;
new_design_s->m_prevp = NULL;
new_design_s->m_nextp = NULL;
}




void parse_tree(void){
 
  bool flag=false,child_flag = false;
  float value=0.0 , entropy;
  int i,j;
  Croot_node *head = first_root;
 Croot_node *tmp = NULL;
 // Croot_node *tail = root;
 Ctree_node *child = NULL, *child_tmp=NULL;
 CAttr *list_atr = NULL;
 // int midpoint,nodecount=0;

 




 while(head){
	child = head->children[0];
	tree_list = (CAttrlist *)malloc(sizeof(CAttrlist));
	tree_list->m_name = head->t_name;
        tree_list->m_nextp = NULL;
	i=0 ; j=0;
	while(child){
	  if(child->area_small == true){
	    child_tmp = child;
	    
	    flag = true;
	  
	    
	  }
	  if (child->childp && child->area_small == false)
	    {
	      value = child->c_pos / (child->c_pos + child->c_neg);
	      if( value < 0.5)
		{ max_count[j] = child->c_entropy;
		  j++;
		}
	      else
		{ min_count[i] = child->c_entropy;
		  i++;
		}
	      
	      //  tmp = child->childp;
	      //child_tmp = child;
	      child_flag = true;
	    }
	       
	  child = child->nextchild;
	}
	if(child_flag == true)
	  {
	     if(j>i)
	       entropy = min_entropy_cal(max_count , j , true);
	     else
	       entropy = min_entropy_cal(min_count , i , false);

	     child = head->children[0];
	     while(child){
	       if(child->c_entropy == entropy)
		 { tmp = child->childp; break;}
	       else
		 child = child->nextchild;
	     }
	     if(flag == false)
	       child_tmp = child;
	     
	  }
	if( child_tmp){
	  
	  list_atr = (CAttr *)malloc(sizeof(CAttr));
	  list_atr->m_name = child_tmp->c_name;
	  list_atr->m_value = child_tmp->c_value;
	  list_atr->m_list = child_tmp->list;
	  list_atr->entropy = child_tmp->c_entropy;
	  list_atr->pos = child_tmp->c_pos;
	  list_atr->neg = child_tmp->c_neg;
	  tree_list->m_first_attr = list_atr;
	  if( !tree_first_list) tree_first_list = tree_list;
	  else insert_attr_list(tree_first_list, tree_list);
	
	}
	else
	  free(tree_list);
	flag = false;
	child_flag = false;
	head = tmp;
	tmp = NULL;
       	child_tmp = NULL;
 	}
	
		
		
}		
 

void delete_treelist(void){


CAttrlist *list_next = NULL;

while(tree_first_list){
	list_next = tree_first_list->m_nextp;
	(tree_first_list->m_first_attr)->m_name = NULL;
	(tree_first_list->m_first_attr)->m_value = NULL;
	free(tree_first_list->m_first_attr);
	tree_first_list->m_name = NULL;
	free(tree_first_list);
	tree_first_list = list_next;
}
}	





/*** delete the decision tree***/
void delete_tree(void){
  
 
 Ctree_node *child=NULL , *child_next=NULL;
 Croot_node *rtemp = NULL;
 Croot_node *r_node = root;
 
   while(r_node){
       child = r_node->children[0];
	while(child){
		child_next = child->nextchild;
		delete_node(child , NULL);
		child = child_next;
	}
        if(r_node == first_root) rtemp = NULL;
	else
	rtemp = (r_node->parentp)->nodeparent;
        delete_node(NULL, r_node);
	r_node = rtemp;
    }

}
	

/*** delete the nodes of the tree ***/
void delete_node( Ctree_node *child, Croot_node *r_node){
  
	if(child)
	{
	 if(child->c_name != 0) child->c_name = NULL;
	 if(child->c_value != 0) child->c_value = NULL;
	 if(child->childp) child->childp = NULL;
	 if(child->nodeparent) child->nodeparent = NULL;
	 if(child->nextchild) child->nextchild = NULL;
	 if(child->list) child->list = NULL;
	 free(child);
	}
	else
	if(r_node && !child)
	{
	 if(r_node->t_name) r_node->t_name = NULL;
	 if(r_node->parentp)r_node->parentp = NULL;
	 free(r_node->children);
	 r_node->children = NULL;
	 
	 
	}
 }





/*** print all the designs to file ******/

void print_to_file(char *process){
  
  char dir[50];
  int chk;
  FILE *ptr_f = NULL;
CDattrdes *attr=NULL;
  CDesign *design = NULL;
  snprintf(dir, sizeof(dir), "/home/anushree1/anushree1/DSE_anushree");
  chk = chdir(dir);
if(chk == 0)
printf("\n the directory has been changed\n");
else
{printf("\n directory not changed\n");
exit(0);
}
	dir[0] = '\0';

snprintf(dir, sizeof(dir), "%s_designs_latency.rpt",process);

  ptr_f = fopen(dir, "wt");
  if(ptr_f == NULL){
    printf("\n cannot create report file\n");
    exit(EXIT_FAILURE);}

  fprintf(ptr_f, "\n the process is %s\n", process);
  
  for(design = first_design; design; design = design->m_nextp){
    fprintf(ptr_f, "\n Design %d \t %d \t %d \t", design->m_count, design->m_area, design->m_latency);
  }

  for(design = first_design;design; design = design->m_nextp){
    fprintf(ptr_f, "\nDesign %d", design->m_count); 
    if(design->m_first_attr != NULL){
      fprintf(ptr_f, "\n");
     for(attr=design->m_first_attr; attr; attr=attr->m_nextp)
	 fprintf(ptr_f,"\n%s\t%s", attr->d_name,attr->d_value );   	 
    }
  }
  fclose(ptr_f);

dir[0] = '\0';
}




/** parse and synthesize the selected design */
void synthesize_design_f(CDesign *design ,char *heuristic , char *process, char *header, char *define_header, char *in_file, char *analyse_command,char *test_vector,int cycles ){

 int ck,lines = 0; 
  char ch;
  char file_synth_r[300];
  char file_read[25];
  char line[MAX_CHARS_WORD];
  FILE *data_file = NULL;

  int c =0 ,d=0;
  printf("\n the heuristic %s\n" , heuristic);

  snprintf(directory_s, sizeof(directory_s), "/home/anushree1/anushree1/DSE_anushree/%s", heuristic);
  ck = mkdir(directory_s , S_IRWXU | S_IRWXG | S_IXUSR);
  if(ck == 0)
    printf("\n the folder created\n");
  else
    printf("\n the folder not created\n");
  directory_s[0]='\0';

  snprintf(directory_s , sizeof(directory_s),"/home/anushree1/anushree1/DSE_anushree/%s/%s", heuristic , process);
  ck = mkdir(directory_s , S_IRWXU | S_IRWXG | S_IXUSR);
  if(ck == 0)
    printf("\n the sub- folder created again\n");

  directory_s[0] = '\0';


    snprintf(directory_s, sizeof(directory_s), "/home/anushree1/anushree1/DSE_anushree/%s/%s/design_%d",heuristic,process , design_num);
    create_headerf(directory_s , design);
directory_s[0]='\0';
if(getcwd(directory_s, sizeof(directory_s)) != NULL)
       printf("\n the current directory%s\n", directory_s);
    directory_s[0] = '\0';
    snprintf(directory_s , sizeof(directory_s), "cp %s .",in_file);
    system(directory_s);
  directory_s[0] = '\0';

  if(header != NULL){
    snprintf(directory_s , sizeof(directory_s), "cp %s ." , header);
    system(directory_s);
  }

 directory_s[0] = '\0';

   if(define_header != NULL){
    snprintf(directory_s , sizeof(directory_s), "cp %s ." , define_header);
    system(directory_s);
  }

 directory_s[0] = '\0';

 /*********** parsing the C file ********************/
 // if(design == first_design){
   snprintf(directory_s, sizeof(directory_s),"%s %s", analyse_command,in_file);
   // }
 printf("\n the parsing command %s\n", directory_s);
   system(directory_s);

 /****** generating FU constraint file********/
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -EE -lfl /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.BLIB %s.IFF -Zflib_fcnt_out -Zflib_out_limit=L50:M100:S100 -s",process);
  system(file_synth_r);
  file_synth_r[0] = '\0';

 /*********generating memory constraint file**********/  
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -c1000 -Zmem_kind=RW1 -s -EE -lfl /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.BLIB %s.IFF -lfc %s-auto.FCNT -Zmlib_mcnt_out",process,process);
  system(file_synth_r);
 file_synth_r[0] = '\0';

 /****** the synthesizing command ************/
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -c1000 -s -Zport_valid_sig_gen -tcio -EE -lfl /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber_540/LINUX/lib/asic_45.BLIB -lfc %s-auto.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT %s.IFF",process,process,process,process);
 system(file_synth_r);
 file_synth_r[0]='\0';

 if(test_vector != NULL){

/******* simulating the design ***************/
 snprintf(file_synth_r,sizeof(file_synth_r),"cmscgen -EE -input=random:transaction -file_out=transaction %s_C.IFF",process);
 system(file_synth_r);
 file_synth_r[0]='\0';
 snprintf(file_synth_r,sizeof(file_synth_r),"mkmfsim -EE -GNU -cycle %d -target sim_cycle.exe %s_C.MakeInfo",cycles,process);
 system(file_synth_r);
 file_synth_r[0]='\0';
 snprintf(file_synth_r,sizeof(file_synth_r),"make -f Makefile.GNU");
 system(file_synth_r);
 file_synth_r[0]='\0';
 snprintf(file_synth_r,sizeof(file_synth_r),"./sim_cycle.exe");
 system(file_synth_r);
 file_synth_r[0]='\0';
 

 snprintf(file_read, sizeof(file_read),"%s.tlv",test_vector);
 data_file=fopen(file_read, "r");
 while( !feof(data_file)){
	ch = fgetc(data_file);
	if(ch == '\n')
	lines++;
 }
file_read[0] = '\0';
printf("\n the number of lines %d\n",lines);

if(lines !=0)
design->m_latency = cycles/lines;
 }

 snprintf(file_read , sizeof(file_read), "%s.CSV", process);
 data_file = fopen(file_read , "r");
  while(fgets(line, sizeof(line), data_file) != NULL)
   { 
     do{
       if(c==0){
       fscanf(data_file , "%d" , &design->m_area);
       printf("area %d\n", design->m_area);}
        fscanf(data_file, "%d", &d);
	

	if(c ==8&&lines==0 )
	design->m_latency = d;
       c++;
     }while(fgetc(data_file) != EOF);
    }

 fclose(data_file);
 design_num++;

}


/**create the decision tree based on simulated designs**/
void create_decisiontree(CDesign *design, CAttrlist *first_list, Ctree_node *node){

  char *name;
  float min_ent , value;
  int i,j ;
  bool roots_done , flag;
  CAttrlist *head = first_list;
  CAttrlist *list_t = NULL ;
  CDesign *des_head = design;
  Croot_node *rt = NULL;
  Ctree_node *prev_child = NULL;




  /* if( first_root) */
/*     { */
/*       if(node->c_pos == 0 ||node->c_neg == 0 ) */
/* 	{if( !node->c_pos) */
/* 	    node->area_small = false; */
/* 	  else */
/* 	    if(node->c_neg == 0) */
/* 	      node->area_small = true; */
/* 	  node->childp = NULL; */
/* 	  node->leaf_node = true; */
/* 	  create_decisiontree(des_head, first_list, node->nextchild); */
/* 	} */
/*       else */
/* 	node->leaf_node = false; */
      
/*     } */
 

  if( !first_root)
    calculate_entropy_gain(first_list,des_head, NULL, 1);
  else
    calculate_entropy_gain(first_list,des_head, node, 2);
  
  
  
  first_list = head;
  
  /*** decide the root node **/
  while(first_list){
    if(first_list->node_done == false)
      {roots_done = false; break;}
    else roots_done = true;
    first_list = first_list->m_nextp;
  }
  
  if(roots_done == false){
    
    
    list_t = first_list;
    first_list = head;
    while(first_list->m_nextp){
      if(first_list->node_done == false){
	first_list = first_list->m_nextp;
	
	if(first_list->gain > list_t->gain){
	  name = first_list->m_name;
	  list_t = first_list;   /**this is the decided list**/
	  
	}
      }
      else
	
	first_list = first_list->m_nextp;
    }
    
    
    
    first_list = head;
    
    list_t->node_done = true;
    
    root = (Croot_node *)malloc(sizeof(Croot_node));
    
    if(first_root == NULL)  create_rootnode(name,list_t , NULL);
    
    else
      {
	prev_child = node;
	create_rootnode(name, list_t, prev_child);
      }
    
    node = NULL;
    
    
    rt = root;
    flag = false;
    i=0 , j=0;
    while(rt){
      child = rt->children[0];
      while(child){
	if(child->leaf_node == false && child->childp == NULL)
	  { if((child->c_pos != child->c_neg)) 
	      { value = child->c_pos / (child->c_pos + child->c_neg);
		if(value < 0.5)
		  {  max_count[j] = child->c_entropy;
		    j++;
		  }
		else
		  { min_count[i] = child->c_entropy;
		    i++;
		  }
		flag = true;
		// node = child; flag = true; break;
	      }
	  }
	child = child->nextchild;
      }
      if(flag == true)
	{
	  if( j > i)
	    min_ent = min_entropy_cal(max_count , j , true);
	  else
	    min_ent = min_entropy_cal(min_count , i, false);
	  
	  child = rt->children[0];
	  while(child){
	    if(child->c_entropy == min_ent)
	      { node = child; break; }
	    else
	      child = child->nextchild;
	  }
	  break;
	}
      else

	if(rt == first_root) rt = NULL;
	else
	  rt = (rt->parentp)->nodeparent;
      
      
    }
    
    if( node || flag == true)
      create_decisiontree(design,first_list,node);
   
   
   
 }
 else return;
 
 
}



/* to calculate the minimum entropy of the attributes */
float min_entropy_cal( float *count , int num , bool cond){
  int k;
  float min = count[0];

  if(cond == false){
    for( k=1; k < num ; k++){
      if((count[k] < min) && count[k]!= 0.0)
	min = count[k];
    }
  }
  else
    if(cond == true){
      for( k=1; k < num ; k++){
	if((count[k] > min) && count[k]!= 0.0)
	  min = count[k];
      }
    }


  return min;

}
 






/* search for the attribute or node in the design**/
bool check_attri(CDesign *des, CAttr *attr , Ctree_node *node){
  
  bool check;
  CDattrdes *des_at = NULL;
  Ctree_node *node_p = NULL;
  
  if(node == NULL){
    for(des_at =des->m_first_attr; des_at; des_at=des_at->m_nextp){
      
      if(des_at->d_name == attr->m_name && des_at->d_value ==attr->m_value)
	{ check = true;
	  break;}
      else
	check = false;
    }
    
  }
  
  else
    if(attr == NULL){
      for(des_at =des->m_first_attr; des_at; des_at=des_at->m_nextp){
	if(des_at->d_name == node->c_name && des_at->d_value ==node->c_value)
	  { check = true; break;}
	else
	  check = false;
      }
      
    }
  
    else{
      
      node_p = node;
      des_at = des->m_first_attr;
      
      while(node_p){
	while(des_at){
	  if(des_at->d_name == node_p->c_name && des_at->d_value ==node_p->c_value)
	    { check = true;break;}
	  else
	    check = false;
	  des_at = des_at->m_nextp;
	}
	if(check == false) break;
	des_at = des->m_first_attr;
	node_p = (node_p->nodeparent)->parentp;
	
      }
      
      
      if(check){
	des_at = des->m_first_attr;
	while(des_at){
	  if(des_at->d_name == attr->m_name && des_at->d_value == attr->m_value)
	    {check = true;break;}
	  else
	    check = false;
	  des_at = des_at->m_nextp;
	}
      }
      
      
    }
  return check;
}



/** calculate the logarithm(log x/ log 2)**/
inline long double logar(long double x){
  
  return log(x) * M_CONST_E;
}

 

  /** to create a root node for each previous child and first root**/

void create_rootnode( char *name, CAttrlist *list_t, Ctree_node *prev_child){
  
  if(first_root == NULL){
    first_root = root;
    first_root->t_name = name;
    create_child(list_t);
    
    first_root->parentp = NULL;
  }
  else
    {
      root->t_name = list_t->m_name;
      create_child(list_t);
      root->parentp = prev_child;
      prev_child->childp = root;
    }
  
}



/** creating children for the root**/

 void create_child(CAttrlist *list_t){

   int i =0, cnt =0;
   CAttr *att = list_t->m_first_attr;
   
   while(att)
     {cnt++;
       att = att->m_nextp;
     }
   att = list_t->m_first_attr;
   root->children  = (Ctree_node **)malloc(sizeof(Ctree_node *) * cnt);
   while(att){
     root->children[i] = (Ctree_node *)malloc(sizeof(Ctree_node));
     
     insert_treenode(i , att);
     //	printf("\n the pos & neg value %f %f\n", root->children[i]->c_pos,root->children[i]->c_neg);
     if( i != 0)
       ( root->children[i-1])->nextchild =  root->children[i];
     att = att->m_nextp;
     i++;
   }
   
 }


/** inserting child node ***/

void insert_treenode( int i , CAttr *att){
  
 (root->children[i])->nodeparent = root;
 (root->children[i])->childp = NULL;
 (root->children[i])->c_name = att->m_name;
 (root->children[i])->c_value = att->m_value;
 (root->children[i])->c_pos = att->pos;
 (root->children[i])->c_neg = att->neg;
 (root->children[i])->c_entropy = att->entropy;
 (root->children[i])->list = att->m_list;
 if( !(root->children[i])->c_pos || !(root->children[i])->c_neg)
   (root->children[i])->leaf_node = true;
 else (root->children[i])->leaf_node = false;
 if(root->children[i]->c_pos == 0 ||(root->children[i]->c_pos && root->children[i]->c_neg))
   root->children[i]->area_small = false;
 if(root->children[i]->c_neg == 0) root->children[i]->area_small = true;
 (root->children[i])->nextchild = NULL;
 
}
    
 
     

/** to calculate entropy for each list attribute from the designs **/

void calculate_entropy_gain(CAttrlist *first_list, CDesign *des_head, Ctree_node *node , int ch){

  int pos=0,neg=0;
  double entropy_s =0.0,entropy=0.0;
  float total_posnum = 0.0;
  float total_negnum = 0.0;
  float count_num = 0.0;
  CDesign *des_h = des_head;
  float tem;
  CAttr *attr = NULL;
  
  
  switch(ch)
    {
      
    case 1:
      
      
      while(des_h){
	if(des_h->m_hash)
	  total_posnum++;
	else
	  total_negnum++;
	count_num++;
	des_h = des_h->m_nextp;
      }
      /*calculate total information gain*/
      entropy_s = -(total_posnum/count_num)*(logar(total_posnum/count_num)) - ((total_negnum/count_num) * (logar(total_negnum/count_num)));
      
      des_h = des_head;
      while( first_list){
	
	attr = first_list->m_first_attr;
	while(des_h && attr){
	  
	  
	  if( check_attri(des_h,attr , NULL)){
	    if(des_h->m_hash){
	      pos++;
	      attr->pos= pos;
	    }
	    else{
	      neg++;
	      attr->neg=neg;
	    }
            
	  }
          
	  des_h = des_h->m_nextp;
	  if(des_h == NULL)
	    {  attr = attr->m_nextp;
	      pos = 0.0,neg =0.0;
	      des_h = des_head;
	    }
	  
	}
	
	attr = first_list->m_first_attr;
	while(attr){
	  if( attr->pos && attr->neg){
	    tem = -(attr->pos/(attr->pos+attr->neg)) * (logar(attr->pos/(attr->pos+attr->neg))) - (attr->neg/(attr->pos+attr->neg))* (logar(attr->neg/(attr->pos+attr->neg)));
	    entropy = entropy + ((attr->pos + attr->neg)/count_num)* tem;
	    
	    attr->entropy = -((attr->pos/count_num)*(logar(attr->pos/count_num))) - ((attr->neg/count_num)*(logar(attr->neg/count_num)));
          }
	  else
	    attr->entropy = 0.0;
	  
	  if(entropy > 1) entropy = 1.0;
	  
	  attr = attr->m_nextp;
        }
  	first_list->gain = entropy_s - entropy;
  	entropy = 0.0;
 	des_h = des_head;
     	first_list = first_list->m_nextp;
      }/* outer while loop*/
      
      break;
      
      
    case 2: 
      
      while(des_h){
	if(check_attri(des_h, NULL, node)){
	  if(des_h->m_hash)
	    total_posnum++;
	  else total_negnum++;
	  count_num++;
	}
	des_h = des_h->m_nextp;
      }
      
      entropy_s = -(total_posnum/count_num)*(logar(total_posnum/count_num)) - ((total_negnum/count_num) * (logar(total_negnum/count_num)));
      des_h = des_head;
      
      
      while( first_list){
	if( first_list->node_done==false)
          {
	    attr = first_list->m_first_attr;
            attr->pos = 0.0; attr->neg = 0.0;
	    
	    while(des_h && attr){
	      
	      if( check_attri(des_h,attr , node)){
		if(des_h->m_hash){
		  pos++;
		  attr->pos = pos;
		}
		else
		  {
		    neg++;
		    attr->neg=neg;
		  }
	      } //if loop
	      
	      des_h = des_h->m_nextp;
	      if(des_h == NULL)
		{  attr = attr->m_nextp;
		  if(attr){ attr->pos = 0.0; attr->neg = 0.0;}
		  pos = 0.0,neg =0.0;
                  des_h = des_head;
                }
	    }
            attr = first_list->m_first_attr;
	    
	    while(attr){
	      if(attr->pos && attr->neg){
		tem =  -(attr->pos/(attr->pos+attr->neg)) * (logar(attr->pos/(attr->pos+attr->neg))) - (attr->neg/(attr->pos+attr->neg))* (logar(attr->neg/(attr->pos+attr->neg)));
		attr->entropy = tem;
	      }
	      else
		{ 
		  tem = 0.0; attr->entropy = 0.0;
		}
	      
	      entropy = entropy + ((attr->pos + attr->neg)/count_num)* tem;
	      
	      if(entropy > 1) entropy = 1.0;
	      
	      attr = attr->m_nextp;
	    }
	    first_list->gain = entropy_s - entropy;
	    entropy = 0.0;
	  }
	else first_list->gain = 0.0;
	des_h = des_head;
	first_list = first_list->m_nextp;
      } /* outer while loop*/
      
      break;
      
    }
  
}









void sorting_list(void){  

  CDesign *tempnode = NULL;
  CDesign *head = NULL;
  CDesign *temp = NULL;
  
  
  int j, flag=0;
  head = first_design;
  
  for(j= 0 ;j<countdes ; j++){
    
    while(head->m_nextp){
      
      tempnode = head->m_nextp;
      if( head->m_fitness < tempnode->m_fitness){
	
	flag = 1;
	if(head->m_prevp)
	  temp = head->m_prevp;
	else
	  temp = NULL;
	head->m_nextp = tempnode->m_nextp;
	head->m_prevp = tempnode;
	tempnode->m_nextp = head;
	tempnode->m_prevp = temp;
	if(head->m_nextp != NULL)
	  (head->m_nextp)->m_prevp = head;
	if(tempnode->m_prevp!=NULL)
	  (tempnode->m_prevp)->m_nextp = tempnode;
	
	if(first_design == temp)
	  first_design->m_nextp = tempnode;
	
	if(head == first_design)
	  first_design = tempnode;
	
	
      }
      else
	
	head = head->m_nextp;
    } 
    if(flag == 0)
      break;
    else
      {
	head = first_design;
	flag = 0;
      }
  } 
  
}
