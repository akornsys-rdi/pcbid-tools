#ifndef MISC_H
    #define MISC_H

    //APP
    #define _VERSION "1.2.0"
    #define _APPNAME "pcbid-generator"
    //EXIT STATUS
    #define EXIT_STATUS_SUCCESS 0
    #define EXIT_STATUS_NO_ARGS 1
    #define EXIT_STATUS_INVALID_ARG 2
    #define EXIT_STATUS_NO_ENOUGH_ARGS 3
    #define EXIT_STATUS_INVALID_DATA 4
	#define EXIT_STATUS_NO_FILE 5
	#define EXIT_STATUS_INVALID_FILE 6
    //FILE CONSTANTS
    #define COMMENT_SYMBOL '\''
    //MAX VALUES
    #define STRING_MAX_LEN 80
    #define PCBID_MAX_NUM 7225 //85^2
    #define PCBID_MAX_ID 52200625 //85^4
    #define PCBID_MAX_FIELDS 2724905250390625 //85^8

    enum yn_answer {
        OTHER_ANSWER = -1,
        NO_ANSWER = 0,
        YES_ANSWER = 1
    };
    enum default_yn {
        DEFAULT_NO = 0,
        DEFAULT_YES = 1
    };
    enum valid_pcbid_id {
        ID_NOT_VALID = 0,
        ID_VALID = 1
    };
    enum valid_country {
        COUNTRY_NOT_VALID = 0,
        COUNTRY_VALID = 1
    };
    enum valid_pcbid_number {
        NUMBER_NOT_VALID = 0,
        NUMBER_VALID = 1
    };
    enum valid_time {
        TIME_NOT_VALID = 0,
        TIME_VALID = 1
    };
    enum awaiting_argument {
        ARG_NONE = 0,
        ARG_AUTHORNAME = 1,
        ARG_COUNTRY = 2,
        ARG_FILE = 3,
        ARG_PROJECTNAME = 4,
        ARG_MODULE = 5,
        ARG_RELEASE = 6,
        ARG_DATE = 7
    };
    enum type_line {
        ERROR_LINE = -1,// Cannot open file
        NONE_LINE = 0,  // EOF
        COMMENT_LINE = 1,
        EMPTY_LINE = 2,
        INVALID_LINE = 3,
        UNEXPECTED_EOL = 4,
        TRUNCATED_LINE = 5,
        VALID_LINE = 6
    };
    enum fsm_read_file {
        END_FILE = 0,
        FIRST_CHAR = 1,
        READ_LINE = 2,
        AWAIT_WS = 3,   // Whitespace
        AWAIT_CM = 4,   // Comment symbol
        AWAIT_CMP = 5,  // Comment: project name
        AWAIT_CMS = 6,  // Comment: separator
        AWAIT_CMA = 7,  // Comment: author name
        AWAIT_CMC = 8,  // Comment: country
        AWAIT_LF = 9    // Line feed
    };
    enum collision_result {
        FORMAT_NOT_VALID = -1,
        NO_COLLISION_FOUND = 0,
        COLLISION_FOUND = 1
    };
    enum search_field {
        FIELD_AUTHOR = 0,
        FIELD_PROJECT = 1
    };
	enum valid_file {
		FILE_NOT_EXIST = -1,
		FILE_NOT_VALID = 0,
		FILE_VALID = 1
	};

    void print_version();
    void print_usage();
    void print_help();
    void convert_uppercase(char *str);
    void ltrim(char *str);
    void rtrim(char *str);
    void remove_heading_tilde(char *str);
    void sanetize_string(char *str);
    enum yn_answer check_yn_answer(char *str, enum default_yn dfl);
    enum valid_country check_country(char *country, char *dfl, char *country_name);
    enum valid_pcbid_number check_pcbid_number(char *number, char *dfl);
    void num_to_charset(__uint64_t number, char *str);
    void str_to_charset(char *number, char *str);
    void charset_to_num(char *str, __uint64_t *number);
    enum valid_time check_time(char *date, char *dfl);
    void get_actual_time(char *str);
    void get_week_from_date(char *week, char *date);
    void get_year_from_date(char *wyear, char *date);
    void calculate_sha256(char *str, unsigned char *md);
    void calculate_id(char *name, unsigned long counter, char *id);
    void concat_pcbid(char *pcbid, char *authorid, char *projectid, char *module, char *release, char *week, char *year);
    void concat_line(char *line, char *authorid, char *projectid, char *projectname, char *authorname, char *countrycode);
    fpos_t set_start_pos(char *filename);
    enum type_line read_line_file(char *filename, fpos_t *pos, char *str, unsigned long size);
    enum valid_file check_file_valid(char *filename, __uint64_t *lines, __uint64_t *entries_ok, __uint64_t *entries_ko);
    enum collision_result process_collision(char *id, __uint64_t num);
    enum collision_result search_file_collisions(char *filename, char *str, enum search_field field);
    void print_line(enum type_line line);
    void write_line_file(char *filename, char *str_line);
    void generate_file_header(char *filename);
    void generate_random_name(char *filename, char *str);
    void sort_file(char *filename, __uint64_t entries_ok);
#endif
