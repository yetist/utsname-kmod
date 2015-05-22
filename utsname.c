#include <linux/module.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/version.h>
#include <linux/string.h>


#if LINUX_VERSION_CODE >= 132630
# define uname_uts_attrib(attrib) utsname()->attrib
#else
# define uname_uts_attrib(attrib) system_utsname.attrib
#endif

#define store_used(attrib, value) do { \
	memset(uname_used_ ##attrib, sizeof( uname_used_ ##attrib), '\0'); \
	memcpy(uname_used_ ##attrib, value, sizeof(uname_used_ ##attrib) -1); \
} while(0)

#define store_kernel(attrib, value) do { \
	memset(uname_uts_attrib(attrib), sizeof(uname_uts_attrib(attrib)), '\0'); \
	memcpy(uname_uts_attrib(attrib), value, sizeof(uname_uts_attrib(attrib)) -1); \
} while (0)

#define store_all(attrib, value) do { \
	store_used(attrib, value); \
	store_kernel(attrib, value); \
} while (0)


#define define_attribute(name, mode)  name## _attribute = __ATTR( name , mode, uts_node_show, uts_node_store)

/* save original value for uname */
static char uname_saved_sysname[sizeof(uname_uts_attrib(sysname))];
static char uname_saved_nodename[sizeof(uname_uts_attrib(nodename))];
static char uname_saved_release[sizeof(uname_uts_attrib(release))];
static char uname_saved_version[sizeof(uname_uts_attrib(version))];
static char uname_saved_machine[sizeof(uname_uts_attrib(machine))];
static char uname_saved_domainname[sizeof(uname_uts_attrib(domainname))];

/* current used value for uname */
static char uname_used_sysname[sizeof(uname_uts_attrib(sysname))];
static char uname_used_nodename[sizeof(uname_uts_attrib(nodename))];
static char uname_used_release[sizeof(uname_uts_attrib(release))];
static char uname_used_version[sizeof(uname_uts_attrib(version))];
static char uname_used_machine[sizeof(uname_uts_attrib(machine))];
static char uname_used_domainname[sizeof(uname_uts_attrib(domainname))];

static ssize_t uts_node_store (struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	char sbuf[count];
	size_t i;

	memcpy(sbuf, buf, sizeof(sbuf));

	for (i = strlen(sbuf); i >= 0; i--) {
		if (sbuf[i] == '\0') continue;
		if (sbuf[i] == '\n') {
			sbuf[i] = '\0';
			break;
		}
	}

	if (strcmp(attr->attr.name, "sysname") == 0) {
		store_all(sysname, sbuf);
	}
	else if (strcmp(attr->attr.name, "nodename") == 0) {
		store_all(nodename, sbuf);
	}
	else if (strcmp(attr->attr.name, "release") == 0) {
		store_all(release, sbuf);
	}
	else if (strcmp(attr->attr.name, "version") == 0) {
		store_all(version, sbuf);
	}
	else if (strcmp(attr->attr.name, "machine") == 0) {
		store_all(machine, sbuf);
	}
	else if (strcmp(attr->attr.name, "domainname") == 0) {
		store_all(domainname, sbuf);
	}
	return count;
}

static ssize_t uts_node_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	if (strcmp(attr->attr.name, "sysname") == 0)
		return sprintf(buf, "%s\n", uname_used_sysname);
	else if (strcmp(attr->attr.name, "nodename") == 0)
		return sprintf(buf, "%s\n", uname_used_nodename);
	else if (strcmp(attr->attr.name, "release") == 0)
		return sprintf(buf, "%s\n", uname_used_release);
	else if (strcmp(attr->attr.name, "version") == 0)
		return sprintf(buf, "%s\n", uname_used_version);
	else if (strcmp(attr->attr.name, "machine") == 0)
		return sprintf(buf, "%s\n", uname_used_machine);
	else if (strcmp(attr->attr.name, "domainname") == 0)
		return sprintf(buf, "%s\n", uname_used_domainname);
	return 0;
}

static struct kobj_attribute define_attribute(sysname, 0644);
static struct kobj_attribute define_attribute(nodename, 0644);
static struct kobj_attribute define_attribute(release, 0644);
static struct kobj_attribute define_attribute(version, 0644);
static struct kobj_attribute define_attribute(machine, 0644);
static struct kobj_attribute define_attribute(domainname, 0644);

static struct attribute *attrs[] = {
	&sysname_attribute.attr,
	&nodename_attribute.attr,
	&release_attribute.attr,
	&version_attribute.attr,
	&machine_attribute.attr,
	&domainname_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = { .attrs = attrs, };

static struct kobject *uname_kobj;

static void save_all_uname(void)
{
	memcpy(uname_saved_sysname, uname_uts_attrib(sysname), sizeof(uname_saved_sysname));
	memcpy(uname_saved_nodename, uname_uts_attrib(nodename), sizeof(uname_saved_nodename));
	memcpy(uname_saved_release, uname_uts_attrib(release), sizeof(uname_saved_release));
	memcpy(uname_saved_version, uname_uts_attrib(version), sizeof(uname_saved_version));
	memcpy(uname_saved_machine, uname_uts_attrib(machine), sizeof(uname_saved_machine));
	memcpy(uname_saved_domainname, uname_uts_attrib(domainname), sizeof(uname_saved_domainname));
}

static void store_all_uname(void)
{
	store_used(sysname, uname_uts_attrib(sysname));
	store_used(nodename, uname_uts_attrib(nodename));
	store_used(release, uname_uts_attrib(release));
	store_used(version, uname_uts_attrib(version));
	if (strcmp(uname_uts_attrib(machine), "mips64") == 0 ) {
		store_used(machine, "mips");
		store_kernel(machine, "mips");
	} else {
		store_used(machine, uname_uts_attrib(machine));
	}
	store_used(domainname, uname_uts_attrib(domainname));
}

static void restore_all_uname(void)
{
	memcpy(uname_uts_attrib(sysname), uname_saved_sysname, sizeof(uname_uts_attrib(sysname)));
	memcpy(uname_uts_attrib(nodename), uname_saved_nodename, sizeof(uname_uts_attrib(nodename)));
	memcpy(uname_uts_attrib(release), uname_saved_release, sizeof(uname_uts_attrib(release)));
	memcpy(uname_uts_attrib(version), uname_saved_version, sizeof(uname_uts_attrib(version)));
	memcpy(uname_uts_attrib(machine), uname_saved_machine, sizeof(uname_uts_attrib(machine)));
	memcpy(uname_uts_attrib(domainname), uname_saved_domainname, sizeof(uname_uts_attrib(domainname)));
}

static int __init utsname_init_module( void )
{
	int retval = 0;
	uname_kobj = kobject_create_and_add("uname", kernel_kobj);
	if (!uname_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(uname_kobj, &attr_group);
	if (retval) {
		kobject_put(uname_kobj);
		return retval;
	}
	save_all_uname();
	store_all_uname();
	return 0;
}

static void __exit utsname_cleanup_module( void )
{
	restore_all_uname();
	kobject_put(uname_kobj);
}

module_init(utsname_init_module);
module_exit(utsname_cleanup_module);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Wu Xiaotian <xiaotian.wu@i-soft.com.cn>");
