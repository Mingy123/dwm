/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../slstatus.h"
#include "../util.h"

// screw openbsd im rewriting the whole thing

// 1 - not charging
int checkCharge() {
    FILE* f = popen("acpi | grep -v unavailable | grep Discharging | wc -l", "r");
    char c;
    fscanf(f, "%c", &c);
    pclose(f);
    return c-48;
}

const char* battery_perc() {
    char *p;
    FILE *fp;
    char* cmd = "acpi | grep -v unavailable | grep -o \"[0-9]*%\"";

    if (!(fp = popen(cmd, "r"))) {
        warn("popen '%s':", cmd);
        return NULL;
    } p = fgets(buf, sizeof(buf) - 1, fp);
    if (pclose(fp) < 0) {
        warn("pclose '%s':", cmd);
        return NULL;
    } if (!p) {
        return NULL;
    } if ((p = strrchr(buf, '\n'))) {
        p[0] = '\0';
    }

    return buf[0] ? buf : NULL;
}

const char* battery_state() {
    const char* disp = battery_perc();
    int batt = atoi(disp);

    if (!checkCharge()) return "\uf376";
    if (batt > 80) return "\uf240";
    if (batt > 60) return "\uf241";
    if (batt > 40) return "\uf242";
    if (batt > 20) return "\uf243";
    return "\uf244";
}
