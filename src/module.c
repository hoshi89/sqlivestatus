#include "module.h"

NEB_API_VERSION(CURRENT_NEB_API_VERSION);
nebmodule *db_neb_handle;

int handle_process_data(int cb, void *data)
{
	nebstruct_process_data *ds = (nebstruct_process_data *)data;

	if (ds->type == NEBTYPE_PROCESS_START) {
		init_db();
	}

	return NEB_OK;
}

int handle_host_status_data(int cb, void *data)
{
	nebstruct_host_status_data *ds = (nebstruct_host_status_data *)data;
	host *hst = (host *)ds->object_ptr;
	update_host(hst);
	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Updating DB for host %s", hst->name);
}

int handle_service_status_data(int cb, void *data)
{
	nebstruct_service_status_data *ds = (nebstruct_service_status_data *)data;
	service *svc = (service *)ds->object_ptr;
	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Updating DB for service %s;%s", svc->description, svc->host_name);
}

int nebmodule_init (__attribute__((unused)) int flags, char *arg, nebmodule *handle)
{
	db_neb_handle = handle;
	nm_log(NSLOG_INFO_MESSAGE, "Initializing DB Status module");

    neb_register_callback(NEBCALLBACK_PROCESS_DATA, db_neb_handle, 0, handle_process_data);
    neb_register_callback(NEBCALLBACK_HOST_STATUS_DATA, db_neb_handle, 0, handle_host_status_data);
	neb_register_callback(NEBCALLBACK_SERVICE_STATUS_DATA, db_neb_handle, 0, handle_service_status_data);

    nm_log(NSLOG_INFO_MESSAGE, "DB Status module initialized");

	return 0;
}

int nebmodule_deinit (__attribute__((unused)) int flags, __attribute__((unused)) int reason)
{
	nm_log(NSLOG_INFO_MESSAGE, "Deinitializing DB Status module");
	nm_log(NSLOG_INFO_MESSAGE, "DB Status module deinitialized");
}
