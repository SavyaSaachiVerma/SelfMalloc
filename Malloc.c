/**
 * Machine Problem: Malloc
 * CS 241 - Fall 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



typedef struct malloc_metadata{
	size_t size;
	size_t ainvayi1;
	size_t ainvayi2;
//	struct malloc_metadata* next;
//	struct malloc_metadata* prev; 
	char buff[];
} m_metadata;

typedef struct free_metadata{
	size_t size;
	struct free_metadata* next;
	struct free_metadata* prev;
} f_metadata;

/* 	
	MACROS
*/

#define SIZE_M_METADATA (sizeof(m_metadata))
#define SIZE_F_METADATA (sizeof(f_metadata))
#define ALIGNMENT 8
#define MEM_ALLOC 1500000


/*
	GLOBAL VARIABLES
*/

m_metadata* m_head=NULL;
f_metadata* f_head=NULL;
//size_t mem_allocated=0;
static int v=0;
size_t sizu;
int flagster=0;
/*
	FUNCTIONS
*/
void move_free_ahead(){  					//moving the free pointer ahead if i allocate some mempry on a freed block

	f_metadata* f=(f_metadata*)f_head;
 
	f_metadata* fpre=NULL;

	f_metadata* p= (f_metadata*)(((char*)m_head)+m_head->size);			//new freed block

if(p==(f_metadata*)0x1fdb7320)
exit(12);

       if(v==0){									//running the move free ahead for the first time
                p->size=sizu-(m_head->size);
//		  p->size=mem_allocated-(m_head->size);	
//                mem_allocated=p->size;
		p->next=NULL;
                p->prev=NULL;
                f_head=p;
		flagster=0;
        }

	if(v!=0){
		while(f!=NULL){					//looking for a free block after our new freed block
			if(f > p){
				break;
			}
			fpre=f;
			f=f->next;
		}
		
		if(flagster==1){
			flagster=0;
			p->next=NULL;
			p->prev=fpre;
			fpre->next=p;
			p->size=sizu-(m_head->size);
		}
		 if(f==NULL){					//when there is no free block ahead of our freed block
		 	p->next=NULL;				//last free block
			p->prev=fpre->prev;			
			if(fpre->prev){
				(fpre->prev)->next=p;
			}
			else{					//incase  the previous block p points is null now 
				f_head=p;
			}
			p->size=sizu-(m_head->size);	
			
				fpre->prev=NULL;			//removing the links of old free block
				fpre->next=NULL;
			
		}
		else{
			//p->size= sizu - (m_head->size);	
			p->next=f;
			f->prev=p;
			p->prev=fpre->prev;
			if(fpre->prev){
				(fpre->prev)->next=p;
			}
			else{
				f_head=p;
			}
			p->size=sizu-(m_head->size);
	
				fpre->prev=NULL;			//removing old free bock links
				fpre->next=NULL;
			
		}

	}
v=1;
}

void* allocate_mem_less(size_t size,f_metadata* frp){						//USES FIRST FIT

//        f_metadata* p=f_head;
        m_metadata* chosen=NULL;
/*
        while(p!=NULL){
                if(p->size >=( size + SIZE_F_METADATA + 6) ){		//these extra bytes prevent collision of mfa block with  next malloced block(23)
                        chosen=(m_metadata*)p;
                        break;
                }
                p=p->next;
        }
*/
	chosen=(m_metadata*)frp;
        if(chosen){
		sizu=chosen->size;        				//the size of the free block it is malloced to
		chosen->size=size;
                m_head=chosen;
		move_free_ahead();
                return chosen->buff;
        }

	chosen=sbrk(0);							//whenever there is not sufficient memory for a block of size < 3050

        if(sbrk(MEM_ALLOC)== (void*)-1){                                              //Set some error
                return NULL;
        }
	flagster=1;
	sizu=MEM_ALLOC;
        chosen->size=size;
        m_head=chosen;	
	move_free_ahead();
    	return chosen->buff;
}

void* allocate_mem_large(size_t size){
	m_metadata* chosen=NULL;

	chosen=sbrk(0);

	if( sbrk(size) == ((void*)-1) ){
		return NULL;							//set some error
	}
	
	chosen->size=size;
//	m_head=chosen;

   return chosen->buff;
}
/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
void* ptr;

	ptr=malloc(num*size);    
	memset(ptr,0,(num*size));
		
    return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {

	if(size==0){								//if the person asks us to allocate 0 memory
		exit(5);
	}

	int flag=0;
	void* ptr;
	f_metadata* f= f_head;	

        size_t required= size + SIZE_M_METADATA;
        required = (required + ALIGNMENT - 1) & ~(ALIGNMENT-1);                 //rounding UP to the nearest 8 bits
	
	while(f!=NULL){								//checking if there is already a free block with the req size
		if(f->size >= required + SIZE_F_METADATA + 6){
			flag=1;							//TIME SAVING TIP:- DOnt run this loop again in malloc just pass the frre
			break;							//pointer capable of holding this data ( if 3050 condn pass NULL)
		}
		f=f->next;
	}

	if(required<=3050 || flag==1){

		ptr=allocate_mem_less(required,f);
	}
	else {
		ptr=allocate_mem_large(required);
	}

return ptr;
	
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {

    
	if(ptr==NULL){						//ERROR CONDITION
		return;
	}	

	m_metadata* index=ptr;					//finding the starting of the malloced block to be freed
	index-=1;

	f_metadata* p=(f_metadata*)index;

//if(p==(f_metadata*)0x1fdb7320)
//exit(12);

	f_metadata* trav=f_head;
	f_metadata* travprev=NULL;		
								// size is same as malloced block
	while(trav!=NULL){					//forming links with the newly freed block
		if( trav > p ){
			p->next=trav;
                        p->prev=travprev;
			if(travprev){
				(travprev)->next=p;
			}
			trav->prev=p;
			break;
		}
		travprev=trav;	
		trav=trav->next;
		
	}

	if(trav==NULL){
		if(travprev){
			travprev->next=p;
		}
		p->prev=travprev;
		p->next=NULL;
	}
	
	if(travprev==NULL){
		f_head=p;		
	}
	
	
	if( (p->next) && ( (f_metadata*)((char*)p+p->size)==p->next ) ){    			//coallescing with block ahead
              	f_metadata* f= p->next;
		p->size+=f->size;
		p->next=f->next;
		if(f->next){
			(f->next)->prev= p;
		}
		if(f){	
										//freeing the links of old ahead free block
			f->next=NULL;
			f->prev=NULL;
		}
	}
       
	if( (p!=f_head) && ( (f_metadata*)( ((char*)p->prev) + (p->prev)->size )== p ) ){		//coalesccing with block before
		f_metadata* bfr= p->prev;
		bfr->size+=p->size;
		bfr->next=p->next;
		if(p->next){
			 (p->next)->prev=bfr;
		}
		
		p->next=NULL;
		p->prev=NULL;		
	}
}
/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {

	void* retptr;    

	if(ptr==NULL){								//if the input pointer is NULL treat it as a MALLOC			
		retptr=malloc(size);
		return retptr;	
	}
	
	if(size==0){								//if the size provided is 0 then treat it as a free
		free(ptr);
		return NULL;
	}

        size_t required= size + SIZE_M_METADATA;
        required = (required + ALIGNMENT - 1) & ~(ALIGNMENT-1);                   //rounding UP to the nearest 8 bits

 	m_metadata* index=ptr;							//pointing to the beginning of the block to be realloced
        index-=1;

	
	if(index->size >= ( required + SIZE_F_METADATA + 6)){			//sending a smaller size than before
		size_t sizz= index->size;
	
		index->size=required;	

		f_metadata* fptr=(f_metadata*)( ((char*)index)+ index->size );	//newly freed block
                fptr->size= sizz - index->size;					//setting its size
		fptr->next=NULL;
		fptr->prev=NULL;

		f_metadata* trav= f_head;
		f_metadata* travpre=NULL;		

		while(trav!=NULL){
			if( ((char*)trav) > ((char*)fptr) ) {
				break;
			}
			travpre= trav;
			trav= trav->next;			
		}
		
/*		
		if(fptr==(f_metadata*)0x1fdb7320){
			fprintf(stderr,"trav:- %p\n",trav);
			fprintf(stderr,"travpre:- %p\n",travpre);
			fprintf(stderr,"travnext:- %p\n",trav->next);
			fprintf(stderr,"travprenext: %p\n", travpre->next);
			fprintf(stderr,"travprepre:- %p\n",travpre->prev);
			exit(14);
		}
*/		
		
		if(trav==NULL){
			if(travpre){
				travpre->next=fptr;
			}
			else{
				f_head=fptr;
			}
			fptr->prev=travpre;
			fptr->next=NULL;
		}
		else{
		
			if(travpre){
				travpre->next=fptr;
			}
			else{
				f_head=fptr;
			}
			fptr->prev=travpre;
			fptr->next=trav;
			trav->prev=fptr;

		}
/*
                if(fptr==(f_metadata*)0x1fdae178){
                        fprintf(stderr,"trav:- %p\n",trav);
                        fprintf(stderr,"travpre:- %p\n",travpre);
			fprintf(stderr,"travnext:- %p\n",trav->next);
                        fprintf(stderr,"next %p\n",fptr->next);
                        fprintf(stderr,"prev %p\n",fptr->prev);
                        exit(14);
                }

*/
	
        	if( (fptr->next)  &&  ( (f_metadata*)((char*)fptr + fptr->size) == fptr->next)){
			f_metadata* f= fptr->next;
            		fptr->size+=f->size;
                	fptr->next=f->next;
			if(f->next){
				(f->next)->prev=fptr;
			}
			if(f){
				
				f->next=NULL;
				f->prev=NULL;
			}
        	}

/*
                if(fptr==(f_metadata*)0x1fd97130){
                        fprintf(stderr,"trav:- %p\n",trav);
			fprintf(stderr,"travnext:- %p\n",trav->next);
                        fprintf(stderr,"travpre:- %p\n",travpre);
                        fprintf(stderr,"next %p\n",fptr->next);
                        fprintf(stderr,"prev %p\n",fptr->prev);
                        exit(14);
                }
*/

        	if(fptr!=f_head && ( (f_metadata*)((char*)fptr->prev+ (fptr->prev)->size)== fptr )){
			f_metadata* bfr=fptr->prev;
	               	bfr->next=fptr->next;
                       	bfr->size+=fptr->size;
			if(fptr->next){
				(fptr->next)->prev=bfr;
			}
	
			fptr->next=NULL;
			fptr->prev=NULL;
        	}

/*		
                if(fptr==(f_metadata*)0x1fdae178){
                        fprintf(stderr,"trav:- %p\n",trav);
                        fprintf(stderr,"travpre:- %p\n",travpre);
			fprintf(stderr,"next %p\n",fptr->next);
			fprintf(stderr,"prev %p\n",fptr->prev);
                        exit(14);
                }
*/

		return index->buff;
		
	}


     if(index->size < required + SIZE_F_METADATA + 6){

	f_metadata* f=f_head;
	f_metadata* fpre=NULL;
	f_metadata* fref;
	
	while( f!=NULL){
		if( (char*)f > (char*)index ){
			break;
		} 
		fpre=f;
		f=f->next;
	}

	if(f!=NULL && ( ((char*)index+index->size) == (char*)f ) && ( ( index->size + f->size) >= (required + SIZE_F_METADATA + 6) ) ){

		size_t siz1= (index->size)+ (f->size);

		if(required<=index->size){
			if(fpre)
				fpre->next=f->next;
			if(f->next)
				f->next->prev=fpre;
			f->prev=NULL;
			f->next=NULL;
			index->size=siz1;
			return(index->buff);	

		}



		index->size=required;
/*
		if(required<=index->size){
			fref= (f_metadata*)(((char*)index)+index->size+100);
		}
*/
	        fref= (f_metadata*)(((char*)index)+index->size);
		
		fref->next=f->next;
		fref->prev=fpre;

		if(fpre){
			fpre->next=fref;
		}
		else{
			f_head=fref;
		}
		if(f->next){
 			(f->next)->prev=fref;
		}

		fref->size=siz1-index->size;
	
		f->next=NULL;
		f->prev=NULL;

		return(index->buff);
	}
	else{
		void* reptr;
		reptr=malloc(size);
		size_t siz= index->size - SIZE_M_METADATA;		
		if(reptr){
			memmove(reptr,ptr,siz);
		}
		free(ptr);

		return reptr;
	} 
     }
		
	
return NULL;
}