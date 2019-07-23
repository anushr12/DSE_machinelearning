/*************************************************************************************
 **
 ** brute_force.c
 **
 ** version 1.0 created by : Dong
 ** Date : Sep 15, 2015
 ** Comments: Brute Force DSE
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

CDesign *first_design = NULL;
CAttrlist *first_attr_list_bf = NULL;
int count_designs=1;
int num_attr = 0;
/*function declaration*/
void comb_func (int tn_index, int k_index[], int attr_index[], CAttrlist *attr_list_cf);
CDesign *gen_new_design(CAttrlist *first_attr_list, int attr_index[]);
//CDesign *gen_empty_design();
CAttr *gen_new_attr(CAttr *first_attr, int now_attr_index);

/***********************
** 
** 1.-  Brute_force
**
** Comments: brute force reference method	
** step 1: with a function, one combination (array) of attributes index..
**         ..(1,2,3)can be found
** step 2: with one combination array, one design can be generate
** step 3: re-do the step 1 and step 2 and find out all the combination
**
**(note: adam for revise)
************************/

void brute_force(CAttrlist *first_attr_list){
	// *first_design should be global variable  
	//CDesign *first_design = NULL;
    
	/* local variable declaration */
	// int num_attr = 0;
	first_attr_list_bf = first_attr_list;
	num_attr = count_attributes(first_attr_list); //number of attributes types
	
	// attr_index can be global variable Adam 06/08
	int attr_index[num_attr];
	int k_index[num_attr]; 
	int tn_index = 0;
	
	/* initialize k_index and attr_index */
	int i_k;
	int i_k_comp = num_attr - 1;
	for (i_k = i_k_comp; i_k > 0; i_k--){
		k_index[i_k] = 1;
        attr_index[i_k] = 0;
	}
    k_index[0]=0;
    attr_index[0] = 0;
	
	comb_func(tn_index, k_index, attr_index, first_attr_list);  // it need to declaration in header ?? adam
  
	/* identify the optimal designs */
	optimal_designs(first_design);
  /* Run brute force method */
  /* Delete non-optimal designs */
  /* Return list of optimal designs found */
  // return first_design;
}

/*********************************************************
 **
 ** comb_func
 **
 ** find out one combination of attributes to be a design
 **
 *********************************************************/
void comb_func(int tn_index, int k_index[], int attr_index[], CAttrlist *attr_list_cf){
	
	CDesign *design_current = NULL;
	int i, r, r_comp;
	r = tn_index;
	CAttrlist *now_attr_list;
        now_attr_list = attr_list_cf;
	
	r_comp = num_attr;
	
	if ( r == 0 ){
	
		for (i=0; i< (now_attr_list->m_num);i++){
			attr_index[r] = i;
	        
			/*create the design*/
			design_current = gen_new_design (first_attr_list_bf, attr_index);
			
			synthesize(design_current);
			
			/* print one combination*/
			int i_print;
            for (i_print=0; i_print<r_comp; i_print++){
               printf ("%d\t",attr_index[i_print]);
               if(i_print==(r_comp-1)) printf("\n");
            } 
		
		}
	
		r++;
		now_attr_list = now_attr_list->m_nextp;
		/*recursive function */
		comb_func (r, k_index, attr_index, now_attr_list);
	}
	
	else if (r < r_comp){
	
		if(k_index[r] < (now_attr_list -> m_num) ){
			attr_index[r] = k_index[r];
			k_index[r]++;
			// move to the previous attributes list
			r--;
			now_attr_list = now_attr_list->m_prevp;
			comb_func (r, k_index, attr_index, now_attr_list);
		}
		
		else{
			int i_r;
			
			//reset the k index before moving to the next attributes list
			for (i_r = r; i_r >0; i_r--){
				k_index[i_r] = 0;
			}
			// move to the next attributes list
			r++;
			now_attr_list = now_attr_list->m_nextp;
			comb_func (r, k_index, attr_index, now_attr_list);
		
		}


    }
}

/*********************************************************
 **
 ** count_attributes 
 **
 ** count the total number of attributes in the list
 **
 *********************************************************/
int count_attributes(CAttrlist *first_attr){

  /* Variables declaration */
  CAttrlist *attr;
  int count=0;

  for(attr=first_attr; attr!= NULL; attr=attr->m_nextp)
    count++;

  return count;
}

/*********************************************************
 **
 ** gen_new_design
 **
 ** Generate a unique new configuration
 ** parameter name can be first_attr_list on not ? Adam 06/08
 **
 *********************************************************/
 CDesign *gen_new_design(CAttrlist *first_attr_list, int attr_index[]){
	/* Variables declaration */
	CDesign *design = NULL;
	CAttrlist *attr = NULL;
	CAttr *tattr_new = NULL;
	
	int now_attr_index = 0;  //define attr1 or attr2 or attr3...
	
	/* Allocate memory for a design */
	design = gen_design();
	
	/* Generate specified attributes with pre-defined index*/
	for (attr=first_attr_list; attr; attr=attr->m_nextp){
		
		/* Generate a new rnd attribute */
		tattr_new = gen_new_attr(attr->m_first_attr, attr_index[now_attr_index]);
		
		now_attr_index++; //move to the next attribute index
		
		if (design->m_first_attr == NULL)
			design->m_first_attr = tattr_new;
		else
			insert_attr(design->m_first_attr, tattr_new);
		// insert_attr function is in dse.c
	}
	
	/* insert design in design linked list */
	if(first_design == NULL)
		first_design = design;
	else 
		insert_design(first_design, design);
	
	return design;
	
 }
 
/*********************************************************
 **
 ** gen_design
 **
 ** generate an empty design
 **
 *********************************************************/
 CDesign *gen_design(){
	/* Variables declaration */
	CDesign *design=NULL;	
	
	design = (CDesign*)malloc(sizeof(CDesign));
	
	design->m_count = count_designs;
	design->m_pareto_optimal = false;
	design->m_adjusted = false;
	design->m_interpolated = false;
	design->m_area = 0;
	design->m_latency = 0;
	design->m_hash = 0;
	
	design->m_prob_max=0;
	design->m_prob =0;
	
	design->m_first_attr = NULL;
	design->m_design_fu_single = NULL;

	design->m_prevp = NULL;
	design->m_nextp = NULL;	
	
	count_designs++;
	
	return design;
 }
 
 /*********************************************************
 **
 ** gen_new_attr
 **
 ** Select a new random attribute
 **
 *********************************************************/
 CAttr *gen_new_attr(CAttr *first_attr, int now_attr_index){
	/*Variables declaration*/
	CAttr *tattr = NULL;
	CAttr *tattr_new = NULL;
	
	int x=0;
 
	for (tattr = first_attr;tattr; tattr = tattr -> m_nextp){
		if(now_attr_index == x){
			tattr_new = copy_attr(tattr);
			break;
		}
		x++;
	}
	
	return tattr_new;
 }

 /*********************************************************
 **
 ** copy_attr
 **
 ** generate and copy attribute
 **
 *********************************************************/
CAttr *copy_attr(CAttr *attr){

	/* Variables declaration */
	CAttr *attr_new = NULL;

	attr_new = (CAttr *)malloc(sizeof(CAttr));
 
	attr_new->m_name = (char*)malloc(sizeof(char)*(strlen(attr->m_name)+1));
	strcpy(attr_new->m_name, attr->m_name);

	attr_new->m_value = (char*)malloc(sizeof(char)*(strlen(attr->m_value)+1));
	strcpy(attr_new->m_value, attr->m_value);

	attr_new->m_min = attr->m_min;
	attr_new->m_max = attr->m_max;
 
	attr_new->m_list = attr->m_list;
	attr_new->m_nextp = NULL;
	attr_new->m_prevp = NULL;

	return attr_new;
}

/*********************************************************
 **
 **synthesize
 **
 ** Synthesizes single design
 **
 *********************************************************/
void synthesize(CDesign *design){

	/* Variables declaration */
	char synops[MAX_CHARS_LINE];
	
	/* 
	** ATTR FILE
	** Create the attribute file as a head file
	*/
	gen_attr_file(design);	
	
	/* 
	** BDLPARS
	**Parse only very first design to create .IFF file
	*/
 
	if(strstr(in_file,".sc") || strstr(in_file,".cpp")) 
		sprintf(synops,"scpars %s", in_file);
	else
		sprintf(synops,"bdlpars %s -DBDL", in_file);

	system(synops);
 
	/*
	** BDLTRAN
	** Synthesize the design
	**
	*/
	//sprintf(synops,"bdltran -c1000 -lfl %s -lb %s %s.IFF -Zflib_fcnt_out -Zmlib_mcnt_out -Zflib_out_limit=L100:M100:S100 -s", flib, blib, process);
	//system(synops);
  
	//sprintf(synops,"bdltran -c1000 -s -lfl %s -lb %s -lfc %s-auto.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT %s.IFF ", flib, blib, process, process, process, process);
	/*for running interpolation with +lfl interpolation-auto.FLIB -adam 15092015*/
	//sprintf(synops,"bdltran -c1000 -s Zresource_fcnt=GENERATE -Zresource_mcnt=GENERATE -Zdup_reset=YES -EE -lfl %s -lb %s -lfc %s-auto.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT +lfl %s-auto.FLIB %s.IFF ", flib, blib, process, process, process, process, process);	
	sprintf(synops,"bdltran -c1000 -s -Zresource_fcnt=GENERATE -Zresource_mcnt=GENERATE -Zflib_out_limit=L100:M100:S100 -Zdup_reset=YES -EE -lfl %s -lb %s %s.IFF",flib,blib,process);
	system(synops);
	

	/* Read the results */
	read_bdltran_syn_results(design, process);	
	/* Make a new folder and store attr.h files */
    /* 	sprintf(synops,"mkdir ./dse_%s/%s%d",process,  process, design->m_count);
	system(synops);

	sprintf(synops,"mv attrs.h ./dse_%s/%s%d  ",process,process,design->m_count);
	system(synops); */ 
	
	/* Make a new folder and store synthes results there */
	sprintf(synops,"mkdir ./dse_%s/%s%d",process,  process, design->m_count);
	system(synops);

	sprintf(synops,"mv *.IFF *.MLIB *.MCNT *.CSV *.QOR  attrs.h ./dse_%s/%s%d  ",process,process,design->m_count);
	system(synops);

	sprintf(synops,"mv %s-auto.FCNT  ./dse_%s/%s%d/%s-auto%d.FCNT  ",process,process, process, design->m_count, process, design->m_count);
	system(synops);

	/* For the interpolation with interp-auto.FLIB adam 15092015*/
	sprintf(synops,"mv %s-auto.FLIB  ./dse_%s/%s%d/%s-auto%d.FLIB  ",process,process, process, design->m_count, process, design->m_count);
	system(synops);	
	
	/* Store results in log file */
	results_single(design);
}

/*********************************************************
 **
 ** gen_attr_file
 **
 ** Generate a header file with the attributes 
 **
 *********************************************************/
void gen_attr_file(CDesign *design){

	/* Variables declaration */
	FILE *ptr_file = NULL;
	CAttr *attr= NULL;
	int x=1;

	ptr_file = fopen("attrs.h", "wt");
	if(ptr_file == NULL){
		printf("\n****** ERROR : Can't generated attrs.h ");
		return;
	}

	fprintf(ptr_file,"/* Automatic DSE for CyberWorkBench ");
	fprintf(ptr_file,"\n by PolyU DARClab DSE */");
	fprintf(ptr_file,"\n");


	for(attr=design->m_first_attr; attr; attr=attr->m_nextp){
		if(attr->m_name !=0 && attr->m_value != 0)
			fprintf(ptr_file,"\n#define %s Cyber %s=%s",attr->m_list->m_name, attr->m_name, attr->m_value);
			x++;
	}

	fprintf(ptr_file,"\n");
	fclose(ptr_file);

}

/*********************************************************
 **
 ** read_bdltran_syn_results
 **
 ** read the synthesis results 
 **
 *********************************************************/
void read_bdltran_syn_results(CDesign *design, char *process){

	/* Variables declaration */
	FILE *ptr_file = NULL;
	char name[MAX_CHARS_WORD], line[MAX_CHARS_LINE];
	char delims[] =",", *result = NULL;
	int x=0;

	sprintf(name, "%s.CSV", process);

	ptr_file = fopen(name, "rt");
	if(ptr_file == NULL){
		printf("\n****** ERROR : Can't open %s.CSV ",process);
		return;
	}


	if(fgets(line, MAX_CHARS_LINE,ptr_file) == NULL)
		return; // file is empty -> bdltran error
	fgets(line, MAX_CHARS_LINE,ptr_file);

	result = strtok(line, delims);
	design->m_area = atoi(result);
  
	while( result != NULL ) {
		x++;
		if(x ==19){
			design->m_latency = atoi(result);     //column 19
			break;
		}


    result = strtok( NULL, delims );
}

	fclose(ptr_file);

}

/*********************************************************
**
** copy_attrs_list
**
** Generate a unique new configuration
**
** Revised version: parent -> design offspring -> design_new
*********************************************************/
void copy_attrs_list(CDesign *design, CDesign *design_new, int num){

  /* Variables declaration */
  CAttr *tattr= NULL, *tattr_new= NULL;
  int count =0;

  for(tattr= design->m_first_attr; tattr; tattr=tattr->m_nextp){

    if(count == num)
      break;

    tattr_new = copy_attr(tattr);
    if(design_new->m_first_attr == NULL)
      design_new->m_first_attr = tattr_new;
    else
      insert_attr(design_new->m_first_attr, tattr_new);

    count ++;

  }

}

/*********************************************************
 **
 ** optima_designs
 **
 ** identify the Pareto-optimal designs
 **
 *********************************************************/
void optimal_designs(CDesign *first_design){

  /* Variables declaration */
	CDesign *design= NULL;

	for(design=first_design; design; design=design->m_nextp)
		design->m_pareto_optimal = optimal_single(first_design, design);
  

}

/*********************************************************
 **
 ** optimal_single
 **
 **  look for the smallest area design for each unique latencies
 **
 *********************************************************/
bool optimal_single(CDesign *first_design, CDesign *design_chk){

	/* Variables declaration */
	CDesign *design= NULL;

	// Return false if the area or latency = 0 -> probably bdltran error
	if(design_chk->m_area == 0 || design_chk->m_latency ==0)
		return false;


	for(design=first_design;design; design=design->m_nextp){
		if(design == design_chk)
			continue;
		else if(design->m_area == 0 || design->m_latency == 0)
			continue;
    
    // look for the smallest area design for each unique latencies
    if(design->m_latency == design_chk->m_latency){
		if(design->m_area < design_chk->m_area)
	return false;
    
	else if(design->m_area == design_chk->m_area){
		if(design_chk->m_count > design->m_count)
			return false;
		}
    }
  }

    return true;
} 

/*********************************************************
 **
 ** insert_design
 **
 ** insert design in linked list
 **
 *********************************************************/
void insert_design(CDesign *first, CDesign *design_new){

  /* Variables declaration */
  CDesign *design = NULL;

  for(design=first;design->m_nextp; design=design->m_nextp);
  
  design->m_nextp = design_new;
  design_new->m_prevp = design;

}
