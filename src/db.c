#include "db.h"

dbi_conn conn = NULL;

#if defined(LIBDBI_VERSION) && LIBDBI_VERSION >= 900
static dbi_inst dbi_instance;
# define dbi_initialize(dir) dbi_initialize_r(dir, &dbi_instance)
# define dbi_driver_open(name) dbi_driver_open_r(name, dbi_instance)
# define dbi_shutdown() do { dbi_shutdown_r(dbi_instance); dbi_instance = NULL; } while(0)
# define dbi_conn_new(name) dbi_conn_new_r(name, dbi_instance)
#endif

int init_db()
{
	host *temp_host = NULL;
	service *temp_service = NULL;
	dbi_result result;

	dbi_initialize(NULL);
	conn = dbi_conn_new("mysql");
	dbi_conn_set_option(conn, "host", "localhost");
	dbi_conn_set_option(conn, "username", "db_status_user");
	dbi_conn_set_option(conn, "password", "password");
	dbi_conn_set_option(conn, "dbname", "db_status");
	dbi_conn_set_option(conn, "encoding", "UTF-8");

	if (dbi_conn_connect(conn) < 0) {
		nm_log(NSLOG_INFO_MESSAGE, "DB Status: Connection to database failed");
		return -1;
	}

	result = dbi_conn_queryf(conn, "TRUNCATE TABLE hosts");
	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Table `hosts` truncated");
	free(result);

	/* Add initial state of all hosts to the database */
	for (temp_host = host_list; temp_host != NULL; temp_host = temp_host->next) {
		insert_host(temp_host);
		nm_log(NSLOG_INFO_MESSAGE, "DB Status: Inserting host %s into table hosts", temp_host->name);
	}

	result = dbi_conn_queryf(conn, "TRUNCATE TABLE services");
	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Table `services` truncated");
	free(result);

	/* Add initial state of all services to the database */
	for (temp_service = service_list; temp_service != NULL; temp_service = temp_service->next) {
		insert_service(temp_service);
		nm_log(NSLOG_INFO_MESSAGE, "DB Status: Inserting service %s;%s into table services", temp_service->description, temp_service->host_name);
	}

	/*
	dbi_result result;
	result = dbi_conn_queryf(conn, "SELECT * FROM report_data");

	if (result) {
		nm_log(NSLOG_INFO_MESSAGE, "DB Status: Found results!");
		while (dbi_result_next_row(result)) {
			char *name = dbi_result_get_string(result, "host_name");
			printf("hostname %s\n", name);
		}
		dbi_result_free(result);
	} else {
		nm_log(NSLOG_INFO_MESSAGE, "DB Status: No results found!");
	}

	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Cleaning old data");
	nm_log(NSLOG_INFO_MESSAGE, "DB Status: Adding objects");

	*/
	neb_deregister_callback(NEBCALLBACK_PROCESS_DATA, init_db);

	return 0;
}

int insert_host(host *hst)
{
	dbi_result res;
	res = dbi_conn_queryf(conn, "INSERT INTO hosts (id, name, display_name, alias, address, check_command, initial_state, check_interval, retry_interval, max_attempts, event_handler, notification_interval, first_notification_delay, notification_options, hourly_value, notification_period, check_period, flap_detection_enabled, low_flap_threshold, high_flap_threshold, flap_detection_options, stalking_options, check_freshness, freshness_threshold, process_performance_data, checks_enabled, check_source, accept_passive_checks, event_handler_enabled, retain_status_information, retain_nonstatus_information, obsess, notes, notes_url, action_url, icon_image, icon_image_alt, statusmap_image, vrml_image, have_2d_coords, x_2d, y_2d, have_3d_coords, x_3d, y_3d, z_3d, problem_has_been_acknowledged, acknowledgement_type, check_type, current_state, last_state, last_hard_state, plugin_output, long_plugin_output, perf_data, state_type, current_attempt, current_event_id, last_event_id, current_problem_id, last_problem_id, latency, execution_time, is_executing, check_options, notifications_enabled, last_notification, next_notification, next_check, last_check, last_state_change, last_hard_state_change, last_time_up, last_time_down, last_time_unreachable, has_been_checked, is_being_freshened, notified_on, current_notification_number, no_more_notifications, current_notification_id, check_flapping_recovery_notification, scheduled_downtime_depth, pending_flex_downtime, state_history_index, last_state_history_update, is_flapping, flapping_comment_id, percent_state_change, total_services, modified_attributes) VALUES (%u, '%s', '%s', '%s', '%s', '%s', %i, %f, %f, %i, '%s', %f, %f, %u, %u, '%s', '%s', %i, %f, %f, %i, %u, %i, %i, %i, %i, '%s', %i, %i, %i, %i, %i, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %i, %i, %i, %i, %f, %f, %f, %i, %i, %i, %i, %i, %i, '%s', '%s', '%s', %i, %i, %ld, %ld, %ld, %ld, %f, %f, %i, %i, %i, '%lld', '%lld', '%lld', '%lld', '%lld', '%lld', '%lld', '%lld', '%lld', %i, %i, %i, %i, %i, %ld, %i, %i, %i, %i, '%lld', %i, %ld, %f, %i, %ld)", hst->id, hst->name, hst->display_name, hst->alias, hst->address, hst->check_command, hst->initial_state, hst->check_interval, hst->retry_interval, hst->max_attempts, hst->event_handler, hst->notification_interval, hst->first_notification_delay, hst->notification_options, hst->hourly_value, hst->notification_period, hst->check_period, hst->flap_detection_enabled, hst->low_flap_threshold, hst->high_flap_threshold, hst->flap_detection_options, hst->stalking_options, hst->check_freshness, hst->freshness_threshold, hst->process_performance_data, hst->checks_enabled, hst->check_source, hst->accept_passive_checks, hst->event_handler_enabled, hst->retain_status_information, hst->retain_nonstatus_information, hst->obsess, hst->notes, hst->notes_url, hst->action_url, hst->icon_image, hst->icon_image_alt, hst->statusmap_image, hst->vrml_image, hst->have_2d_coords, hst->x_2d, hst->y_2d, hst->have_3d_coords, hst->x_3d, hst->y_3d, hst->z_3d, hst->problem_has_been_acknowledged, hst->acknowledgement_type, hst->check_type, hst->current_state, hst->last_state, hst->last_hard_state, hst->plugin_output, hst->long_plugin_output, hst->perf_data, hst->state_type, hst->current_attempt, hst->current_event_id, hst->last_event_id, hst->current_problem_id, hst->last_problem_id, hst->latency, hst->execution_time, hst->is_executing, hst->check_options, hst->notifications_enabled, hst->last_notification, hst->next_notification, hst->next_check, hst->last_check, hst->last_state_change, hst->last_hard_state_change, hst->last_time_up, hst->last_time_down, hst->last_time_unreachable, hst->has_been_checked, hst->is_being_freshened, hst->notified_on, hst->current_notification_number, hst->no_more_notifications, hst->current_notification_id, hst->check_flapping_recovery_notification, hst->scheduled_downtime_depth, hst->pending_flex_downtime, hst->state_history_index, hst->last_state_history_update, hst->is_flapping, hst->flapping_comment_id, hst->percent_state_change, hst->total_services, hst->modified_attributes);
	free(res);
	return 0;
}

int insert_service(service *svc)
{
	dbi_result res;
	res = dbi_conn_queryf(conn, "INSERT INTO services (id, host_name, description, display_name, check_command, event_handler, initial_state, check_interval, retry_interval, max_attempts, notification_interval, first_notification_delay, notification_options, stalking_options, hourly_value, is_volatile, notification_period, check_period, flap_detection_enabled, low_flap_threshold, high_flap_threshold, flap_detection_options, process_performance_data, check_freshness, freshness_threshold, accept_passive_checks, event_handler_enabled, checks_enabled, check_source, retain_status_information, retain_nonstatus_information, notifications_enabled, obsess, notes, notes_url, action_url, icon_image, icon_image_alt, problem_has_been_acknowledged, acknowledgement_type, host_problem_at_last_check, check_type, current_state, last_state, last_hard_state, plugin_output, long_plugin_output, perf_data, state_type, next_check, last_check, current_attempt, current_event_id, last_event_id, current_problem_id, last_problem_id, last_notification, next_notification, no_more_notifications, check_flapping_recovery_notification, last_state_change, last_hard_state_change, last_time_ok, last_time_warning, last_time_unknown, last_time_critical, has_been_checked, is_being_freshened, notified_on, current_notification_number, current_notification_id, latency, execution_time, is_executing, check_options, scheduled_downtime_depth, pending_flex_downtime, state_history_index, is_flapping, flapping_comment_id, percent_state_change, modified_attributes, event_handler_args) VALUES (%u, '%s', '%s', '%s', '%s', '%s', %i, %f, %f, %i, %f, %f, %u, %u, %u, %i, '%s', '%s', %i, %f, %f, %u, %i, %i, %i, %i, %i, %i, '%s', %i, %i, %i, %i, '%s', '%s', '%s', '%s', '%s', %i, %i, %i, %i, %i, %i, %i, '%s', '%s', '%s', %i, '%lld', '%lld', %i, %ld, %ld, %ld, %ld, '%lld', '%lld', %i, %i, '%lld', '%lld', '%lld', '%lld', '%lld', '%lld', %i, %i, %u, %i, %ld, %f, %f, %i, %i, %i, %i, %i, %i, %ld, %f, %ld, '%s')", svc->id, svc->host_name, svc->description, svc->display_name, svc->check_command, svc->event_handler, svc->initial_state, svc->check_interval, svc->retry_interval, svc->max_attempts, svc->notification_interval, svc->first_notification_delay, svc->notification_options, svc->stalking_options, svc->hourly_value, svc->is_volatile, svc->notification_period, svc->check_period, svc->flap_detection_enabled, svc->low_flap_threshold, svc->high_flap_threshold, svc->flap_detection_options, svc->process_performance_data, svc->check_freshness, svc->freshness_threshold, svc->accept_passive_checks, svc->event_handler_enabled, svc->checks_enabled, svc->check_source, svc->retain_status_information, svc->retain_nonstatus_information, svc->notifications_enabled, svc->obsess, svc->notes, svc->notes_url, svc->action_url, svc->icon_image, svc->icon_image_alt, svc->problem_has_been_acknowledged, svc->acknowledgement_type, svc->host_problem_at_last_check, svc->check_type, svc->current_state, svc->last_state, svc->last_hard_state, svc->plugin_output, svc->long_plugin_output, svc->perf_data, svc->state_type, svc->next_check, svc->last_check, svc->current_attempt, svc->current_event_id, svc->last_event_id, svc->current_problem_id, svc->last_problem_id, svc->last_notification, svc->next_notification, svc->no_more_notifications, svc->check_flapping_recovery_notification, svc->last_state_change, svc->last_hard_state_change, svc->last_time_ok, svc->last_time_warning, svc->last_time_unknown, svc->last_time_critical, svc->has_been_checked, svc->is_being_freshened, svc->notified_on, svc->current_notification_number, svc->current_notification_id, svc->latency, svc->execution_time, svc->is_executing, svc->check_options, svc->scheduled_downtime_depth, svc->pending_flex_downtime, svc->state_history_index, svc->is_flapping, svc->flapping_comment_id, svc->percent_state_change, svc->modified_attributes, svc->event_handler_args);
	free(res);
	return 0;
}

int update_host(host *hst)
{
	dbi_result res;
	res = dbi_conn_queryf(conn, "UPDATE hosts SET id = %u, name = '%s', display_name = '%s', alias = '%s', address = '%s', check_command = '%s', initial_state = %i, check_interval = %f, retry_interval = %f, max_attempts = %i, event_handler = '%s', notification_interval = %f, first_notification_delay = %f, notification_options = %u, hourly_value = %u, notification_period = '%s', check_period = '%s', flap_detection_enabled = %i, low_flap_threshold = %f, high_flap_threshold = %f, flap_detection_options = %i, stalking_options = %u, check_freshness = %i, freshness_threshold = %i, process_performance_data = %i, checks_enabled = %i, check_source = '%s', accept_passive_checks = %i, event_handler_enabled = %i, retain_status_information = %i, retain_nonstatus_information = %i, obsess = %i, notes = '%s', notes_url = '%s', action_url = '%s', icon_image = '%s', icon_image_alt = '%s', statusmap_image = '%s', vrml_image = '%s', have_2d_coords = %i, x_2d = %i, y_2d = %i, have_3d_coords = %i, x_3d = %f, y_3d = %f, z_3d = %f, problem_has_been_acknowledged = %i, acknowledgement_type = %i, check_type = %i, current_state = %i, last_state = %i, last_hard_state = %i, plugin_output = '%s', long_plugin_output = '%s', perf_data = '%s', state_type = %i, current_attempt = %i, current_event_id = %ld, last_event_id = %ld, current_problem_id = %ld, last_problem_id = %ld, latency = %f, execution_time = %f, is_executing = %i, check_options = %i, notifications_enabled = %i, last_notification = '%lld', next_notification = '%lld', next_check = '%lld', last_check = '%lld', last_state_change = '%lld', last_hard_state_change = '%lld', last_time_up = '%lld', last_time_down = '%lld', last_time_unreachable = '%lld', has_been_checked = %i, is_being_freshened = %i, notified_on = %i, current_notification_number = %i, no_more_notifications = %i, current_notification_id = %ld, check_flapping_recovery_notification = %i, scheduled_downtime_depth = %i, pending_flex_downtime = %i, state_history_index = %i, last_state_history_update = '%lld', is_flapping = %i, flapping_comment_id = %ld, percent_state_change = %f, total_services = %i, modified_attributes = %ld WHERE name = '%s'", hst->id, hst->name, hst->display_name, hst->alias, hst->address, hst->check_command, hst->initial_state, hst->check_interval, hst->retry_interval, hst->max_attempts, hst->event_handler, hst->notification_interval, hst->first_notification_delay, hst->notification_options, hst->hourly_value, hst->notification_period, hst->check_period, hst->flap_detection_enabled, hst->low_flap_threshold, hst->high_flap_threshold, hst->flap_detection_options, hst->stalking_options, hst->check_freshness, hst->freshness_threshold, hst->process_performance_data, hst->checks_enabled, hst->check_source, hst->accept_passive_checks, hst->event_handler_enabled, hst->retain_status_information, hst->retain_nonstatus_information, hst->obsess, hst->notes, hst->notes_url, hst->action_url, hst->icon_image, hst->icon_image_alt, hst->statusmap_image, hst->vrml_image, hst->have_2d_coords, hst->x_2d, hst->y_2d, hst->have_3d_coords, hst->x_3d, hst->y_3d, hst->z_3d, hst->problem_has_been_acknowledged, hst->acknowledgement_type, hst->check_type, hst->current_state, hst->last_state, hst->last_hard_state, hst->plugin_output, hst->long_plugin_output, hst->perf_data, hst->state_type, hst->current_attempt, hst->current_event_id, hst->last_event_id, hst->current_problem_id, hst->last_problem_id, hst->latency, hst->execution_time, hst->is_executing, hst->check_options, hst->notifications_enabled, hst->last_notification, hst->next_notification, hst->next_check, hst->last_check, hst->last_state_change, hst->last_hard_state_change, hst->last_time_up, hst->last_time_down, hst->last_time_unreachable, hst->has_been_checked, hst->is_being_freshened, hst->notified_on, hst->current_notification_number, hst->no_more_notifications, hst->current_notification_id, hst->check_flapping_recovery_notification, hst->scheduled_downtime_depth, hst->pending_flex_downtime, hst->state_history_index, hst->last_state_history_update, hst->is_flapping, hst->flapping_comment_id, hst->percent_state_change, hst->total_services, hst->modified_attributes);
	free(res);
	return 0;
}

int update_service(service *svc)
{
	dbi_result res;
	res = dbi_conn_queryf(conn, "UPDATE services SET id = %u, host_name = '%s', description = '%s', display_name = '%s', check_command = '%s', event_handler = '%s', initial_state = %i, check_interval = %f, retry_interval = %f, max_attempts = %i, notification_interval = %f, first_notification_delay = %f, notification_options = %u, stalking_options = %u, hourly_value = %u, is_volatile = %i, notification_period = '%s', check_period = '%s', flap_detection_enabled = %i, low_flap_threshold = %f, high_flap_threshold = %f, flap_detection_options = %u, process_performance_data = %i, check_freshness = %i, freshness_threshold = %i, accept_passive_checks = %i, event_handler_enabled = %i, checks_enabled = %i, check_source = '%s', retain_status_information = %i, retain_nonstatus_information = %i, notifications_enabled = %i, obsess = %i, notes = '%s', notes_url = '%s', action_url = '%s', icon_image = '%s', icon_image_alt = '%s', problem_has_been_acknowledged = %i, acknowledgement_type = %i, host_problem_at_last_check = %i, check_type = %i, current_state = %i, last_state = %i, last_hard_state = %i, plugin_output = '%s', long_plugin_output = '%s', perf_data = '%s', state_type = %i, next_check = '%lld', last_check = '%lld', current_attempt = %i, current_event_id = %ld, last_event_id = %ld, current_problem_id = %ld, last_problem_id = %ld, last_notification = '%lld', next_notification = '%lld', no_more_notifications = %i, check_flapping_recovery_notification = %i, last_state_change = '%lld', last_hard_state_change = '%lld', last_time_ok = '%lld', last_time_warning = '%lld', last_time_unknown = '%lld', last_time_critical = '%lld', has_been_checked = %i, is_being_freshened = %i, notified_on = %u, current_notification_number = %i, current_notification_id = %ld, latency = %f, execution_time = %f, is_executing = %i, check_options = %i, scheduled_downtime_depth = %i, pending_flex_downtime = %i, state_history_index = %i, is_flapping = %i, flapping_comment_id = %ld, percent_state_change = %f, modified_attributes = %ld, event_handler_args = '%s' WHERE host_name = '%s' AND description = '%s'", svc->id, svc->host_name, svc->description, svc->display_name, svc->check_command, svc->event_handler, svc->initial_state, svc->check_interval, svc->retry_interval, svc->max_attempts, svc->notification_interval, svc->first_notification_delay, svc->notification_options, svc->stalking_options, svc->hourly_value, svc->is_volatile, svc->notification_period, svc->check_period, svc->flap_detection_enabled, svc->low_flap_threshold, svc->high_flap_threshold, svc->flap_detection_options, svc->process_performance_data, svc->check_freshness, svc->freshness_threshold, svc->accept_passive_checks, svc->event_handler_enabled, svc->checks_enabled, svc->check_source, svc->retain_status_information, svc->retain_nonstatus_information, svc->notifications_enabled, svc->obsess, svc->notes, svc->notes_url, svc->action_url, svc->icon_image, svc->icon_image_alt, svc->problem_has_been_acknowledged, svc->acknowledgement_type, svc->host_problem_at_last_check, svc->check_type, svc->current_state, svc->last_state, svc->last_hard_state, svc->plugin_output, svc->long_plugin_output, svc->perf_data, svc->state_type, svc->next_check, svc->last_check, svc->current_attempt, svc->current_event_id, svc->last_event_id, svc->current_problem_id, svc->last_problem_id, svc->last_notification, svc->next_notification, svc->no_more_notifications, svc->check_flapping_recovery_notification, svc->last_state_change, svc->last_hard_state_change, svc->last_time_ok, svc->last_time_warning, svc->last_time_unknown, svc->last_time_critical, svc->has_been_checked, svc->is_being_freshened, svc->notified_on, svc->current_notification_number, svc->current_notification_id, svc->latency, svc->execution_time, svc->is_executing, svc->check_options, svc->scheduled_downtime_depth, svc->pending_flex_downtime, svc->state_history_index, svc->is_flapping, svc->flapping_comment_id, svc->percent_state_change, svc->modified_attributes, svc->event_handler_args, svc->host_name, svc->description);
	free(res);
	return 0;
}