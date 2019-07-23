
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
int row=0 , row_i , node_cnt=0;
int col = 0;
int design_num = 00;
int list_cnt=0;
int attr_cnt[];
int global_counter = 0;
CAttrlist *tree_list = NULL;
CAttrlist *tree_first_list = NULL;
CDesign *first_design  ;
CDesign *new_design_s = NULL;
CDesign *prev_design_s = NULL;
CDattrdes *design_attr_s = NULL;
CAttr *attr = NULL;
Croot_node *first_root = NULL;
Croot_node *root = NULL;
Ctree_node *child = NULL;
char directory_s[150];
int arr[20][20];
int countdes=0;

CDesign *simulated_annealing(CAttrlist *first_attr_list, char *heuristic , char *process,char *header , char *define_header, char *in_file, char *analyse_command){

 

 first_design = NULL;
 row = 0;
 
  attr_random_assign(first_attr_list);

  

 

 annealing_algorithm(first_attr_list , heuristic, process, header,define_header, in_file , analyse_command);

  free(prev_design_s);
  free(design_attr_s);

return first_design;
}

void annealing_algorithm(CAttrlist *first_attr_list, char *heuristic , char *process,char *header , char *define_header, char *in_file, char *analyse_command){

CDesign *desk = NULL;
  CDattrdes *atr = NULL ;
CDattrdes  *atr1 = NULL;

  int cntr , cntr1=0;
int sum=0, avg=0, area_threshold;

  new_design_s = (CDesign *) malloc(sizeof(CDesign));
  if(prev_design_s== NULL){

    prev_design_s = (CDesign *)malloc(sizeof(CDesign));
    design_attr_s = (CDattrdes *)malloc(sizeof(CDattrdes));

    prev_design_s = new_design_s;  /* transfer to previous design*/
  }

else

   design_attr_s = prev_design_s->m_first_attr;

  row_i=0;
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
       design_attr_s = prev_design_s->m_first_attr;
       rand_design_create(first_attr_list);
       desk = first_design;
    }
    else
      continue;
  }

insert_design(new_design_s , countdes);

  countdes++;

 synthesize_design(new_design_s , heuristic,process ,header,define_header, in_file, analyse_command );  /*synthesize and extract area, delay information  1 STEP*/

  if(countdes < 10)
    annealing_algorithm(first_attr_list , heuristic, process, header,define_header, in_file , analyse_command);
else
 /**** print to log file*****/
{
  print_to_file();

/***** find the average area of all designs****/
for(desk = first_design ; desk != NULL; desk = desk->m_nextp){
sum = sum+desk->m_area;
}
avg = sum / countdes;
area_threshold = avg - (avg / 20);
for(desk = first_design ; desk != NULL; desk = desk->m_nextp){
  if(desk->m_area <= area_threshold)
   desk->m_hash = true;
else
desk->m_hash = false;
                                        
}

//sorting_list();
create_decisiontree(first_design, first_attr_list , NULL);

parse_tree();

delete_tree();

}
}

void parse_tree(void){
 
 bool flag;
 Croot_node *head = first_root;
 Croot_node *tmp = NULL;
 Croot_node *tail = root;
 Ctree_node *child = NULL, *child_tmp=NULL;
 CAttr *list_atr = NULL;
 
 while(head){
	child = head->children[0];
	tree_list = (CAttrlist *)malloc(sizeof(CAttrlist));
	tree_list->m_name = head->t_name;
        
	while(child){
		if(child->area_small == true){
		child_tmp = child;
	
		flag = true;
		
		}
		else if (child->childp && child->area_small == false){
		tmp = child->childp;
		if(flag != true) child_tmp = child;
		
		}
		child = child->nextchild;
	}
	list_atr = (CAttr *)malloc(sizeof(CAttr));
	list_atr->m_name = child_tmp->c_name;
	list_atr->m_value = child_tmp->c_value;
	list_atr->m_list = child_tmp->list;
	list_atr->m_nextp = NULL;
	tree_list->m_first_attr = list_atr;   
	if( !tree_first_list) tree_first_list = tree_list;
		else insert_attr_list(tree_first_list, tree_list);
	head = tmp;
	flag = false;
	if( head == tail) head = NULL;
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
	  if( head->m_area > tempnode->m_area){

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
	  } /*end of while loop*/
	        if(flag == 0)
		break;
	        else
	        {
	           head = first_design;
	           flag = 0;
	        }
    } /* end of for loop*/

}




void print_to_file(){
  
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

  ptr_f = fopen("designs.rpt", "wt");
  if(ptr_f == NULL){
    printf("\n cannot create report file\n");
    exit(EXIT_FAILURE);}

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
}


void attr_random_assign(CAttrlist *attr_list){

  
  CAttr *attr = NULL;
  int cnt = 0;
  
 /*  // int a = 0; */
 
  if(attr_list != NULL){
    list_cnt++;
  /***** for going through all the lists *******/
    col = 0;
    /*for going through one attribute list**/
    for( attr = attr_list->m_first_attr ; attr ; attr = attr->m_nextp){
      
     arr[row][col] = col;
      col++;
    }
    attr_cnt[list_cnt-1]=col;
    printf("\n the row, column %d %d\n" , row,col);
    arr[row][col] = 'a';
    printf("\n the row size %d\n", col);
    printf("\n the first element of arr_cnt %d %d %d\n" , attr_cnt[0], attr_cnt[1],attr_cnt[2]);
    row++;
    attr_random_assign(attr_list->m_nextp);
  }
    
 /*  //  printf(" the length of first row %d\n", sizeof(arr[0])); */
}



/*simulated annealing function*/
void rand_design_create(CAttrlist *attr_list){

  int RandIndex;
  double prob ;
  int  cnt = 0 ;
  int  col_i = 0 ;
  CAttr *attr = NULL;
 
  CDattrdes *new_attr = NULL;
 
  
  
  

  if(attr_list != NULL){
    //  srand(time(NULL));
  new_attr = (CDattrdes *)malloc(sizeof(CDattrdes));
  if(attr_list->m_prevp == NULL)
    row_i = 0;
    
  if(first_design == NULL){

   
    while(arr[row_i][col_i] != 'a') /* go to the last element of each row of array*/
      col_i++;
   
   
    printf("\n the element %d %d %d\n", arr[row_i][col_i], row_i,col_i);
 
    
RandIndex = rand() / (RAND_MAX / col_i + 1); //arr[row_i][col_i]; /* generate a random number amongst the number of attributes in a design*/
       cnt = 0;
   
   for( attr = attr_list->m_first_attr ; attr ; attr = attr->m_nextp){
    if (cnt == RandIndex)
      {
     assign_value(new_attr,attr , NULL);
     break;
      }
    cnt++;
   }
 


  }
  else
    {
     
      prob = rand() / ((double)RAND_MAX);
      if(prob <= 0.8){

         while(arr[row_i][col_i] != 'a') /* go to the last element of each row of array*/
	   col_i++;
	 
	 //printf("\n the element %d \n", arr[row_i][col_i]);
           RandIndex = random_range(col_i , attr_list);
       
	cnt = 0;
	for(attr = attr_list->m_first_attr ; attr ; attr=attr->m_nextp){
	 
	  if(cnt == RandIndex)
	    {
	    assign_value(new_attr , attr,NULL);
	    break;
	    }
	  
	  cnt++;
	}
      }
      else
	
	assign_value(new_attr , NULL , design_attr_s);	

       design_attr_s = design_attr_s->m_nextp;
    }

   insert_node(new_design_s , new_attr);
   row_i++;  
   rand_design_create(attr_list->m_nextp);
     } /**end of IF loop***/

 
}

int random_range(int column , CAttrlist *list){

  int index , cnt;
  CAttr *tmp_a = NULL;

  index = rand() / (RAND_MAX / column + 1);

  cnt = 0;
  for(tmp_a = list->m_first_attr ; tmp_a ; tmp_a = tmp_a->m_nextp){
    if(cnt == index){
      if((tmp_a->m_name != design_attr_s->d_name) && (tmp_a->m_value != design_attr_s->d_value))
        return index;
      else
	return(random_range(column , list));
    }
    cnt++;
  }

  // return index;
}



void synthesize_design(CDesign *design ,char *heuristic , char *process, char *header, char *define_header, char *in_file, char *analyse_command ){

 int ck; 
  char file_synth_r[200];
  char file_read[20];
  char line[MAX_CHARS_WORD];
  FILE *data_file = NULL;

  int c =0 , d=0;
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
    snprintf(directory_s , sizeof(directory_s), "cp %s %s ." , header, define_header);
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
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -EE -lfl /eda/bin/cwb/cyber/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber/lib/asic_45.BLIB %s.IFF -Zflib_fcnt_out -Zflib_out_limit=L50:M100:S100 -s",process);
  system(file_synth_r);
  file_synth_r[0] = '\0';

 /*********generating memory constraint file**********/  
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -c1000 -Zmem_kind=RW1 -s -EE -lfl /eda/bin/cwb/cyber/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber/lib/asic_45.BLIB %s.IFF -lfc %s-auto.FCNT -Zmlib_mcnt_out",process,process);
  system(file_synth_r);
 file_synth_r[0] = '\0';

 /****** the synthesizing command ************/
  snprintf(file_synth_r , sizeof(file_synth_r), "bdltran -c1000 -s -tcio -EE -lfl /eda/bin/cwb/cyber/lib/asic_45.FLIB -lb /eda/bin/cwb/cyber/lib/asic_45.BLIB -lfc %s-auto.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT %s.IFF",process,process,process,process);
 system(file_synth_r);
 file_synth_r[0]='\0';
 snprintf(file_read , sizeof(file_read), "%s.CSV", process);
 data_file = fopen(file_read , "r");
  while(fgets(line, sizeof(line), data_file) != NULL)
   { 
     do{
       if(c==0){
       fscanf(data_file , "%d" , &design->m_area);
       printf("area %d\n", design->m_area);}

       fscanf(data_file, "%d" , &d);
      /*  //  printf("values %d\n", d); */
       if(c == 8)
	 design->m_latency = d;
      /*  //  printf("the count%d\n",c); */
       c++;
     }while(fgetc(data_file) != EOF);
    }

 fclose(data_file);
 design_num++;

}



void create_decisiontree(CDesign *design, CAttrlist *first_list, Ctree_node *node){

char *name;
bool roots_done , flag;
CAttrlist *head = first_list;
CAttrlist *list_t = NULL ;
CDesign *des_head = design;
Croot_node *rt = NULL;
Ctree_node *prev_child = NULL;




if( first_root)
{
     if(node->c_pos == 0 ||node->c_neg == 0 )
        {if( !node->c_pos)
         node->area_small = false;
        else
        if(node->c_neg == 0)
         node->area_small = true;
         node->childp = NULL;
         node->leaf_node = true;
   create_decisiontree(des_head, first_list, node->nextchild);
        }
     else
       node->leaf_node = false;
  
}
 

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
 
//if(roots_done == true)
//exit(0);


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
 
 while(rt){
	child = rt->children[0];
	while(child){
		if(child->leaf_node == false && child->childp == NULL)
		{ node = child; flag = true;break;}
		child = child->nextchild;
	}
	if(flag == true) break;
	rt = (rt->parentp)->nodeparent;
	if(rt == first_root) rt = NULL;
	
 }




if( node || flag == true)
  	create_decisiontree(design,first_list,node);
 
 

 }
else return;

 
}



bool check_attri(CDesign *des, CAttr *attr , Ctree_node *node){

bool check;
CDattrdes *des_at = NULL;

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
 
    des_at = des->m_first_attr;
    while(des_at){
	if(des_at->d_name == node->c_name && des_at->d_value ==node->c_value)
            {check = true;break;}
        else
	    check = false;
        des_at = des_at->m_nextp;
        }
    des_at = des->m_first_attr;
 if(check){
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
  
      

inline long double logar(long double x){

return log(x) * M_CONST_E;
}

 

  
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
  printf("\n the pos & neg value %f %f\n", root->children[i]->c_pos,root->children[i]->c_neg);
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
 (root->children[i])->list = att->m_list;
 if( !(root->children[i])->c_pos || !(root->children[i])->c_neg)
 (root->children[i])->leaf_node = true;
 else (root->children[i])->leaf_node = false;
if(root->children[i]->c_pos == 0 ||(root->children[i]->c_pos && root->children[i]->c_neg))
  root->children[i]->area_small = false;
if(root->children[i]->c_neg == 0) root->children[i]->area_small = true;
 (root->children[i])->nextchild = NULL;

}
    
 
     


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
              }
            
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
            }
            else tem = 0.0;
                
            entropy = entropy + ((attr->pos + attr->neg)/count_num)* tem;
              
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
