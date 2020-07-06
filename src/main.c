#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    enum yn_answer answer = NO_ANSWER;
    enum yn_answer check_collision = NO_ANSWER;
    enum valid_country answer_country = COUNTRY_NOT_VALID;
    enum valid_pcbid_number answer_number = NUMBER_NOT_VALID;
    enum valid_time answer_time = TIME_NOT_VALID;
    enum awaiting_argument await_arg = ARG_NONE;
    enum valid_file file_status = FILE_NOT_VALID;
    bool authorname_flag = false;
    bool collisionfile_flag = false;
    bool country_flag = false;
    bool projectname_flag = false;
    bool module_flag = false;
    bool release_flag = false;
    bool date_flag = false;
    bool dry_run_flag = false;
    bool interactive_flag = false;
    bool quiet_flag = false;
    bool remove_tilde_flag = false;
    bool yn_has_author = false;
    bool yn_has_project = false;
    bool yn_has_collision = false;
    bool yn_correct = false;
    bool yn_interactive = false;
    char str_buffer[STRING_MAX_LEN] = "";
    char str_has_author[STRING_MAX_LEN] = "";
    char str_authorid[STRING_MAX_LEN] = "";
    char str_authorname[STRING_MAX_LEN] = "";
    char str_country[STRING_MAX_LEN] = "";
    char str_country_name[STRING_MAX_LEN] = "";
    char str_file[STRING_MAX_LEN] = "";
    char str_has_project[STRING_MAX_LEN] = "";
    char str_projectid[STRING_MAX_LEN] = "";
    char str_projectname[STRING_MAX_LEN] = "";
    char str_full_id[STRING_MAX_LEN] = "";
    char str_has_collision[STRING_MAX_LEN] = "";
    char str_module[STRING_MAX_LEN] = "1";
    char str_release[STRING_MAX_LEN] = "1";
    char str_time[STRING_MAX_LEN] = "";
    char str_default_time[STRING_MAX_LEN] = "";
    char str_week[STRING_MAX_LEN] = "";
    char str_year[STRING_MAX_LEN] = "";
    char str_correct[STRING_MAX_LEN] = "";
    char str_pcbid[STRING_MAX_LEN] = "";
    char str_line[STRING_MAX_LEN] = "";
    unsigned long author_collision_counter = 0;
    unsigned long project_collision_counter = 0;
    __uint64_t file_lines = 0;
    __uint64_t file_entries_ok = 0;
    __uint64_t file_entries_ko = 0;

    if (argc > 1) {
        await_arg = ARG_NONE;
        for(unsigned char i = 1; i < argc; i++) {
            // -h, --help
            if ((!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) && await_arg == ARG_NONE) {
                print_help();
                fflush(stdout);
                return EXIT_STATUS_SUCCESS;
            }
            // -v, --version
            else if ((!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) && await_arg == ARG_NONE) {
                print_version();
                fflush(stdout);
                return EXIT_STATUS_SUCCESS;
            }
            // -i, --dry-run
            else if ((!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dry-run")) && await_arg == ARG_NONE) {
                dry_run_flag = true;
            }
            // -i, --interactive
            else if ((!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interactive")) && await_arg == ARG_NONE) {
                interactive_flag = true;
            }
            // -q, --quiet
            else if ((!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) && await_arg == ARG_NONE) {
                quiet_flag = true;
            }
            // -r, --remove-tilde
            else if ((!strcmp(argv[i], "-r") || !strcmp(argv[i], "--remove-tilde")) && await_arg == ARG_NONE) {
                remove_tilde_flag = true;
            }
            // --author-name
            else if (!strcmp(argv[i], "--author-name") && await_arg == ARG_NONE) {
                authorname_flag = true;
                await_arg = ARG_AUTHORNAME;
            }
            // --collision-file
            else if (!strcmp(argv[i], "--collision-file") && await_arg == ARG_NONE) {
                collisionfile_flag = true;
                await_arg = ARG_FILE;
            }
            // --country
            else if (!strcmp(argv[i], "--country") && await_arg == ARG_NONE) {
                country_flag = true;
                await_arg = ARG_COUNTRY;
            }
            // --project-name
            else if (!strcmp(argv[i], "--project-name") && await_arg == ARG_NONE) {
                projectname_flag = true;
                await_arg = ARG_PROJECTNAME;
            }
            // --module
            else if (!strcmp(argv[i], "--module") && await_arg == ARG_NONE) {
                module_flag = true;
                await_arg = ARG_MODULE;
            }
            // --release
            else if (!strcmp(argv[i], "--release") && await_arg == ARG_NONE) {
                release_flag = true;
                await_arg = ARG_RELEASE;
            }
            // --date
            else if (!strcmp(argv[i], "--date") && await_arg == ARG_NONE) {
                date_flag = true;
                await_arg = ARG_DATE;
            }
            // unknow
            else {
                if (await_arg == ARG_AUTHORNAME) {
                    strcpy(str_authorname, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_COUNTRY) {
                    strcpy(str_country, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_FILE) {
                    strcpy(str_file, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_PROJECTNAME) {
                    strcpy(str_projectname, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_MODULE) {
                    strcpy(str_module, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_RELEASE) {
                    strcpy(str_release, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_DATE) {
                    strcpy(str_time, argv[i]);
                    await_arg = ARG_NONE;
                }
                else if (await_arg == ARG_NONE) {
                    fprintf(stderr, "Invalid option: '%s'\n\n", argv[i]);
                    print_usage();
                    fflush(stdout);
                    return EXIT_STATUS_INVALID_ARG;
                }
            }
        }
    }
    else {
        fprintf(stderr, "At least one argument is required\n\n");
        print_usage();
        fflush(stdout);
        return EXIT_STATUS_NO_ARGS;
    }
    if (interactive_flag) {
        print_version();
        printf("\n");
        if (collisionfile_flag) {
            file_status = check_file_valid(str_file, &file_lines, &file_entries_ok, &file_entries_ko);
            if (file_status == FILE_NOT_EXIST) {
                fprintf(stderr, "ERROR: File doesn't exist\n");
                fflush(stderr);
                return EXIT_STATUS_NO_FILE;
            }
            else if (file_status == FILE_NOT_VALID) {
                fprintf(stderr, "ERROR: Invalid file format\n");
                fflush(stderr);
                return EXIT_STATUS_INVALID_FILE;
            }
            else {
                if (!quiet_flag) {
                    printf("File read. %ld lines found with %ld entries.", file_lines, file_entries_ok);
                    if (file_entries_ko) printf(" Also %ld invalid lines have been found!\n", file_entries_ko);
                    else printf("\n");
                }
            }
        }
        do {
            printf("Please insert your author name [%s]: ", str_authorname);
            strcpy(str_buffer, str_authorname);
            fgets(str_authorname, STRING_MAX_LEN, stdin);
            sanetize_string(str_authorname);
            if (!strcmp(str_authorname, "")) {
                sanetize_string(str_buffer);
                strcpy(str_authorname, str_buffer);
                if (!strcmp(str_authorname, "")) fprintf(stderr, "WARNING: Empty author name field.\n");
            }
            do {
                printf("Do you already have a PCBID author identifier? [y/N]: ");
                fgets(str_has_author, STRING_MAX_LEN, stdin);
                answer = check_yn_answer(str_has_author, DEFAULT_NO);
                if (answer == OTHER_ANSWER) {
                    printf("\nPlease insert valid data!\n");
                    yn_has_author = false;
                }
                else {
                    check_collision = !answer;
                    yn_has_author = true;
                }
            } while (!yn_has_author);
            do {
                calculate_id(str_authorname, author_collision_counter, str_authorid);
                if (check_collision) {
                    if (collisionfile_flag) {
                        if (search_file_collisions(str_file, str_authorid, FIELD_AUTHOR) == COLLISION_FOUND) {
                            author_collision_counter++;
                            if (!quiet_flag) printf("Collision found! Regenerating field...\n");
                            yn_has_collision = true;
                        }
                        else {
                            yn_has_collision = false;
                            printf("Asigned author identifier: %s\n", str_authorid);
                        }
                    }
                    else {
                        printf("Asigned author identifier: %s\n", str_authorid);
                        printf("Please manually check for collision.\n"); //global
                        do {
                            printf("The assigned author identifier has collision? [y/N]: ");
                            fgets(str_has_collision, STRING_MAX_LEN, stdin);
                            answer = check_yn_answer(str_has_collision, DEFAULT_NO);
                            if (answer == YES_ANSWER) {
                                author_collision_counter++;
                                yn_has_collision = true;
                            }
                            else if (answer == NO_ANSWER) yn_has_collision = false;
                            else printf("\nPlease insert valid data!\n");
                        } while (answer == OTHER_ANSWER);
                    }
                }
                else printf("Asigned author identifier: %s\n", str_authorid);
            } while (yn_has_collision);
            do {
                printf("What country code the author name belongs to? (ISO3166-1-A2) [%s]: ", str_country);
                strcpy(str_buffer, str_country);
                fgets(str_country, STRING_MAX_LEN, stdin);
                answer_country = check_country(str_country, str_buffer, str_country_name);
                if (answer_country == COUNTRY_NOT_VALID) printf("\nPlease insert valid data!\n");
            } while (answer_country == COUNTRY_NOT_VALID);
            printf("Please insert your project name [%s]: ", str_projectname);
            strcpy(str_buffer, str_projectname);
            fgets(str_projectname, STRING_MAX_LEN, stdin);
            sanetize_string(str_projectname);
            if (!strcmp(str_projectname, "")) {
                sanetize_string(str_buffer);
                strcpy(str_projectname, str_buffer);
                if (!strcmp(str_projectname, "")) fprintf(stderr, "WARNING: Empty project name field.\n");
            }
            do {
                printf("Do you already have a PCBID project identifier? [y/N]: ");
                fgets(str_has_project, STRING_MAX_LEN, stdin);
                answer = check_yn_answer(str_has_project, DEFAULT_NO);
                if (answer == OTHER_ANSWER) {
                    printf("\nPlease insert valid data!\n");
                    yn_has_project = false;
                }
                else {
                    check_collision = !answer;
                    yn_has_project = true;
                }
            } while (!yn_has_project);
            do {
                calculate_id(str_projectname, project_collision_counter, str_projectid);
                if (check_collision) {
                    if (collisionfile_flag) {
                        strcpy(str_full_id, str_authorid);
                        strcat(str_full_id, str_projectid);
                        if (search_file_collisions(str_file, str_full_id, FIELD_PROJECT) == COLLISION_FOUND) {
                            project_collision_counter++;
                            if (!quiet_flag) printf("Collision found! Regenerating field...\n");
                            yn_has_collision = true;
                        }
                        else {
                            yn_has_collision = false;
                            printf("Asigned project identifier: %s\n", str_projectid);
                        }
                    }
                    else {
                        printf("Asigned project identifier: %s\n", str_projectid);
                        printf("Please manually check for collision.\n"); //same author
                        do {
                            printf("The assigned project identifier has collision? [y/N]: ");
                            fgets(str_has_collision, STRING_MAX_LEN, stdin);
                            answer = check_yn_answer(str_has_collision, DEFAULT_NO);
                            if (answer == YES_ANSWER) {
                                project_collision_counter++;
                                yn_has_collision = true;
                            }
                            else if (answer == NO_ANSWER) yn_has_collision = false;
                            else printf("\nPlease insert valid data!\n");
                        } while (answer == OTHER_ANSWER);
                    }
                }
                else printf("Asigned project identifier: %s\n", str_projectid);
            } while (yn_has_collision);
            do {
                printf("What is the PCB number? [%s]: ", str_module);
                strcpy(str_buffer, str_module);
                fgets(str_module, STRING_MAX_LEN, stdin);
                answer_number = check_pcbid_number(str_module, str_buffer);
                if (answer_number == NUMBER_VALID) str_to_charset(str_module, str_module);
                else printf("\nPlease insert valid data!\n");
            } while (answer_number == NUMBER_NOT_VALID);
            do {
                printf("What is the release number of the PCB? [%s]: ", str_release);
                strcpy(str_buffer, str_release);
                fgets(str_release, STRING_MAX_LEN, stdin);
                answer_number = check_pcbid_number(str_release, str_buffer);
                if (answer_number == NUMBER_VALID) str_to_charset(str_release, str_release);
                else printf("\nPlease insert valid data!\n");
            } while (answer_number == NUMBER_NOT_VALID);
            do {
                if (!strcmp(str_time, "")) get_actual_time(str_default_time);
                else strcpy(str_default_time, str_time);
                printf("What is the design date? (YYYY-MM-DD) [%s]: ", str_default_time);
                fgets(str_time, STRING_MAX_LEN, stdin);
                answer_time = check_time(str_time, str_default_time);
                if (answer_time == TIME_VALID) {
                    get_week_from_date(str_week, str_time);
                    get_year_from_date(str_year, str_time);
                }
                else if (answer_time == TIME_NOT_VALID) printf("\nPlease insert valid data!\n");
            } while (answer_time == TIME_NOT_VALID);
            printf("\n\tAuthor ID: %s (%s)\n", str_authorid, str_authorname);
            printf("\tCountry: %s (%s)\n", str_country, str_country_name);
            printf("\tProject ID: %s (%s)\n", str_projectid, str_projectname);
            printf("\tModule: %s\n", str_module);
            printf("\tRelease: %s\n", str_release);
            printf("\tWeek: %s\n", str_week);
            printf("\tYear: %s\n\n", str_year);
            do {
                printf("Is this information correct? [Y/n]: ");
                fgets(str_correct, STRING_MAX_LEN, stdin);
                answer = check_yn_answer(str_correct, DEFAULT_YES);
                if (answer == YES_ANSWER) {
                    yn_correct = true;
                    yn_interactive = true;
                }
                else if (answer == NO_ANSWER) {
                    printf("\nPlease re-enter the data\n\n");
                    yn_correct = true;
                    yn_interactive = false;
                }
                else {
                    printf("\nPlease insert valid data!\n");
                    yn_correct = false;
                }
            } while (!yn_correct);
        } while (!yn_interactive);
    }
    else if (authorname_flag && country_flag && projectname_flag && module_flag && release_flag && date_flag) {
        if (collisionfile_flag) {
            file_status = check_file_valid(str_file, &file_lines, &file_entries_ok, &file_entries_ko);
            if (file_status == FILE_NOT_EXIST) {
                fprintf(stderr, "ERROR: File doesn't exist\n");
                fflush(stderr);
                return EXIT_STATUS_NO_FILE;
            }
            else if (file_status == FILE_NOT_VALID) {
                fprintf(stderr, "ERROR: Invalid file format\n");
                fflush(stderr);
                return EXIT_STATUS_INVALID_FILE;
            }
            else {
                if (!quiet_flag) {
                    printf("File read. %ld lines found with %ld entries.", file_lines, file_entries_ok);
                    if (file_entries_ko) printf(" Also %ld invalid lines have been found!\n", file_entries_ko);
                    else printf("\n");
                }
            }
        }
        if (!strcmp(str_authorname, "")) fprintf(stderr, "WARNING: Empty author name field.\n");
        do {
            calculate_id(str_authorname, author_collision_counter, str_authorid);
            if (collisionfile_flag) {
                if (search_file_collisions(str_file, str_authorid, FIELD_AUTHOR) == COLLISION_FOUND) {
                    author_collision_counter++;
                    yn_has_collision = true;
                }
                else {
                    yn_has_collision = false;
                }
            }
        } while(yn_has_collision);
        strcpy(str_buffer, str_country);
        answer_country = check_country(str_country, str_buffer, str_country_name);
        if (answer_country == COUNTRY_NOT_VALID) {
            fprintf(stderr, "ERROR: Invalid country provided\n");
            fflush(stderr);
            return EXIT_STATUS_INVALID_DATA;
        }
        if (!strcmp(str_projectname, "")) fprintf(stderr, "WARNING: Empty project name field.\n");
        do {
            calculate_id(str_projectname, project_collision_counter, str_projectid);
            if (collisionfile_flag) {
                strcpy(str_full_id, str_authorid);
                strcat(str_full_id, str_projectid);
                if (search_file_collisions(str_file, str_full_id, FIELD_PROJECT) == COLLISION_FOUND) {
                    project_collision_counter++;
                    yn_has_collision = true;
                }
                else {
                    yn_has_collision = false;
                }
            }
        } while(yn_has_collision);
        answer_number = check_pcbid_number(str_module, "1");
        if (answer_number == NUMBER_VALID) str_to_charset(str_module, str_module);
        else {
            fprintf(stderr, "ERROR: Invalid module provided\n");
            fflush(stderr);
            return EXIT_STATUS_INVALID_DATA;
        }
        answer_number = check_pcbid_number(str_release, "1");
        if (answer_number == NUMBER_VALID) str_to_charset(str_release, str_release);
        else {
            fprintf(stderr, "ERROR: Invalid release provided\n");
            fflush(stderr);
            return EXIT_STATUS_INVALID_DATA;
        }
        get_actual_time(str_default_time);
        answer_time = check_time(str_time, str_default_time);
        if (answer_time == TIME_VALID) {
            get_week_from_date(str_week, str_time);
            get_year_from_date(str_year, str_time);
        }
        else {
            fprintf(stderr, "ERROR: Invalid date provided\n");
            fflush(stderr);
            return EXIT_STATUS_INVALID_DATA;
        }
    }
    else {
        fprintf(stderr, "Not enough arguments\n\n");
        print_usage();
        fflush(stdout);
        return EXIT_STATUS_NO_ENOUGH_ARGS;
    }
    concat_pcbid(str_pcbid, str_authorid, str_projectid, str_module, str_release, str_week, str_year);
    concat_line(str_line, str_authorid, str_projectid, str_projectname, str_authorname, str_country);
    if (collisionfile_flag && !dry_run_flag) {
        write_line_file(str_file, str_line);
        sort_file(str_file, file_entries_ok + 1);
    }
    if (!quiet_flag) printf("\nPCBID: ");
    if (!remove_tilde_flag) printf("~");
    printf("%s\n", str_pcbid);
    if (!collisionfile_flag) if (!quiet_flag) printf("Please insert this line in the `projects.txt` file:\n%s", str_line);
    if (!quiet_flag) printf("\n");

    return EXIT_STATUS_SUCCESS;
}
