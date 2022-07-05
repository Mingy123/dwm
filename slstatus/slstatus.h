const char* datetime(const char *fmt) {
	time_t t;
	t = time(NULL);
	if (!strftime(buf, sizeof(buf), fmt, localtime(&t))) {
		warn("strftime: Result string exceeds buffer size");
		return NULL;
	}

	return buf;
}

int checkCharge() {
    FILE* f = popen("acpi | grep -v unavailable | wc -l", "r");
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

    if (!checkCharge() || batt > 80) return "\uf240";
    if (batt > 60) return "\uf241";
    if (batt > 40) return "\uf242";
    if (batt > 20) return "\uf243";
    return "\uf244";
}

const char* wifi() {
    FILE* f = popen("ip a | grep wlan0 | grep DOWN | wc -l", "r");
    char c = getc(f);
    pclose(f);
    if (c == '0') return "\uf1eb";
    return "\uf05e";
}
