#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>

// https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/hid/hid-playstation.c line 227
typedef struct {
	char contact;
	char x_lo;
	char x_hi:4, y_lo:4;
	char y_hi;
} __attribute__((packed)) touch_point_t;

// https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/hid/hid-playstation.c line 236
typedef struct {
	char lx, ly;
	char rx, ry;
	char l2, r2;
	char seq_number;
	char buttons[4];
	char reserved[4];
	short gyro[3];
	short accel[3];
	char reserved2;
	touch_point_t points[2];
	char reserved3[12];
	char status[3];
	char reserved4[8];
} __attribute__((packed)) input_report_t;

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("usage: %s /dev/hidrawX\nwhere X is a number\n", argv[0]);
		return 1;
	}
	const char* hidraw_path = argv[1];
	int fd = open(hidraw_path, O_RDONLY);
	if (fd == -1) {
		perror("couldn't open hidraw file");
		return 1;
	}

	struct hidraw_devinfo info;
	if (ioctl(fd, HIDIOCGRAWINFO, &info) != -1) {
		printf("found controller: 0x%04x:0x%04x\n", info.vendor, info.product);
	} else {
		perror("ioctl failed");
		close(fd);
		return -1;
	}

	// according to https://controllers.fandom.com/wiki/Sony_DualSense
	// reports aren't longer than 63 bytes in size
	unsigned char report[64];

	while (1) {
		if (read(fd, report, sizeof(report)) == -1) {
			perror("read error");
			close(fd);
			return -1;
		}

		if (report[0] != 0x01) continue; // not an input report

		input_report_t inp_rep = *(input_report_t*)&report[1];
		printf("lx, ly = (%hhu, %hhu) ", inp_rep.lx, inp_rep.ly);
		printf("rx, ry = (%hhu, %hhu) ", inp_rep.rx, inp_rep.ry);
		printf("l2, r2 = (%hhu, %hhu) ", inp_rep.l2, inp_rep.r2);

		printf("buttons: %04x ( ", *(int*)inp_rep.buttons);

		// https://controllers.fandom.com/wiki/Sony_DualSense#Input_Reports

		if (inp_rep.buttons[0] & 0x10) printf("square ");
		if (inp_rep.buttons[0] & 0x20) printf("cross ");
		if (inp_rep.buttons[0] & 0x40) printf("circle ");
		if (inp_rep.buttons[0] & 0x80) printf("triangle ");

		char dpad = inp_rep.buttons[0] & 0x0F;
		const char* dirs[] = {"up", "up-right", "right", "down-right", "down", "down-left", "left", "up-left", "center"};
		printf("(dpad: %s) ", dirs[(int)dpad]);

		if (inp_rep.buttons[1] & 0x01) printf("l1 ");
		if (inp_rep.buttons[1] & 0x02) printf("r1 ");
		if (inp_rep.buttons[1] & 0x04) printf("l2 ");
		if (inp_rep.buttons[1] & 0x08) printf("r2 ");
		if (inp_rep.buttons[1] & 0x10) printf("create ");
		if (inp_rep.buttons[1] & 0x20) printf("options ");
		if (inp_rep.buttons[1] & 0x40) printf("l3 ");
		if (inp_rep.buttons[1] & 0x80) printf("r3 ");

		if (inp_rep.buttons[2] & 0x01) printf("ps ");
		if (inp_rep.buttons[2] & 0x02) printf("touchpad ");
		if (inp_rep.buttons[2] & 0x04) printf("mute ");

		printf(")\n");

		usleep(1000);
	}
}