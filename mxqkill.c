
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <assert.h>

#include <sysexits.h>

#include <mysql.h>
#include <string.h>

#include "mx_log.h"
#include "mx_util.h"
#include "mx_mysql.h"
#include "mx_getopt.h"

#include "mxq_group.h"
#include "mxq_job.h"

#include "mxq.h"

static void print_usage(void)
{
    mxq_print_generic_version();
    printf(
    "\n"
    "Usage:\n"
    "  %s [options]\n"
    "  %s [options] --group-id <group-id>\n"
    "\n"
    "Synopsis:\n"
    "  kill/cancel jobs running in MXQ cluster\n"
    "\n"
    "options:\n"
    "\n"
    "  -g | --group-id <group-id>    cancel/kill group <group-id>\n"
    "\n"
    "  -v | --verbose   be more verbose\n"
    "       --debug     set debug log level (default: warning log level)\n"
    "  -V | --version\n"
    "  -h | --help\n"
    "\n"
    "Change how to connect to the mysql server:\n"
    "\n"
    "  -M | --mysql-default-file  [mysql-file]   (default: %s)\n"
    "  -S | --mysql-default-group [mysql-group]  (default: %s)\n"
    "\n"
    "Environment:\n"
    "  MXQ_MYSQL_DEFAULT_FILE   change default for [mysql-file]\n"
    "  MXQ_MYSQL_DEFAULT_GROUP  change default for [mysql-group]\n"
    "\n",
        program_invocation_short_name,
        program_invocation_short_name,
        MXQ_MYSQL_DEFAULT_FILE_STR,
        MXQ_MYSQL_DEFAULT_GROUP_STR
    );
}

static int update_group_status_cancelled(struct mx_mysql *mysql, struct mxq_group *g)
{
    struct mx_mysql_stmt *stmt = NULL;
    unsigned long long num_rows = 0;
    int res;

    assert(g->group_id);

    res = mx_mysql_statement_init(mysql, &stmt);
    if (res < 0)
        return res;

    stmt = mx_mysql_statement_prepare(mysql,
            "UPDATE mxq_group SET"
                " group_status = " status_str(MXQ_GROUP_STATUS_CANCELLED)
                " WHERE group_id = ?"
                " AND group_status = " status_str(MXQ_GROUP_STATUS_OK)
                " AND user_uid = ?"
                " AND group_jobs-group_jobs_finished-group_jobs_failed-group_jobs_cancelled-group_jobs_unknown > 0"
            );
    if (res < 0) {
        mx_log_err("mx_mysql_statement_prepare(): %m");
        mx_mysql_statement_close(&stmt);
        return -(errno=-res);
    }

    res += mx_mysql_statement_param_bind(stmt, 0, uint64, &(g->group_id));
    res += mx_mysql_statement_param_bind(stmt, 1, uint32, &(g->user_uid));
    assert(res == 0);

    res = mx_mysql_statement_execute(stmt, &num_rows);

    if (res < 0)
        mx_log_err("mx_mysql_statement_execute(): %m");

    mx_mysql_statement_close(&stmt);

    if (res < 0)
        return -(errno=-res);

    assert(num_rows <= 1);
    return (int)num_rows;
}

static int update_job_status_cancelled_by_group(struct mx_mysql *mysql, struct mxq_group *g)
{
    struct mx_mysql_stmt *stmt = NULL;
    unsigned long long num_rows = 0;
    int res;

    assert(g->group_id);

    res = mx_mysql_statement_init(mysql, &stmt);
    if (res < 0)
        return res;

    stmt = mx_mysql_statement_prepare(mysql,
            "UPDATE mxq_job SET"
                " job_status = " status_str(MXQ_JOB_STATUS_CANCELLED)
                " WHERE group_id = ?"
                " AND job_status = " status_str(MXQ_JOB_STATUS_INQ)
                " AND host_hostname = ''"
                " AND server_id = ''"
                " AND host_pid = 0"
            );
    if (res < 0) {
        mx_log_err("mx_mysql_statement_prepare(): %m");
        mx_mysql_statement_close(&stmt);
        return -(errno=-res);
    }

    res += mx_mysql_statement_param_bind(stmt, 0, uint64, &(g->group_id));
    assert(res == 0);

    res = mx_mysql_statement_execute(stmt, &num_rows);

    if (res < 0)
        mx_log_err("mx_mysql_statement_execute(): %m");

    mx_mysql_statement_close(&stmt);

    if (res < 0)
        return -(errno=-res);

    return (int)num_rows;
}


int main(int argc, char *argv[])
{
    struct mx_mysql *mysql = NULL;
    struct mxq_group group;

    uid_t ruid, euid, suid;
    struct passwd *passwd;

    int res;

    uint64_t arg_group_id;
    char     arg_debug;

    char *arg_mysql_default_group;
    char *arg_mysql_default_file;

    int i;
    int opt;
    struct mx_getopt_ctl optctl;
    struct mx_option opts[] = {
                MX_OPTION_NO_ARG("help",                 'h'),
                MX_OPTION_NO_ARG("version",              'V'),

                MX_OPTION_NO_ARG("debug",                5),
                MX_OPTION_NO_ARG("verbose",              'v'),

                MX_OPTION_REQUIRED_ARG("group-id", 'g'),

                MX_OPTION_OPTIONAL_ARG("mysql-default-file",  'M'),
                MX_OPTION_OPTIONAL_ARG("mysql-default-group", 'S'),
                MX_OPTION_END
    };

    arg_mysql_default_group = getenv("MXQ_MYSQL_DEFAULT_GROUP");
    if (!arg_mysql_default_group)
        arg_mysql_default_group = MXQ_MYSQL_DEFAULT_GROUP;

    arg_mysql_default_file  = getenv("MXQ_MYSQL_DEFAULT_FILE");
    if (!arg_mysql_default_file)
        arg_mysql_default_file = MXQ_MYSQL_DEFAULT_FILE;

    arg_group_id = 0;
    arg_debug    = 0;

    mx_log_level_set(MX_LOG_NOTICE);

    mx_getopt_init(&optctl, argc-1, &argv[1], opts);
    optctl.flags = MX_FLAG_STOPONUNKNOWN|MX_FLAG_STOPONNOOPT;

    while ((opt=mx_getopt(&optctl, &i)) != MX_GETOPT_END) {
        if (opt == MX_GETOPT_ERROR) {
            exit(EX_USAGE);
        }

        switch (opt) {
            case 'V':
                mxq_print_generic_version();
                exit(EX_USAGE);

            case 'h':
                print_usage();
                exit(EX_USAGE);

            case 5:
                arg_debug = 1;
                mx_log_level_set(MX_LOG_DEBUG);
                break;

            case 'v':
                if (!arg_debug)
                    mx_log_level_set(MX_LOG_INFO);
                break;

            case 'g':
                if (mx_strtou64(optctl.optarg, &arg_group_id) < 0 || !arg_group_id) {
                    if (!arg_group_id)
                        errno = ERANGE;
                    mx_log_err("Invalid argument for --group-id '%s': %m", optctl.optarg);
                    exit(1);
                }
                break;

            case 'M':
                arg_mysql_default_file = optctl.optarg;
                break;

            case 'S':
                arg_mysql_default_group = optctl.optarg;
                break;
        }
    }

    MX_GETOPT_FINISH(optctl, argc, argv);

    if (!arg_group_id) {
        print_usage();
        exit(EX_USAGE);
    }

    res = mx_mysql_init(&mysql);
    assert(res == 0);

    mx_mysql_option_set_default_file(mysql, arg_mysql_default_file);
    mx_mysql_option_set_default_group(mysql, arg_mysql_default_group);

    res = mx_mysql_connect_forever(&mysql);
    assert(res == 0);

    mx_log_info("MySQL: Connection to database established.");

    if (arg_group_id) {
        memset(&group, 0, sizeof(group));

        res = getresuid(&ruid, &euid, &suid);
        assert(res != -1);

        passwd = getpwuid(ruid);
        assert(passwd != NULL);

        group.group_id  = arg_group_id;
        group.user_uid  = ruid;
        group.user_name = passwd->pw_name;

        res = update_group_status_cancelled(mysql, &group);

        if (res <= 0) {
            mx_mysql_finish(&mysql);
            mx_log_info("MySQL: Connection to database closed.");

            if (res == 0)
                mx_log_warning("no active group with group_id=%lu found for user=%s(%d)",
                        group.group_id, group.user_name, group.user_uid);
            else
                mx_log_err("cancelling group failed: %m");

            return 1;
        }

        assert(res == 1);

        res = update_job_status_cancelled_by_group(mysql, &group);

        mx_mysql_finish(&mysql);
        mx_log_info("MySQL: Connection to database closed.");

        if (res == -1 && errno == ENOENT)
            res=0;

        if (res >= 0) {
            mx_log_notice("cancelled %d jobs in group with group_id=%lu",
                    res, group.group_id);
            mx_log_notice("marked all running jobs in group with group_id=%lu to be killed by executing servers.",
                    group.group_id);
            mx_log_notice("deactivated group with group_id=%lu",
                    group.group_id);
            return 0;
        } else {
            mx_log_err("cancelling jobs failed: %m");
        }
    }

    return 1;
}

