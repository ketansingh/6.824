// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

void print_map(std::map<lock_protocol::lockid_t, int> mymap)
{
	std::cout<<"Map\n";
	std::map<lock_protocol::lockid_t, int>::const_iterator it;
	for(it = mymap.begin();
	    it != mymap.end(); ++it)
	{
	    std::cout << it->first << " " << it->second << "\n";
	}
}


lock_server::lock_server():
  nacquire (0)
{
	pthread_mutex_init(&locks_mutex, NULL);
	pthread_cond_init (&locks_cond, NULL);

}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  //std::cout<<"Lock id:"<<lid<<std::endl;
  printf("acquire request from clt %d\n", clt);
  pthread_mutex_lock (&locks_mutex);
  //print_map(locks);

  if(locks.find(lid)!=locks.end() && locks[lid]==1)
  {
  	while(locks[lid]==1)
  	{
  		pthread_cond_wait(&locks_cond, &locks_mutex);	
  	}
  	
  }
  
  
  //acquire lock
  locks[lid]=1;
  //print_map(locks);

  pthread_mutex_unlock(&locks_mutex);

  
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  //std::cout<<"Lock id:"<<lid<<std::endl;
  printf("release request from clt %d\n", clt);
  pthread_mutex_lock (&locks_mutex);
  //print_map(locks);
  
  //release lock
  locks[lid]=0;
  //print_map(locks);

  pthread_cond_signal(&locks_cond);
  
  pthread_mutex_unlock(&locks_mutex);

  
  r = nacquire;
  return ret;
}