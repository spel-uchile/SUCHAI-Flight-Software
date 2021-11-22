//
// Created by carlgonz 2020-02-24
//

#include "app/system/taskTest.h"

static const char* tag = "sgp4_test_task";
static char current_dir[127] = CURRENT_DIR;

// The task to run the test
void taskTest(void* param)
{
    char* input_file = (char*) param;
    FILE *base_file = NULL;

    LOGI(tag, "Started");
    LOGI(tag, "----SGP4 propagator test ----");

    cmd_t *tle1 = cmd_get_str("tle_set");
    cmd_add_params_str(tle1, "1 42788U 17036Z   20268.81967680  .00001527  00000-0  65841-4 0  9992");
    cmd_send(tle1);

    cmd_t *tle2 = cmd_get_str("tle_set");
    cmd_add_params_str(tle2, "2 42788  97.2921 320.6805 0012366  42.2734 317.9454 15.23993159181016");
    cmd_send(tle2);

    cmd_t *tle_u = cmd_get_str("tle_update");
    cmd_send(tle_u);
    tle_u = cmd_get_str("tle_get");
    cmd_send(tle_u);

    char line[255];
    char fname_data[255];
    sprintf(fname_data, "%s/data.csv", current_dir);
    LOGI(tag,fname_data);
    base_file = fopen(fname_data, "r");
    char fname_test[255];
    sprintf(fname_test, "%s/test_%ld.csv", current_dir, time(NULL));
    LOGI(tag, fname_test)

    time_t dt;
    double r[3];
    double v[3];
    double lat, lon, alt, gst;

    LOGI(tag, "---- Sending PROP Commands ----");
    while(fgets(line, 255, base_file) != NULL)
    {
        int npar = sscanf(line, "%ld,%lf,%lf,%lf,%lf,%lf,%lf",
                          &dt, r, r+1, r+2, v, v+1, v+2);
        cmd_t *cmd_prop = cmd_get_str("_test_tle_prop");
        cmd_add_params_var(cmd_prop, dt, fname_test);
        cmd_send(cmd_prop);
    }
    fclose(base_file);

    LOGI(tag, "---- Sending CMP Command ----");
    cmd_t *cmd_prop_test = cmd_get_str("_test_tle_comp");
    cmd_add_params_var(cmd_prop_test, fname_data, fname_test);
    cmd_send(cmd_prop_test);

    LOGI(tag, "---- Sending Exit Command ----");
    cmd_t *cmd_exit = cmd_get_str("obc_reset");
    cmd_send(cmd_exit);
}
