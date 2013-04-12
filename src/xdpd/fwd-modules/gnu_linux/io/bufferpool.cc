#include "bufferpool.h"
#include <rofl/datapath/pipeline/platform/cutil.h>

/* Static member initialization */
bufferpool* bufferpool::instance = NULL;
pthread_mutex_t bufferpool::mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t bufferpool::cond = PTHREAD_COND_INITIALIZER;
 
//Constructor and destructor
bufferpool::bufferpool(long long unsigned int pool_items)
	: pool(pool_items)
{

	long long unsigned int i;
	datapacket_t* dp;
	datapacketx86* dpx86;

	//Reserve space
	//  pool.reserve(pool_items);
	pool_status.reserve(pool_items);

	for(i=0;i<pool_items;++i){

		//Init datapacket
		dp = (datapacket_t*)malloc(sizeof(datapacket_t));
		
		if(!dp){
			//Mark as unavailable
			pool[i] = NULL;
			pool_status[i] = BPX86_SLOT_UNAVAILABLE;

			//Skip
			continue;
		}

		//Memset datapacket
		memset(dp,0,sizeof(dp));
		
		//Init datapacketx86
		try {
			dpx86 = new datapacketx86();
		}catch(std::bad_alloc ex){

			//Mark as unavailable
			pool[i] = NULL;
			pool_status[i] = BPX86_SLOT_UNAVAILABLE;
			
			free(dp);
			continue;		
		}		

		//Assign the buffer_id
		dpx86->internal_buffer_id = i;			
		dpx86->buffer_id = 0; //Mark as 0
		
		//Link them
		dp->platform_state = (platform_datapacket_state_t*)dpx86;

		//Add to the pool	
		pool[i] = dp;
		pool_status[i] = BPX86_SLOT_AVAILABLE;
	}

	//Set size
	pool_size = pool_items;
}

bufferpool::~bufferpool(){
	
	unsigned long long int i;

	for(i=0;i<pool.size();++i){
		if(pool[i]){
			delete (datapacketx86*)pool[i]->platform_state;
			free(pool[i]);
		}	
	}
}

/*
* Protected static singleton instance
*/
inline bufferpool* bufferpool::get_instance(void){

	//Be lock-less once initialized	
	if(!bufferpool::instance){
		//Wait init
		pthread_mutex_lock(&bufferpool::mutex);		
		pthread_cond_wait(&bufferpool::cond,&bufferpool::mutex); 
		pthread_mutex_unlock(&bufferpool::mutex);		
	}
	return bufferpool::instance;
}

//Public interface of the pool

/*
* Retreives an available buffer. This method is BLOCKING
*/
datapacket_t* bufferpool::get_free_buffer(bool blocking){

	long long unsigned int i;
	
	bufferpool* bp = get_instance();

	//Trying to minimize locking.	
	for(i=0;; (i+1 == bp->pool_size)? i=0 : ++i){ // fixme this has to be improved since its always searching a free packet from the beginning --> queue
		if(bp->pool_status[i] == BPX86_SLOT_AVAILABLE){
			
			//Take mutex
			pthread_mutex_lock(&bufferpool::mutex);		
	
			//Recheck
			if(bp->pool_status[i] != BPX86_SLOT_AVAILABLE){
				//There has been an undesired scheduling of threads 
				pthread_mutex_unlock(&bufferpool::mutex);		
				continue;	
			}
			
			//Mark as in-use		
			bp->pool_status[i] = BPX86_SLOT_IN_USE;
		
			//Release
			pthread_mutex_unlock(&bufferpool::mutex);		
			
			//return buffer 
			return bp->pool[i];
		}
		
		//Wait in cond variable before restarting from the first item of the pool 
		if (i+1 == bp->pool_size) {
			if (blocking) {
				pthread_mutex_lock(&bufferpool::mutex);
				pthread_cond_wait(&bufferpool::cond,&bufferpool::mutex);
				pthread_mutex_unlock(&bufferpool::mutex);
			} else {
				return NULL;
			}
		}
	}
}

/*
* Releases a previously acquired buffer.
*/
void bufferpool::release_buffer(datapacket_t* buf){

	bufferpool* bp = get_instance();

	unsigned int id = ((datapacketx86*)buf->platform_state)->internal_buffer_id;
	
	//Release
	if(bp->pool_status[id] != BPX86_SLOT_IN_USE){
		//Attempting to release an unallocated/unavailable buffer
		//TODO: put a trace here
		DEBUG_ERROR_EXIT("Attempting to release an unallocated/unavailable buffer");
	}else{ 
		buf->is_replica = false; //Make sure this flag is 0
		bp->pool_status[id] = BPX86_SLOT_AVAILABLE;
		pthread_cond_broadcast(&bufferpool::cond);
	}
}

//
// Buffer pool management
//
void bufferpool::init(long long unsigned int capacity){
	

	pthread_mutex_lock(&bufferpool::mutex);		

	if(bufferpool::instance){
		//Double-call to init??
		fprintf(stderr,"Double call to bufferpool init!! Skipping...\n");
		pthread_mutex_unlock(&bufferpool::mutex);
		return;	
	}

	//Init 	
	bufferpool::instance = new bufferpool(capacity);

	//Wake consumers
	pthread_cond_broadcast(&bufferpool::cond);

	//Release and go!	
	pthread_mutex_unlock(&bufferpool::mutex);		
}

void bufferpool::destroy(){

	if(get_instance())
		delete get_instance();

	instance = NULL;	
}

/*
* Resizes current buffer pool to new_capacity
*/
void bufferpool::increase_capacity(long long unsigned int new_capacity){

	long long unsigned int i;
	bufferpool* bp = get_instance();
	
	datapacket_t* dp;
	datapacketx86* dpx86;

	//Check for current capacity
	if(new_capacity <= bp->pool_size)
		return;	

	//Resize vectors
	try {
		bp->pool.reserve(new_capacity);
		bp->pool_status.reserve(new_capacity);
	}catch(std::bad_alloc ex){
		return; 
	}		

	//Create buffers	
	for(i=bp->pool_size;i<new_capacity;i++){

		//Init datapacket
		dp = (datapacket_t*)malloc(sizeof(datapacket_t));
		
		if(!dp){
			//Mark as unavailable
			bp->pool[i] = NULL;
			bp->pool_status[i] = BPX86_SLOT_UNAVAILABLE;

			//Skip
			continue;
		}

		//Init datapacketx86
		try {
			dpx86 = new datapacketx86();
		}catch(std::bad_alloc ex){
			//Mark as unavailable
			bp->pool[i] = NULL;
			bp->pool_status[i] = BPX86_SLOT_UNAVAILABLE;
			
			free(dp);
			continue;		
		}		

		//Assign the buffer_id
		dpx86->internal_buffer_id = i;			
		
		//Link them
		dp->platform_state = (platform_datapacket_state_t*)dpx86;

		//Add to the pool	
		bp->pool[i] = dp;
		bp->pool_status[i] = BPX86_SLOT_AVAILABLE;
	
	}
	
	//Finally allow to use extended capacity
	bp->pool_size = new_capacity;
	
}
