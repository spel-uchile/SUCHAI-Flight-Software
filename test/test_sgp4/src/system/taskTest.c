//
// Created by carlgonz 2020-02-24
//

#include "taskTest.h"

static const char* tag = "sgp4_test_task";
extern TLE tle;

// 2-norm distance for two 3D vectors
double dist(double *v1, double *v2)
{
    double dist = 0;
    double tmp = 0;

    // just unroll the loop
    tmp = v1[0]-v2[0];
    dist += tmp*tmp;
    tmp = v1[1]-v2[1];
    dist += tmp*tmp;
    tmp = v1[2]-v2[2];
    dist += tmp*tmp;

    return sqrt(dist);
}

// A command that propagate the TLE to given epoch and save result to file
int _test_tle_prop(char *fmt, char *params, int nparams)
{
    double r[3];  // Sat position in ECI frame
    double v[3];  // Sat velocity in ECI frame
    uint64_t ts = 0;
    FILE *file;
    char filename[SCH_BUFF_MAX_LEN];

    assert(!(params != NULL && sscanf(params, fmt, &ts, filename) != nparams));
        //return CMD_ERROR;

    if(ts == 0)
        ts = (uint64_t) dat_get_time();

    file = fopen(filename, "a");
    assert(!(file == NULL));
        //return CMD_ERROR;

    portTick init_time = osTaskGetTickCount();
    double diff = (double)ts - (double)tle.epoch/1000.0;
    diff /= 60.0;
    getRV(&tle,diff,r,v);
    //getRVForDate(&tle, ts*1000, r, v);
    portTick getrv_time = osTaskGetTickCount();

    LOGD(tag, "T : %.6f - %.6f = %.6f", (double)ts, tle.epoch/1000.0, diff);
    LOGD(tag, "R : (%.6f, %.6f, %.6f)", r[0], r[1], r[2]);
    LOGD(tag, "V : (%.6f, %.6f, %.6f)", v[0], v[1], v[2]);
    LOGD(tag, "Er: %d", tle.rec.error);

    fprintf(file, "%ld,%05.8f,%05.8f,%05.8f,%05.8f,%05.8f,%05.8f\n", ts, r[0], r[1], r[2], v[0], v[1], v[2]);
    fclose(file);
    portTick final_time = osTaskGetTickCount();

    LOGI(tag, "getRVForDate: %.06f ms", (getrv_time-init_time)/1000.0);
    LOGI(tag, "Total time  : %.06f ms", (final_time-init_time)/1000.0);
    return CMD_OK;
}

// A command that compares the reference values with calculated ones
int _test_tle_cmp(char *fmt, char *params, int nparams)
{
    time_t t, tv;
    FILE *file_data, *file_test;
    char fname_data[SCH_BUFF_MAX_LEN];
    char fname_test[SCH_BUFF_MAX_LEN];
    char line_data[SCH_BUFF_MAX_LEN];
    char line_test[SCH_BUFF_MAX_LEN];
    double r[3], rv[3];  // Sat position in ECI frame
    double v[3], vv[3];  // Sat velocity in ECI frame
    double rdist, vdist, lat, lon, alt, gst;
    double rerr = 0;
    double verr = 0;
    int cnt = 0, n;

    assert(!(params != NULL && sscanf(params, fmt, fname_data, fname_test) != nparams));
        //return CMD_ERROR;

    file_data = fopen(fname_data, "r");
    file_test = fopen(fname_test, "r");
    assert(!(file_data == NULL || file_test == NULL));
        //return CMD_ERROR;

    while(fgets(line_data, SCH_BUFF_MAX_LEN, file_data) != NULL &&
            fgets(line_test, SCH_BUFF_MAX_LEN, file_test) != NULL)
    {
        sscanf(line_data, "%ld,%lf,%lf,%lf,%lf,%lf,%lf",
               &tv, rv, rv+1, rv+2, vv, vv+1, vv+2);
        sscanf(line_test, "%ld,%lf,%lf,%lf,%lf,%lf,%lf",
               &t, r, r+1, r+2, v, v+1, v+2);
        assertf(t==tv, tag, "%d != %d (%s)", t, tv, line_test);

        rdist = dist(r,rv);
        vdist = dist(v,vv);
        rerr += rdist;
        verr += vdist;
        cnt++;
        if(rdist > 10e-3 || vdist > 10e-4)
        {
            printf("%d %ld %e %e\n", tle.objectNum, tv, rdist, vdist);
            printf("RV: %.06lf %.06lf %.06lf\n", rv[0], rv[1], rv[2]);
            printf("R : %.06lf %.06lf %.06lf\n", r[0], r[1], r[2]);
            printf("VV: %.06lf %.06lf %.06lf\n", vv[0], vv[1], vv[2]);
            printf("V : %.06lf %.06lf %.06lf\n", v[0], v[1], v[2]);
        }
    }

    fclose(file_data);
    fclose(file_test);

    rerr = rerr/cnt;
    verr = verr/cnt;

    LOGI(tag, "Typical errors r=%e mm, v=%e mm/s", 1e6*rerr, 1e6*verr);
    assert(1e6*rerr < 10e3 && 1e6*verr < 10);
    return CMD_OK;
}

// The task to run the test
void taskTest(void* param)
{
    cmd_add("_test_tle_prop", _test_tle_prop, "%ld %s", 2);
    cmd_add("_test_tle_comp", _test_tle_cmp, "%s %s", 2);

    char* input_file = (char*) param;
    FILE *base_file = NULL;

    LOGI(tag, "Started");
    LOGI(tag, "----SGP4 propagator test ----");

    cmd_t *tle1 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle1, "1 42788U 17036Z   20268.81967680  .00001527  00000-0  65841-4 0  9992");
    cmd_send(tle1);

    cmd_t *tle2 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle2, "2 42788  97.2921 320.6805 0012366  42.2734 317.9454 15.23993159181016");
    cmd_send(tle2);

    cmd_t *tle_u = cmd_get_str("obc_update_tle");
    cmd_send(tle_u);
    tle_u = cmd_get_str("obc_get_tle");
    cmd_send(tle_u);

    char line[255];
    char *fname_data = "../data.csv";
    base_file = fopen(fname_data, "r");
    char fname_test[255];
    sprintf(fname_test, "test_%ld.csv", time(NULL));

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
