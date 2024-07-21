#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

void isRoot(void) {
  int uid = geteuid();

  if (uid != 0) {
    printf("This scipt needs to be run as root!\n");
    exit(1);
  }
}

const char* getUserInput(void) {
  printf("Running lsblk\n");
  system("lsblk");
  printf("Please enter the drive you want to mount (eg. /dev/sda) : ");
  char* drive;
  scanf("%s", drive);

  return drive;
}

const char* getDriveNum(void) {
  printf("Running blkid.\n");
  system("blkid");
  printf("Please enter the number of root partition (eg. if root is /dev/sda3, enter 3): ");
  char* num;
  scanf("%s", num);
  return num;
}

const char* getType(const char* device) {
  static char buf[1035];

  char* type;
  char command[50];

  strcpy(command, "blkid -s TYPE -o value ");
  strcat(command, device);

  FILE* fp = popen(command, "r");
  if (fp == NULL) {
    printf("Failed to get the type of partition %s\n", device);
    exit(1);
  }

  while (fgets(buf, sizeof(buf), fp) != NULL) {
    type = buf;
  }
  pclose(fp);

  return type;
}

bool isEfi(void) {
  const char *efiPath = "/sys/firmware/efi/fw_platform_size";

  if (access(efiPath, F_OK) == -1) {
    printf("This system is being run in BIOS mode.");
    printf("Will not be mounting the ESP partition!");
    return false;
  }
  printf("This system is being run in EFI mode.");
  printf("Trying to mount the ESP partition!");
  return true;
}

void mountDrive(const char *drive, const char *destination, const char *fstype, unsigned long opts) {
  const char *null = NULL;
  mount(drive, destination, fstype, opts, null);
}

int main() {

  isRoot();
  const char* drive = getUserInput();

  // mount root
  char* root;
  const char* rootNum = getDriveNum();

  strcpy(root, drive);
  strcat(root, rootNum);

  const char* rootType = getType(root);

  /*
  mount efi, then ask if a seperate /boot exists
  if yes, jump to mount /boot
  if no, continue as normal
  */


  if (isEfi()) {
    printf("Trying to mount the ESP partition as the first partition on the disk.\n");
    char* esp;
    strcpy(esp, drive);
    strcat(esp, "1");

    const char* destination = "/mnt/gentoo/efi";
    const char* fstype = "vfat";
    unsigned long opts = 0;

    mountDrive(esp, destination, fstype, opts);

    printf("\nIs there a separate boot partition? Yy/Nn (default: no): ");
    char* yesBoot;
    scanf("%s", yesBoot);

    if (yesBoot == "y" || yesBoot == "Y") {
      goto bootMount;
    }

}
  bootMount:
    const char* bootNum = getDriveNum();
    char* bootDevice;
    strcpy(bootDevice, drive);
    strcat(bootDevice, bootNum);
    const char* bootType = getType(bootDevice);

  // mount swap if it exists

  /*
  ask if /home is separate
  if yes, mount /home
  if no, continue
  */

  /*
  ask if /var is separate
  if yes, mount /var
  if no, continue to chrooting
  */



  return 0;
}
