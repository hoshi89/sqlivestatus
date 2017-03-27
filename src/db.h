#ifndef INCLUDED_DB
#define INCLUDED_DB

#include <naemon/naemon.h>
#include <dbi/dbi.h>



int init_db();
int insert_host(host *hst);
int insert_service(service *svc);
int update_host(host *hst);
int update_service(service *svc);

#endif