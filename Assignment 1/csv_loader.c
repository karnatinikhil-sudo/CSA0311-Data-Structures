#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "csv_loader.h"

static char *trim_whitespace(char *str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int load_graph_csv(Graph *g, const char *path) {
    if (!g || !path) return -1;

    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "load_graph_csv: could not open '%s'\n", path);
        return -1;
    }

    char line[512];
    int edges_loaded = 0;
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* Strip trailing newlines / carriage returns */
        char *p = line + strlen(line) - 1;
        while (p >= line && (*p == '\r' || *p == '\n')) {
            *p = '\0';
            p--;
        }

        char *trimmed = trim_whitespace(line);
        if (trimmed[0] == '\0' || trimmed[0] == '#') {
            continue; /* Skip empty line or comment */
        }

        /* Parse comma-separated fields: from,to,weight,bidirectional */
        char *from_tok = strtok(trimmed, ",");
        char *to_tok   = strtok(NULL, ",");
        char *w_tok    = strtok(NULL, ",");
        char *bi_tok   = strtok(NULL, ",");

        if (!from_tok || !to_tok || !w_tok) {
            continue; /* Invalid line format, skip */
        }

        char *from = trim_whitespace(from_tok);
        char *to   = trim_whitespace(to_tok);
        char *w_str = trim_whitespace(w_tok);
        char *bi_str = bi_tok ? trim_whitespace(bi_tok) : "1";

        /* Skip header row if present */
        char *endptr = NULL;
        double weight = strtod(w_str, &endptr);
        if (endptr == w_str) {
            /* Not a valid number, likely header (e.g. 'weight' or 'cost') */
            continue;
        }

        int bidirectional = (bi_str && (strcmp(bi_str, "1") == 0 ||
                                        strcmp(bi_str, "true") == 0 ||
                                        strcmp(bi_str, "TRUE") == 0 ||
                                        strcmp(bi_str, "yes") == 0 ||
                                        strcmp(bi_str, "YES") == 0)) ? 1 : 0;

        graph_add_edge(g, from, to, weight, bidirectional);
        edges_loaded++;
    }

    fclose(fp);
    return edges_loaded;
}
