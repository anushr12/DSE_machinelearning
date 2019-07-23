

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


CDesign *genetic_algorithm( CAttrlist *first_attr_list , char *heuristic,char *process, char *header, char *define_header, char *in_file, char *analyse_command, char *test_vector,int cycles,int num_of_pop,int iterate_num){

  
  int i,cnt=0,iteration_cnt=0;
  CDesign *tmp = NULL;
  CDesign *design=NULL, *first=NULL;
  float sum_of_fitness=0.0,sum_of_prob=0.0, max_cost,sum=0.0;
  float y;
  
  first_design_pop1=NULL;
  prev_design_s=NULL;
  design_attr_s=NULL;
  first_design = NULL;
  row = 0;

  attr_random_assign(first_attr_list);
  
  /*** to check the total possible designs ***/
  for( i=0; i != list_cnt ; i++)
    num_of_designs = num_of_designs * attr_cnt[i];

  srand(time(NULL));

  // synthesize and extract cost of the designs in population 

  for( i = 0; i < num_of_pop;i++){
    
   
    prev_design_s = new_design_s;
    if(prev_design_s != NULL)
    design_attr_s = prev_design_s->m_first_attr;
      

    new_design_s = (CDesign *) malloc(sizeof(CDesign));
    initialize_design();
    row_i = 0;
    
    rand_design_create(first_attr_list);  
    insert_design(new_design_s, countdes); 
    countdes++;
     synthesize_design(new_design_s , heuristic,process ,header,define_header, in_file, analyse_command,test_vector,cycles );  /*synthesize and extract area, delay information  1 STEP*/
     new_design_s->m_cost_value  = (alpha * new_design_s->m_area) + (beta * new_design_s->m_latency);
  
      
  }

  max_cost = calculate_max_cost();  
  design = first_design;
  while(design){
    sum = sum + (max_cost - design->m_cost_value);
    design = design->m_nextp;
  }

  design = first_design;
  while(design){
    
  //calculate fitness value
    design->m_fitness = (max_cost - design->m_cost_value)/sum; 
    sum_of_fitness = sum_of_fitness + design->m_fitness;
    design = design->m_nextp;
  }
  design = first_design;

  while(design){
    design->m_prob = sum_of_prob + design->m_fitness / sum_of_fitness;
     sum_of_prob += design->m_prob;
     design = design->m_nextp;
  }
  
  //select the 'x' best designs to copy it to next population
  sorting_list();
  //select midpoint of the designs and divide into two sets
  
  
  // divide_pop(num_of_pop,first_design);
  
  //first = (CDesign *)malloc(sizeof(CDesign));
  //assign_design_value( first_design_pop2,first);
  tmp=first_design;

  while(tmp){
    cnt++;
    tmp = tmp->m_nextp;
  }
  
  crossover(first_design,cnt);
  
  //mutate all designs in the original population
  mutate_designs(first_attr_list,first_design);
  
  //synthesize designs in pop1
  design = first_design_pop1;
  while(design){
    synthesize_design(design , heuristic,process ,header,define_header, in_file, analyse_command,test_vector,cycles );  /*synthesize and extract area, delay information  1 STEP*/
    design->m_cost_value  = (alpha * design->m_area) + (beta * design->m_latency);

  //pop1 is the changed design set from crossover and now merge with the original pop set
  
  design = first_design;
  while(design->m_nextp)
    design = design->m_nextp;

  design->m_nextp = first_design_pop1;
  first_design_pop1->m_prevp = design; 
  
   design = first_design;
   num_of_pop=0;
   while(design){
     num_of_pop++;
     design = design->m_nextp;
   }
   
   iteration_cnt++;
   if(iteration_cnt < iterate_num)
     first_design = genetic_algorithm(first_attr_list, heuristic,process,header,define_header,in_file,analyse_command,test_vector,cycles,num_of_pop,iterate_num);
   else
     return first_design;
   
     
  
    
}


float calculate_max_cost(void){
  
  CDesign *design = first_design;
  float max_cost = first_design->m_cost_value;

  while(design->m_nextp){
    design = design->m_nextp;
    if(design->m_cost_value > max_cost){
      max_cost = design->m_cost_value;
    }
  }

  design=NULL;

  return max_cost;
}




 void mutate_designs(CAttrlist *attr_list, CDesign *first_design){
   
   int i=0;
   double prob;
   CDesign *tmp = NULL;
   
   
   int mutation_rate = 0.10 * num_of_designs;
   tmp = first_design;
   while(tmp){
     
      prob = rand() / ((double)RAND_MAX);
      if(prob < 0.10 && i<mutation_rate){
	flip_attribute(attr_list,first_design);
       
      }
      else if(i>mutation_rate)
	break;
      i++;
      tmp = tmp->m_nextp;
   }
	
 }
   
   
  

 void flip_attribute(CAttrlist *attr_list, CDesign *first_design){
   
   CAttr *attr=NULL;
   CDattrdes *at = NULL;
   CAttrlist *list = NULL;
   CDesign *des=NULL;
   
   des = first_design;
   list = attr_list;
   
   while(des && list){
     attr = list->m_first_attr;
     at = des->m_first_attr;
     while(at && attr){
       if(at->d_name==attr->m_name && at->d_value==attr->m_value){
	 if(attr->m_nextp == NULL){
	   at->d_name = list->m_first_attr->m_name;
	   at->d_value = list->m_first_attr->m_value;
	 }
	 else
	   {  at->d_name = attr->m_nextp->m_name;
	      at->d_value = attr->m_nextp->m_value;
	   }
       }
       at = at->m_nextp;
       attr = attr->m_nextp;
     }
     des = des->m_nextp;
     list = list->m_nextp;
   }
 }
	 
   
  
  
  
void crossover(CDesign *first_design,int cnt){

  // first always select first partner as the first design of the pop2 and select second partner randomly from the pop2.

  int cnt1=0,i,counter=0;
  int cnt=0;
  double prob;
  CDattrdes *at = NULL;
  CDesign *parent1 =NULL,des_new=NULL; //select randomly
  CDesign *parent2 = NULL;
  CDesign *tmp_t=NULL;
  CDesign *tmp=NULL;
  bool chk = false;
  
  at = first_design->m_first_attr;
  while(at){
    cnt1++;
    at = at->m_nextp;
  }

  tmp = first_design;
  while(tmp){
    
     prob = rand() / ((double)RAND_MAX);
     
     if(prob < 0.25){
       des_new = (CDesign *)malloc(sizeof(CDesign));
       assign_design_value(tmp,des_new);
       if(first_design_pop2 == NULL)
	 first_design_pop2 = des_new;
       else
	 insert_design_s(des_new,cnt); 
     }
     cnt++;
     tmp = tmp->m_nextp;
  }
  
  

  tmp_t = first_design_pop2;
  tmp = first_design_pop2->m_nextp;
  while(tmp_t!=NULL && tmp!=NULL){
    if(tmp_t->m_nextp){
      parent1 = tmp_t;
      parent2 = tmp;
      do_crossover(parent1,parent2,cnt1);
    }
    else
      {
	parent1 = tmp_t;
	parent2 = first_design_pop2;
	do_crossover(parent1,parent2,cnt1);
	break;
      }
    tmp_t = tmp_t->m_nextp;
    tmp=tmp->m_nextp;
  }
   
 }


 
 void do_crossover(CDesign *first , CDesign *second,int cnt1){
 
   int i,RandIndex,counter=0;
  CDattrdes *tmp_a = NULL,*tmp_b=NULL;
  CDattrdes *cross_1 = NULL,*cross_2=NULL;
   CDesign *tmp_t=NULL;
  CDesign *tmp=NULL;
  bool chk = false;



   //randomly choose a crossover point for both designs
  
  do{
    RandIndex= rand() / (RAND_MAX / cnt1 + 1); 
  }while(RandIndex == 0);


  tmp_a = first->m_first_attr;
  tmp_b = second->m_first_attr;

  for(i=0;i<cnt1;i++){
    if(i==RandIndex){
      cross_1 = (CDattrdes *)malloc(sizeof(CDattrdes));
      assign_value(cross_1, NULL, tmp_a);
      cross_1->m_prevp = NULL;
      (tmp_a->m_prevp)->m_nextp=NULL;
      cross_1->m_nextp = tmp_a->m_nextp;
      cross_2 = (CDattrdes *)malloc(sizeof(CDattrdes));
      assign_value(cross_2,NULL, tmp_b);
      cross_2->m_prevp = NULL;
      (tmp_b->m_prevp)->m_nextp=NULL;
       cross_2->m_nextp = tmp_b->m_nextp;
      break;
    }
    else
      {
	tmp_a = tmp_a->m_nextp;
	tmp_b = tmp_b->m_nextp;
      }
  }

 
  
      
  // crossover the two designs
  (tmp_a->m_prevp)->m_nextp = cross_2;
  cross_2->m_prevp = tmp_a->m_prevp;

  (tmp_b->m_prevp)->m_nextp = cross_1;
  cross_1->m_prevp = tmp_b->m_prevp;
  

  
  //now put the children in their positions in the original population
  tmp = first_design;
  while(tmp){
    if(tmp->m_fitness == first->m_fitness){
      assign_design_value(first,tmp);
      break;
    }
    else
      tmp = tmp->m_nextp;
  }
  
  //clear pop2
  delete_designs(first_design_pop2);

   
   

 }



void insert_design_s(CDesign *design, int cntr){

  CDesign *des=NULL;
  design->m_count = cntr;
  
 
    for(des=first_design_pop1;des->m_nextp != NULL; des=des->m_nextp);
    des->m_nextp= design;
    design->m_prevp = des;
 
  
}




void assign_design_value(CDesign *from,CDesign *to_design){
  
  to_design->m_count = from->m_count;
  to_design->m_area = from->m_area;
  to_design->m_latency = from->m_latency;
  to_design->m_cost_value = from->m_cost_value;
  to_design->m_fitness = from->m_fitness;
  to_design->m_prob = from->m_prob;
  to_design->m_first_attr = from->m_first_attr;
  to_design->m_prevp = NULL;
  to_design->m_nextp = NULL;

}




















