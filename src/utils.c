#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "openssl/sha.h"
#include "utils.h"

// K85 charset
const char k85_charset[] = "0123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijklmnpqrstuvwxyz!#$%&()*+-;<=>?@^_{}[]/\\:\"";

// Print version information
void print_version() {
    printf(_APPNAME " " _VERSION "\n");
    printf("Copyright © 2020 RileyStarlight.\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

// Print usage information
void print_usage() {
    printf("Usage: " _APPNAME " {-h | -v}\n");
    printf("  or   " _APPNAME " -i [OPTIONS]\n");
    printf("  or   " _APPNAME " --author-name <EXP> --country <EXP> --project-name <EXP> --module <EXP> --release <EXP> --date <EXP> [OPTIONS]\n");
    printf("Generate unique numbers for PCBs.\n");
    printf("Try `" _APPNAME " --help` for more information.\n\n");
}

// Print usage and help information
void print_help() {
    print_usage();
    printf("Options\n");
    printf("      --author-name <EXP>\tSet author name to <EXP>\n");
    printf("      --collision-file <FILE>\tSet collision file to <FILE>\n");
    printf("      --country <EXP>\t\tSet country to <EXP> (ISO3166-1-A2)\n");
    printf("      --date <EXP>\t\tSet date to <EXP> (YYYY-MM-DD)\n");
    printf("  -d, --dry-run\t\t\tPrevent overwriting files\n");
    printf("  -h, --help\t\t\tDisplay this help and exit\n");
    printf("  -i, --interactive\t\tInteractive mode\n");
    printf("      --module <EXP>\t\tSet module to <EXP> (decimal)\n");
    printf("      --project-name <EXP>\tSet project name to <EXP>\n");
    printf("  -q, --quiet\t\t\tSuppress verbose messages\n");
    printf("      --release <EXP>\t\tSet release to <EXP> (decimal)\n");
    printf("  -r, --remove-tilde\t\tRemove heading tilde\n");
    printf("  -v, --version\t\t\tOutput version information and exit\n");
}

// Convert a string to uppercase
// Arguments:
//  str: String to be processed, returns the processed string using this variable
void convert_uppercase(char *str) {
    for (unsigned char i = 0; i < strlen(str); i++) {
        if (str[i] >= 'a' && str[i] <= 'z') str[i] = toupper(str[i]);
    }
}

// Remove the whitespace to the left of the string, if any
// Arguments:
//  str: String to be processed, returns the processed string using this variable
void ltrim(char *str) {
    unsigned char left_chars = 0;
    unsigned char len = 0;
    char sep[] = " \t\v\f\r\n";
    char buffer[STRING_MAX_LEN];

    left_chars = strspn(str, sep);
    if (left_chars > 0) {
        len = strlen(str);
        if (left_chars == len) buffer[0] = '\0';
        else memmove(buffer, str + left_chars, len + 1 - left_chars);
        strcpy(str, buffer);
    }
}

// Remove the whitespace to the right of the string, if any
// Arguments:
//  str: String to be processed, returns the processed string using this variable
void rtrim(char *str) {
    unsigned char right_chars = 0;
    char sep[] = " \t\v\f\r\n";
    
    right_chars = strlen(str) - 1;
    while (right_chars >= 0 && strchr(sep, str[right_chars]) != NULL) {
        str[right_chars] = '\0';
        right_chars--;
    }
}

// Remove the tilde at the beginning of the string, if any
// Arguments:
//  str: String to be processed, returns the processed string using this variable
void remove_heading_tilde(char *str) {
    unsigned char pos = 0;
    unsigned char len = 0;

    pos = strspn(str, "~");
    if (pos > 0) {
        len = strlen(str);
        if (pos == len) str[0] = '\0';
        else memmove(str, str + pos, len + 1 - pos);
    }
}

// Clean the string of whitespaces, if any
// Arguments:
//  str: String to be processed, returns the processed string using this variable
void sanetize_string(char *str) {
    ltrim(str);
    if (strcmp(str, "")) rtrim(str);
}

// Parse a string to extract _YES_ or _NO_
// Arguments:
//  str: String to be processed, can change this variable
//  dfl: Default value in case `str` is empty
// Return:
//  `yn_answer`-type variable. See enum declaration
enum yn_answer check_yn_answer(char *str, enum default_yn dfl) {
    convert_uppercase(str);
    sanetize_string(str);
    if (dfl == DEFAULT_YES) {
        if (!strcmp(str, "Y") || !strcmp(str, "YES") || !strcmp(str, "")) return YES_ANSWER;
        else if (!strcmp(str, "N") || !strcmp(str, "NO")) return NO_ANSWER;
        else return OTHER_ANSWER;
    }
    else if (dfl == DEFAULT_NO) {
        if (!strcmp(str, "Y") || !strcmp(str, "YES")) return YES_ANSWER;
        else if (!strcmp(str, "N") || !strcmp(str, "NO") || !strcmp(str, "")) return NO_ANSWER;
        else return OTHER_ANSWER;
    }
    return 0;
}

// Parse a string to extract a country code according to ISO 3166
// Arguments:
//  country: String to be processed, can change this variable
//  dfl: Default value in case `country` is empty
//  country_name: Return the full name of the country
// Return:
//  `valid_country`-type variable. See enum declaration
enum valid_country check_country(char *country, char *dfl, char *country_name) {
    char iso3166_1_list[][3] = {
    "AD","AE","AF","AG","AI","AL","AM","AO","AQ","AR","AS","AT","AU","AW","AX",
    "AZ","BA","BB","BD","BE","BF","BG","BH","BI","BJ","BL","BM","BN","BO","BQ",
    "BR","BS","BT","BV","BW","BY","BZ","CA","CC","CD","CF","CG","CH","CI","CK",
    "CL","CM","CN","CO","CR","CU","CV","CW","CX","CY","CZ","DE","DJ","DK","DM",
    "DO","DZ","EC","EE","EG","EH","ER","ES","ET","FI","FJ","FK","FM","FO","FR",
    "GA","GB","GD","GE","GF","GG","GI","GL","GM","GN","GP","GQ","GR","GS","GT",
    "GU","GW","GY","HK","HM","HN","HR","HT","HU","ID","IE","IL","IM","IN","IO",
    "IQ","IR","IS","IT","JE","JM","JO","JP","KE","KG","KH","KI","KM","KN","KP",
    "KR","KW","KY","KZ","LA","LB","LC","LI","LK","LR","LS","LT","LU","LV","LY",
    "MA","MC","MD","ME","MF","MG","MH","MK","ML","MM","MN","MO","MP","MQ","MR",
    "MS","MT","MU","MV","MW","MX","MY","MZ","OM","PA","PE","PF","PG","PH","PK",
    "PL","PM","PN","PR","PS","PT","PW","PY","QA","RE","RO","RS","RU","RW","SA",
    "SB","SC","SD","SE","SG","SH","SI","SJ","SK","SL","SM","SN","SO","SR","SS",
    "ST","SV","SX","SY","SZ","TC","TD","TF","TG","TH","TJ","TK","TL","TM","TN",
    "TO","TR","TT","TV","TW","TZ","UA","UG","UM","US","UY","UZ","VA","VC","VE",
    "VG","VI","VN","VU","WF","WS","YE","YT","ZA","ZM","ZW" };
    char country_name_list[][STRING_MAX_LEN] = {
    "Andorra", "United Arab Emirates", "Afghanistan", "Antigua and Barbuda",
    "Anguilla", "Albania", "Armenia", "Angola", "Antarctica", "Argentina",
    "American Samoa", "Austria", "Australia", "Aruba", "Åland Islands",
    "Azerbaijan", "Bosnia and Herzegovina", "Barbados", "Bangladesh",
    "Belgium", "Burkina Faso", "Bulgaria", "Bahrain", "Burundi", "Benin",
    "Saint Barthélemy", "Bermuda", "Brunei Darussalam",
    "Plurinational State of Bolivia", "Bonaire, Sint Eustatius and Saba",
    "Brazil", "Bahamas", "Bhutan", "Bouvet Island", "Botswana", "Belarus",
    "Belize", "Canada", "Cocos (Keeling) Islands",
    "Democratic Republic of the Congo", "Central African Republic", "Congo",
    "Switzerland", "Côte d'Ivoire", "Cook Islands", "Chile", "Cameroon",
    "China", "Colombia", "Costa Rica", "Cuba", "Cabo Verde", "Curaçao",
    "Christmas Island", "Cyprus", "Czechia", "Germany", "Djibouti", "Denmark",
    "Dominica", "Dominican Republic", "Algeria", "Ecuador", "Estonia", "Egypt",
    "Western Sahara", "Eritrea", "Spain", "Ethiopia", "Finland", "Fiji",
    "Falkland Islands (Malvinas)", "Federated States of Micronesia",
    "Faroe Islands", "France", "Gabon",
    "United Kingdom of Great Britain and Northern Ireland", "Grenada",
    "Georgia", "French Guiana", "Guernsey", "Gibraltar", "Greenland", "Gambia",
    "Guinea", "Guadeloupe", "Equatorial Guinea", "Greece",
    "South Georgia and the South Sandwich Islands", "Guatemala", "Guam",
    "Guinea-Bissau", "Guyana", "Hong Kong",
    "Heard Island and McDonald Islands", "Honduras", "Croatia", "Haiti",
    "Hungary", "Indonesia", "Ireland", "Israel", "Isle of Man", "India",
    "British Indian Ocean Territory", "Iraq", "Islamic Republic of Iran",
    "Iceland", "Italy", "Jersey", "Jamaica", "Jordan", "Japan", "Kenya",
    "Kyrgyzstan", "Cambodia", "Kiribati", "Comoros", "Saint Kitts and Nevis",
    "Democratic People's Republic of Korea", "Republic of Korea", "Kuwait",
    "Cayman Islands", "Kazakhstan", "Lao People's Democratic Republic",
    "Lebanon", "Saint Lucia", "Liechtenstein", "Sri Lanka", "Liberia",
    "Lesotho", "Lithuania", "Luxembourg", "Latvia", "Libya", "Morocco",
    "Monaco", "Republic of Moldova", "Montenegro",
    "Collectivity of Saint Martin", "Madagascar", "Marshall Islands",
    "North Macedonia", "Mali", "Myanmar", "Mongolia", "Macao",
    "Northern Mariana Islands", "Martinique", "Mauritania", "Montserrat",
    "Malta", "Mauritius", "Maldives", "Malawi", "Mexico", "Malaysia",
    "Mozambique", "Oman", "Panama", "Peru", "French Polynesia",
    "Papua New Guinea", "Philippines", "Pakistan", "Poland",
    "Saint Pierre and Miquelon", "Pitcairn", "Puerto Rico",
    "State of Palestine", "Portugal", "Palau", "Paraguay", "Qatar", "Réunion",
    "Romania", "Serbia", "Russian Federation", "Rwanda", "Saudi Arabia",
    "Solomon Islands", "Seychelles", "Sudan", "Sweden", "Singapore",
    "Saint Helena, Ascension and Tristan da Cunha", "Slovenia",
    "Svalbard and Jan Mayen", "Slovakia", "Sierra Leone", "San Marino",
    "Senegal", "Somalia", "Suriname", "South Sudan", "Sao Tome and Principe",
    "El Salvador", "Sint Maarten", "Syrian Arab Republic", "Eswatini",
    "Turks and Caicos Islands", "Chad", "French Southern Territories", "Togo",
    "Thailand", "Tajikistan", "Tokelau", "Timor-Leste", "Turkmenistan",
    "Tunisia", "Tonga", "Turkey", "Trinidad and Tobago", "Tuvalu",
    "Taiwan, Province of China", "United Republic of Tanzania", "Ukraine",
    "Uganda", "United States Minor Outlying Islands",
    "United States of America", "Uruguay", "Uzbekistan", "Holy See",
    "Saint Vincent and the Grenadines", "Bolivarian Republic of Venezuela",
    "British Virgin Islands", "United States Virgin Islands", "Viet Nam",
    "Vanuatu", "Wallis and Futuna", "Samoa", "Yemen", "Mayotte",
    "South Africa", "Zambia", "Zimbabwe" };
    unsigned int iso3166_1_size= 0;

    convert_uppercase(country);
    sanetize_string(country);
    if (!strcmp(country, "")) {
        convert_uppercase(dfl);
        sanetize_string(dfl);
        strcpy(country, dfl);
    }
    iso3166_1_size = sizeof(iso3166_1_list) / 3;
    for (unsigned int i = 0; i < iso3166_1_size; i++) {
        if (!strcmp(country, iso3166_1_list[i])) {
            strcpy(country_name, country_name_list[i]);
            return COUNTRY_VALID;
        }
    }
    return COUNTRY_NOT_VALID;
}

// Parse a string to extract the number provided
// Arguments:
//  number: String to be processed, can change this variable
//  dlf: Default value in case `number` is empty
// Return:
//  `valid_pcbid_number`-type variable. See enum declaration
enum valid_pcbid_number check_pcbid_number(char *number, char *dfl) {
    unsigned long ul = 0;

    sanetize_string(number);
    if (!strcmp(number, "")) {
        strcpy(number, dfl);
    }
    for (unsigned char i = 0; i < strlen(number); i++) {
        if (number[i] < '0' || number[i] > '9') {
            return NUMBER_NOT_VALID;
        }
    }
    ul = strtoul(number, NULL, 10);
    if (ul < 1 || ul > PCBID_MAX_NUM - 1) return NUMBER_NOT_VALID;
    return NUMBER_VALID;
}

// Convert a decimal number to an base85 string using the K85 charset, for author or project field
// Arguments:
//  number: Decimal number to be processed
//  str: Returns the resulting string
void num_to_charset(__uint64_t number, char *str) {
    unsigned char k85_pos[4];

    for (unsigned char i = 0; i < 4; i++) {
        k85_pos[3 - i] = number % 85;
        number /= 85;
    }
    sprintf(str, "%c%c%c%c", k85_charset[k85_pos[0]], k85_charset[k85_pos[1]], k85_charset[k85_pos[2]], k85_charset[k85_pos[3]]);
}

// Convert a decimal number to an base85 string using the K85 charset, for module or release field
// Arguments:
//  number: Decimal number to be processed
//  str: Returns the resulting string
void str_to_charset(char *number, char *str) {
    unsigned char k85_pos[2];
    __uint64_t sum = 0;

    sum = strtol(number, NULL, 10);
    for (unsigned char i = 0; i < 2; i++) {
        k85_pos[1 - i] = sum % 85;
        sum /= 85;
    }
    sprintf(str, "%c%c", k85_charset[k85_pos[0]], k85_charset[k85_pos[1]]);
}

// Convert a base85 string using the K85 charset to a decimal number
// Arguments:
//  str: String to be processed
//  number: Returns the resulting number, in decimal
void charset_to_num(char *str, __uint64_t *number) {
    unsigned char len = 0;

    *number = 0;
    len = strlen(str);
    for (unsigned char i = 0; i < 85; i++) {
        for (unsigned char j = 0; j < len; j++) {
            if (str[j] == k85_charset[i]) *number += (i * pow(85, (len-1)-j));
        }
    }
}


// Parse a string to extract a date in ISO 8601 format
// Arguments:
//  date: String to be processed, can change this variable
//  dfl: Default value in case `date` is empty
// Return:
//  `valid_time`-type variable. See enum declaration
enum valid_time check_time(char *date, char *dfl) {
    char *str_year;
    char *str_month;
    char *str_day;
    char buffer[STRING_MAX_LEN] = "";
    int year = 0;
    int month = 0;
    int day = 0;

    sanetize_string(date);
    if (!strcmp(date, "")) {
        memcpy(date, dfl, STRING_MAX_LEN);
    }
    if (strlen(date) == 10) {
        if (date[4] == '-' && date[7] == '-') {
            memcpy(buffer, date, STRING_MAX_LEN);
            str_year = strtok(buffer, "-");
            str_month = strtok(NULL, "-");
            str_day = strtok(NULL, "-");
            year = atoi(str_year);
            month = atoi(str_month);
            day = atoi(str_day);
            if (year < 1900 || year > 9999) return TIME_NOT_VALID;
            if (month < 1 || month > 12) return TIME_NOT_VALID;
            if (day < 1 || day > 31) return TIME_NOT_VALID;
            if (day > 30 && (month == 2 || month == 4 || month == 6 || month == 9 || month == 11)) return TIME_NOT_VALID;
            if (day > 29 && month == 2) return TIME_NOT_VALID;
            if (day > 28 && month == 2 && !(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) return TIME_NOT_VALID;
            return TIME_VALID;
        }
    }
    else {
        return TIME_NOT_VALID;
    }

    return 0;
}

// Get the current date in ISO 8601 format
// Arguments:
//  str: Returns the resulting string
void get_actual_time(char *str) {
    time_t epoch;
    struct tm *timestruct;

    time(&epoch);
    timestruct = gmtime(&epoch);
    strftime(str, STRING_MAX_LEN, "%F", timestruct);
}

// Process a string with the date to obtain the week number according to ISO 8601
// Arguments:
//  week: Returns the resulting string
//  date: String to be processed
void get_week_from_date(char *week, char *date) {
    time_t epoch;
    struct tm *timestruct;
    char *str_year;
    char *str_month;
    char *str_day;
    char buffer[STRING_MAX_LEN] = "";
    int year;
    int month;
    int day;

    memcpy(buffer, date, STRING_MAX_LEN);
    str_year = strtok(buffer, "-");
    str_month = strtok(NULL, "-");
    str_day = strtok(NULL, "-");
    year = atoi(str_year);
    month = atoi(str_month);
    day = atoi(str_day);
    time(&epoch);
    timestruct = gmtime(&epoch);
    timestruct->tm_sec = 0;
    timestruct->tm_min = 0;
    timestruct->tm_hour = 0;
    timestruct->tm_mday = day;
    timestruct->tm_mon = month - 1;
    timestruct->tm_year = year - 1900;
    timestruct->tm_isdst = -1;
    mktime(timestruct);
    strftime(week, STRING_MAX_LEN, "%V", timestruct);
}

// Process a string with the date to obtain the year number according to ISO 8601
// Arguments:
//  wyear: Returns the resulting string
//  date: String to be processed
void get_year_from_date(char *wyear, char *date) {
    time_t epoch;
    struct tm *timestruct;
    char *str_year;
    char *str_month;
    char str_week[STRING_MAX_LEN] = "";
    char *str_day;
    char buffer[STRING_MAX_LEN] = "";
    int year;
    int month;
    int week;
    int day;

    memcpy(buffer, date, STRING_MAX_LEN);
    str_year = strtok(buffer, "-");
    str_month = strtok(NULL, "-");
    str_day = strtok(NULL, "-");
    year = atoi(str_year);
    month = atoi(str_month);
    day = atoi(str_day);
    time(&epoch);
    timestruct = gmtime(&epoch);
    timestruct->tm_sec = 0;
    timestruct->tm_min = 0;
    timestruct->tm_hour = 0;
    timestruct->tm_mday = day;
    timestruct->tm_mon = month - 1;
    timestruct->tm_year = year - 1900;
    timestruct->tm_isdst = -1;
    mktime(timestruct);
    strftime(str_week, STRING_MAX_LEN, "%V", timestruct);
    week = atoi(str_week);
    if (month == 1 && week > 51) {
        timestruct->tm_year--;
        mktime(timestruct);
    }
    strftime(wyear, STRING_MAX_LEN, "%y", timestruct);
}

// Generate the sha256 hash of a string
// Arguments:
//  str: String to be processed
//  md: Return the resulting hash
void calculate_sha256(char *str, unsigned char *md) {
    SHA256_CTX SHA_context;

    SHA256_Init(&SHA_context);
    SHA256_Update(&SHA_context, str, strlen(str));
    SHA256_Final(md, &SHA_context);
}

// Generate the PCBID identifier of a string
// Arguments:
//  name: String to be processed
//  counter: Number of previous collisions found for the string
//  id: Returns the resulting string
void calculate_id(char *name, unsigned long counter, char *id) {
    char buffer[STRING_MAX_LEN + 11] = "";
    char append[STRING_MAX_LEN] = "";
    char digest_hex[(SHA256_DIGEST_LENGTH * 2) + 1] = "";
    char chunk[8][((SHA256_DIGEST_LENGTH * 2) / 8) + 1 ];
    unsigned char md[SHA256_DIGEST_LENGTH] = "";
    __uint64_t sum = 0;

    strcpy(buffer, name);
    if (counter) {
        sprintf(append, " #%lu", counter);
        strcat(buffer, append);
    }
    calculate_sha256(buffer, md);
    for (unsigned char i = 0; i < SHA256_DIGEST_LENGTH; i++) sprintf(digest_hex + (i * 2), "%02x", md[i]);
    for (unsigned char i = 0; i < (SHA256_DIGEST_LENGTH * 2) / 8; i++) {
        memmove(chunk[i], digest_hex + (i * 8), 8);
        chunk[i][8] = '\0';
        sum += strtol(chunk[i], NULL, 16);
    }
    sum = sum % PCBID_MAX_ID;
    num_to_charset(sum, id);
}

// Concatenate strings to form the PCBID number
// Arguments:
//  pcbid: Returns the resulting string
//  authorid: String containing the author identifier
//  projectid: String containing the project identifier
//  module: String containing the module number
//  release: String containing the release number
//  week: String containing the week number
//  year: String containing the year number
void concat_pcbid(char *pcbid, char *authorid, char *projectid, char *module, char *release, char *week, char *year) {
    strcpy(pcbid, authorid);
    strcat(pcbid, projectid);
    strcat(pcbid, module);
    strcat(pcbid, release);
    strcat(pcbid, week);
    strcat(pcbid, year);
}

// Concatenate strings to form the entry line of the `projects.txt` file
// Arguments:
//  line: Returns the resulting string
//  authorid: String containing the author identifier
//  projectid: String containing the project identifier
//  projectname: String containing the project name
//  authorname: String containing the author name
//  countrycode: String containing the country code
void concat_line(char *line, char *authorid, char *projectid, char *projectname, char *authorname, char *countrycode) {
    sprintf(line, "%s%s    %c %s @ %s (%s)\n", authorid, projectid, COMMENT_SYMBOL, projectname, authorname, countrycode);
}

// Get the start position of a file
// Arguments:
//  filename: String with file path
// Return:
//  `fpos_t`-type variable with the file position
fpos_t set_start_pos(char *filename) {
    FILE *fptr;
    fpos_t pos;

    fptr = fopen(filename, "r");
    if (fptr != NULL) {
        rewind(fptr);
        fgetpos(fptr, &pos);
        fclose(fptr);
    }
    return pos;
}

// Process a file line
// Arguments:
//  filename: String with file path
//  pos: Start position of the line to be processed. Returns the position of the next line
//  str: Returns the content of the line read, truncated if it exceeds the `size` lenght
//  size: Maximum size of the `str` variable
// Return:
//  `type_line`-type variable. See enum declaration
enum type_line read_line_file(char *filename, fpos_t *pos, char *str, unsigned long size) {
    FILE *fptr;
    unsigned char index = 0;
    char c;
    unsigned long i = 0;
    enum fsm_read_file fsm_status = FIRST_CHAR;
    enum type_line ret = NONE_LINE;

    fptr = fopen(filename, "r");
    if (fptr == NULL) ret = ERROR_LINE;
    else {
        fsetpos(fptr, pos);
        do {
            c = fgetc(fptr);
            if (i < size) {
                str[i++] = c;
                switch (fsm_status) {
                  case END_FILE:
                    fsm_status = END_FILE;
                    break;
                  case FIRST_CHAR:
                    if (c == EOF) {
                        fsm_status = END_FILE;
                        ret = NONE_LINE;
                    }
                    else if (c == COMMENT_SYMBOL) {
                        fsm_status = AWAIT_LF;
                        ret = COMMENT_LINE;
                    }
                    else if (c > ' ' && c <= '~') {
                        if (c != 'I' && c != '|' && c != 'o' && c != 'O' && c != '`' && c != '\'' && c != ' ') {
                            fsm_status = READ_LINE;
                            index = 0;
                        }
                        else {
                            fsm_status = AWAIT_LF;
                            ret = INVALID_LINE;
                        }
                    }
                    else {
                        if (c != '\n') {
                            fsm_status = AWAIT_LF;
                            ret = INVALID_LINE;
                        }
                        else ret = EMPTY_LINE;
                    }
                    break;
                  case READ_LINE:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (index < 7) {
                        if ( c > ' ' && c < '~') {
                            if (c != 'I' && c != '|' && c != 'o' && c != 'O' && c != '`' && c != '\'' && c != ' ') {
                                index++;
                                if (index >= 7) fsm_status = AWAIT_WS;
                            }
                            else {
                                fsm_status = AWAIT_LF;
                                ret = INVALID_LINE;
                            }
                        }
                        else {
                            fsm_status = AWAIT_LF;
                            ret = INVALID_LINE;
                        }
                    }
                    break;
                  case AWAIT_WS:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c == ' ' || c == '\t') fsm_status = AWAIT_CM;
                    else {
                        fsm_status = AWAIT_LF;
                        ret = INVALID_LINE;
                    }
                    break;
                  case AWAIT_CM:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c == COMMENT_SYMBOL) {
                        fsm_status = AWAIT_CMP;
                    }
                    else if ( c != ' ' && c != '\t') {
                        fsm_status = AWAIT_LF;
                        ret = INVALID_LINE;
                    }
                    break;
                  case AWAIT_CMP:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c != '@' && c != ' ' && c != '\t') fsm_status = AWAIT_CMS;
                    else if (c != ' ' && c != '\t') {
                        fsm_status = AWAIT_LF;
                        ret = INVALID_LINE;
                    }
                    break;
                  case AWAIT_CMS:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c == '@') fsm_status = AWAIT_CMA;
                    break;
                  case AWAIT_CMA:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c != '(' && c != ' ' && c != '\t') fsm_status = AWAIT_CMC;
                    else if (c != ' ' && c != '\t') {
                        fsm_status = AWAIT_LF;
                        ret = INVALID_LINE;
                    }
                    break;
                  case AWAIT_CMC:
                    if (c == '\n') {
                        fsm_status = FIRST_CHAR;
                        ret = UNEXPECTED_EOL;
                    }
                    else if (c == '(') index = 1;
                    else if (index < 4) {
                        if ((index < 3) && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) index++;
                        else if ((index == 3) && (c == ')')) {
                            fsm_status = AWAIT_LF;
                            ret = VALID_LINE;
                        }
                        else {
                            fsm_status = AWAIT_LF;
                            ret = INVALID_LINE;
                        }
                    }
                    break;
                  case AWAIT_LF:
                    if (c == '\n' || c == EOF) {
                        fsm_status = FIRST_CHAR;
                    }
                    break;
                }
            }
            else {
                fsm_status = FIRST_CHAR;
                ret = TRUNCATED_LINE;
            }
        } while (c != '\n' && c != EOF);
        str[i] = '\0';
        fgetpos(fptr, pos);
        fclose(fptr);
    }
    return ret;
}

// Parse the contents of the archive for valid entries
// Arguments:
//  filename: String with file path
//  lines: Returns the number of lines in the file
//  entries_ok: Returns the number of valid lines (according to expected file format)
//  entries_ko: Returns the number of invalid lines
// Return:
//  `valid_file`-type variable. See enum declaration
enum valid_file check_file_valid(char *filename, __uint64_t *lines, __uint64_t *entries_ok, __uint64_t *entries_ko) {
    fpos_t file_position = set_start_pos(filename);
    char dummy[STRING_MAX_LEN*3] = "";
    enum type_line line = NONE_LINE;
    enum valid_file ret = FILE_NOT_VALID;

    *lines = 0;
    *entries_ok = 0;
    *entries_ko = 0;
    do {
        line = read_line_file(filename, &file_position, dummy, STRING_MAX_LEN*3);
        if (line == ERROR_LINE) ret = FILE_NOT_EXIST;
        if (line != NONE_LINE) (*lines)++;
        if (line == VALID_LINE) (*entries_ok)++;
        else if (line == INVALID_LINE || line == UNEXPECTED_EOL) (*entries_ko)++;
    } while(line != NONE_LINE && line != ERROR_LINE);
    if (ret != FILE_NOT_EXIST) { 
        if (*lines == 0 || *entries_ok != 0) ret = FILE_VALID;
        else ret = FILE_NOT_VALID;
    }
    return ret;
}

// Compare two identifiers for collisions
// Arguments:
//  id: String with the identifier to be checked
//  num: Decimal number with the value of the identifier to be checked
// Return:
//  `collision_result`-type variable. See enum declaration
enum collision_result process_collision(char *id, __uint64_t num) {
    __uint64_t decimal_id = 0;

    charset_to_num(id, &decimal_id);
    if (decimal_id == num) return COLLISION_FOUND;
    return NO_COLLISION_FOUND;
}

// Scan a file for collisions
// Arguments:
//  filename: String with file path
//  str_id: String with the identifier to be checked
//  field: Type of field to be checked
// Return:
//  `collision_result`-type variable. See enum declaration
enum collision_result search_file_collisions(char *filename, char *str_id, enum search_field field) {
    fpos_t file_position = set_start_pos(filename);
    char id_buffer[9] = "";
    char str_line[STRING_MAX_LEN*3] = "";
    enum type_line line = NONE_LINE;
    enum collision_result ret = FORMAT_NOT_VALID;
    __uint64_t decimal_str = 0;

    charset_to_num(str_id, &decimal_str);
    do {
        line = read_line_file(filename, &file_position, str_line, STRING_MAX_LEN*3);
        if (line == VALID_LINE) {
            if (field == FIELD_AUTHOR) {
                memmove(id_buffer, str_line, 4);
                id_buffer[4] = '\0';
            }
            else if (field == FIELD_PROJECT) {
                memmove(id_buffer, str_line, 8);
                id_buffer[8] = '\0';
            }
            if (process_collision(id_buffer, decimal_str) == COLLISION_FOUND) ret = COLLISION_FOUND;
            else if (ret == FORMAT_NOT_VALID) ret = NO_COLLISION_FOUND;
        }
    } while (line != NONE_LINE);
    return ret;
}

// Prints line type, for debugging only
// Arguments:
//  line: Type of line read
void print_line(enum type_line line) {
    if (line == NONE_LINE) printf("None\n");
    else if (line == COMMENT_LINE) printf("Comment\n");
    else if (line == EMPTY_LINE) printf("Empty\n");
    else if (line == INVALID_LINE) printf("Invalid\n");
    else if (line == UNEXPECTED_EOL) printf("Unexpected end\n");
    else if (line == TRUNCATED_LINE) printf("Truncated\n");
    else printf("Valid - ");
}

// Write a line in a file
// Arguments:
//  filename: String with file path
//  str_line: String with the content to be written in the file
void write_line_file(char *filename, char *str_line) {
    FILE *fptr;

    fptr = fopen(filename, "a");
    if (fptr == NULL) fprintf(stderr, "ERROR: Temporary file could not be opened\n");
    else {
        fputs(str_line, fptr);
        fclose(fptr);
    }
}

// Save the `projects.txt` header to a file
// Arguments:
//  filename: String with file path
void generate_file_header(char *filename) {
    char str_date[STRING_MAX_LEN] = "";
    char str_gen[STRING_MAX_LEN] = "";

    get_actual_time(str_date);
    strcpy(str_gen, "' File automatically generated by " _APPNAME " v" _VERSION " on ");
    strcat(str_gen, str_date);
    strcat(str_gen, "\n");
    write_line_file(filename, "' File to manage the different project identifiers for PCBID\n");
    write_line_file(filename, "' Keep sorted in ascending order according to K85 Charset\n");
    write_line_file(filename, "' Syntax: `aaaapppp    ' Project name @ Author name (ISO3166-1-A2 country code)`\n");
    write_line_file(filename, str_gen);
    write_line_file(filename, "\n");
}

// Generate a temporary random file name from a file path
// Arguments:
//  filename: String with file path
//  str: Returns the resulting string
void generate_random_name(char *filename, char *str) {
    static bool seed_generated = false;
    unsigned char len = 0;
    char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char str_random_name[9] = "";
    char c = 0;

    if (!seed_generated) {
        srand(time(NULL));
        seed_generated = true;
    }
    len = sizeof(alphabet); //63
    for (unsigned char i = 0; i < 8; i++) {
        c = alphabet[rand() % (len - 1)];
        str_random_name[i] = c;
    }
    str_random_name[8] = '\0';
    strcpy(str, filename);
    strcat(str, "-");
    strcat(str, str_random_name);
    strcat(str, ".tmp");
}

// Sort the entries in a file using a temporary file
// Arguments:
//  filename: String with file path
//  entries_ok: Number of valid entries found in the archive
void sort_file(char *filename, __uint64_t entries_ok) {
    fpos_t file_position = set_start_pos(filename);
    char str_line[STRING_MAX_LEN*3] = "";
    char buffer_line[STRING_MAX_LEN*3] = "";
    char str_id[9] = "";
    char str_temp_file[STRING_MAX_LEN] = "";
    enum type_line line;
    __uint64_t line_id = 0;
    __uint64_t buffer_id = 0;
    __uint64_t max_id = PCBID_MAX_FIELDS;
    __uint64_t min_id = 0;
    __uint64_t dummy = 0;

    do {
        generate_random_name(filename, str_temp_file);
    } while(check_file_valid(str_temp_file, &dummy, &dummy, &dummy) != FILE_NOT_EXIST);
    generate_file_header(str_temp_file);
    for(__uint64_t i = 0; i < entries_ok; i++) {
        max_id = PCBID_MAX_FIELDS;
        file_position = set_start_pos(filename);
        do {
            line = read_line_file(filename, &file_position, str_line, STRING_MAX_LEN*3);
            if (line == VALID_LINE) {
                memmove(str_id, str_line, 8);
                str_id[8] = '\0';
                charset_to_num(str_id, &line_id);
                //printf("min: %ld, line: %ld, max: %ld\n", min_id, line_id, max_id);
                if (line_id < max_id && line_id > min_id) {
                    max_id = line_id;
                    strcpy(buffer_line, str_line);
                    buffer_id = line_id;
                }
            }
        } while (line != NONE_LINE && line != ERROR_LINE);
        write_line_file(str_temp_file, buffer_line);
        min_id = buffer_id;
    }
    if (rename(str_temp_file, filename)) {
        fprintf(stderr, "ERROR: The file could not be renamed");
    }
}
