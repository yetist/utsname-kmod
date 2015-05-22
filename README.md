# ustname-kmod
linux kernel module to use sysfs for read/write uname

# Compile

make
make modules_install

# Usage

after modprobe utsname, you will read/write this files to change `uname` output:

    /sys/kernel/uname/domainname
    /sys/kernel/uname/machine
    /sys/kernel/uname/nodename
    /sys/kernel/uname/release
    /sys/kernel/uname/sysname
    /sys/kernel/uname/version
