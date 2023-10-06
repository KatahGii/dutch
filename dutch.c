#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>
#include <locale.h>

void print_usage() {
    printf("Usage: dutch [OPTION] FILE...\n");
    printf("Update the access and modification times of each FILE to the current time.\n\n");
    printf("Type 'dutch --help' to see detailed usage information.\n");
    exit(1);
}

void print_version() {
    printf("dutch version 1.0.0, an almost drop-in replacement for touch\n");
    printf("Copyright (C) 2023 Katahgii\n");
    exit(0);
}

void touch_file(const char *filename, int no_create, struct utimbuf new_times, int silent, int no_dereference) {
    struct stat st = {0};

    if (no_dereference) {
        if (lstat(filename, &st) == -1) {
            if (!silent) {
                fprintf(stderr, "Error: File %s does not exist and no-create option is set\n", filename);
            }
            exit(1);
        }
        struct timeval tv[2];
        tv[0].tv_sec = new_times.actime;
        tv[0].tv_usec = 0;
        tv[1].tv_sec = new_times.modtime;
        tv[1].tv_usec = 0;
        if (utimes(filename, tv) != 0 && !silent) {
            perror("Error updating timestamp");
            exit(1);
        }
    } else {
        if (stat(filename, &st) == -1) {
            if (no_create) {
                if (!silent) {
                    fprintf(stderr, "Error: File %s does not exist and no-create option is set\n", filename);
                }
                exit(1);
            }
            FILE *file = fopen(filename, "w");
            if (!file) {
                if (!silent) {
                    perror("Error creating file");
                }
                exit(1);
            }
            fclose(file);
        }
        if (utime(filename, &new_times) != 0 && !silent) {
            perror("Error updating timestamp");
            exit(1);
        }
    }
}

void display_help_with_pager() {
    FILE *pager;
    const char *pager_cmd = "less";

    if ((pager = popen(pager_cmd, "w")) == NULL) {
        perror("Error opening pager");
        exit(EXIT_FAILURE);
    }

    fprintf(pager, "Usage: dutch [OPTION]... FILE...\n");
    fprintf(pager, "Update the access and modification times of each FILE to the current time.\n\n");
    fprintf(pager, " -a, --time=access        Change only the access time.\n");
    fprintf(pager, " -m, --time=modify        Change only the modification time.\n");
    fprintf(pager, " -c, --no-create          Do not create any files that do not exist.\n");
    fprintf(pager, " -f                       Ignored (for backward compatibility).\n");
    fprintf(pager, " -d, --date=STRING        Use STRING as the time instead of the current time.\n");
    fprintf(pager, " -t STAMP                 Use [[CC]YY]MMDDhhmm[.ss] instead of the current time.\n");
    fprintf(pager, " -r, --reference=FILE     Use FILE's times instead of the current time.\n");
    fprintf(pager, " -h, --no-dereference     Affect symbolic links instead of referenced files.\n");
    fprintf(pager, " -s, --silent             Silent mode; do not display errors.\n");
    fprintf(pager, " --help                   Display this help and exit.\n");
    fprintf(pager, " --version                Display version information and exit.\n\n");
    fprintf(pager, "Examples:\n");
    fprintf(pager, "  dutch file.txt\n");
    fprintf(pager, "  dutch -m file.txt\n");
    fprintf(pager, "  dutch -d \"2023-04-01 12:34:56\" file1.txt file2.txt\n");

    pclose(pager);
}

int main(int argc, char *argv[]) {
    int change_access_time = 1;
    int change_modification_time = 1;
    int no_create = 0;
    int silent = 0;
    int no_dereference = 0;
    char *timestamp = NULL;
    char *reference_file = NULL;
    struct utimbuf new_times;

    setlocale(LC_ALL, "");

    static struct option long_options[] = {
        {"time", required_argument, 0, 0},
        {"access", no_argument, 0, 'a'},
        {"modification", no_argument, 0, 'm'},
        {"no-create", no_argument, 0, 'c'},
        {"date", required_argument, 0, 'd'},
        {"reference", required_argument, 0, 'r'},
        {"no-dereference", no_argument, 0, 'h'},
        {"silent", no_argument, 0, 's'},
        {"help", no_argument, 0, 0},
        {"version", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;
    while ((opt = getopt_long(argc, argv, "amcf:d:t:r:sh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                if (strcmp(long_options[option_index].name, "help") == 0) {
                    display_help_with_pager();
                    exit(0);
                } else if (strcmp(long_options[option_index].name, "version") == 0) {
                    print_version();
                } else if (strcmp(long_options[option_index].name, "time") == 0) {
                    if (strcmp(optarg, "access") == 0) {
                        change_modification_time = 0;
                    } else if (strcmp(optarg, "modify") == 0) {
                        change_access_time = 0;
                    } else {
                        fprintf(stderr, "Error: Invalid argument for --time\n");
                        exit(1);
                    }
                }
                break;
            case 'a':
                change_modification_time = 0;
                break;
            case 'm':
                change_access_time = 0;
                break;
            case 'c':
                no_create = 1;
                break;
            case 'f':
                // Ignored for backward compatibility
                break;
            case 'd':
                timestamp = optarg;
                break;
            case 't':
                timestamp = optarg;
                break;
            case 'r':
                reference_file = optarg;
                break;
            case 'h':
                no_dereference = 1;
                break;
            case 's':
                silent = 1;
                break;
            case '?':
                exit(1);
            default:
                print_usage();
        }
    }

    if (optind >= argc && !silent) {
        fprintf(stderr, "Error: Expected filename after options\n");
        print_usage();
    }

    if (reference_file) {
        struct stat ref_stat;
        if (stat(reference_file, &ref_stat) != 0 && !silent) {
            perror("Error reading reference file");
            exit(1);
        }
        new_times.actime = ref_stat.st_atime;
        new_times.modtime = ref_stat.st_mtime;
    } else if (timestamp) {
        struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
        if (!strptime(timestamp, "%c", &tm) && !strptime(timestamp, "%x %X", &tm) && !silent) {
            fprintf(stderr, "Error: Could not parse timestamp\n");
            exit(1);
        }
        new_times.actime = change_access_time ? mktime(&tm) : time(NULL);
        new_times.modtime = change_modification_time ? mktime(&tm) : time(NULL);
    } else {
        new_times.actime = time(NULL);
        new_times.modtime = time(NULL);
    }

    for (int i = optind; i < argc; i++) {
        touch_file(argv[i], no_create, new_times, silent, no_dereference);
    }

    return 0;
}
