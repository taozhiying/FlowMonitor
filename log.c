#include "log.h"

/* get total line nums of a file 
 * err return -1, ok return line nums
 */
int util_get_file_count_lines(char *file_name)
{
    char c, lc;
    int line_nums = 0;
    lc = 0;

    /* get file nums */
    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        return -1;
    }
    while((c = fgetc(fp)) != EOF) {
        if (c == '\n') line_nums++;
        lc = c;
    }
    if (lc != '\n') line_nums++;
    fclose(fp);
    return line_nums;
}

/* delete file content, but only keep the last n lines
 *         n = #keep_line
 */
int log_delete_file_line(char *file_name, int keep_line)
{
    int line_nums = util_get_file_count_lines(file_name);
    char cmd[256];

    if (line_nums > keep_line) {
        snprintf(cmd, sizeof(cmd), "sed -i \'1,%dd\' %s", line_nums - keep_line,  file_name);
        if (system1(cmd)) {
            LOGERR("run cmd err.[cmd : %s]\n", cmd);
            return -1;
        }
    }
    return 0;
}

int recod_log_once(char *log_file, char *log)
{
    char date_str[128] = {0};
    char buf[1024] = {0};

    LOGMSG("%s", log);
    
    FILE *fp = fopen(log_file,"a+");
    if(!fp) {
        LOGERR("open log file [%s] err.[%m]\n", log_file);
        return -1;
    }
    get_currnet_date_str(date_str, sizeof(date_str));
    snprintf(buf, sizeof(buf), "%s %s",date_str,log);
    fwrite(buf, 1, strlen(buf), fp);
    fclose(fp);
    return 0;
}

void log_record(char *log_file, char *log)
{
    int line_nums = util_get_file_count_lines(log_file);

    if (line_nums > MAX_LOG_LINE || line_nums == -1) {
        log_delete_file_line(log_file, LOG_DEL_LINE);
    }
    
    recod_log_once(log_file, log);
}


