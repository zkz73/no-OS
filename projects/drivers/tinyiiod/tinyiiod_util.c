#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <xil_io.h>
#include "tinyiiod.h"
#include "tinyiiod_util.h"

/**
 * Get channel number
 * @param *ch channel name + channel number
 * @return channel number Ex: for "altvoltage0" return 0, for "voltage2" return 2
 */
static int32_t get_channel_number(const char *ch)
{
	char *p = (char*)ch;
	int32_t ch_num = 0;

	while(*p) {
		if(isdigit(*p))
			ch_num = strtol(p, &p, 10);
		else
			p++;
	}

	return ch_num;
}

/**
 * Get attribute ID from map based on attribute name
 * @param attr* attribute name
 * @param *map pointer to map terminated with NULL element
 * @param map_size map_size element numbers of the map
 * @return attribute ID, or negative value if attribute is not found
 */
int16_t get_attribute_id(const char *attr, const struct attrtibute_map* map)
{
	int16_t i = 0;

	if(!map)
		return -EINVAL;

	while(map[i].attr_name)
	{
		if (strequal(attr, map[i].attr_name ))
			return i;
		i++;
	}

	return -ENODEV;
}

/**
 * read all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
ssize_t read_all_attr(char *buf, size_t len,
			     const struct channel_info *channel, const struct attrtibute_map* map)
{
	int16_t i = 0, j = 0;
	char local_buf[0x1000];
	while(map[i].attr_name)
	{
		int16_t attr_length = map[i].exec((local_buf), len, channel);
		int32_t *len = (int32_t *)(buf + j);
		*len = Xil_EndianSwap32(attr_length);

		j += 4;
		if(attr_length >= 0) {
			sprintf(buf + j, "%s", local_buf);
			if (attr_length & 0x3) /* multiple of 4 */
				attr_length = ((attr_length >> 2) + 1) << 2;
			j += attr_length;
		}
		i++;
	}

	return j;
}

/**
 * write all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
ssize_t write_all_attr(char *buf, size_t len,
			      const struct channel_info *channel, const struct attrtibute_map* map)
{
	return 0;
}

ssize_t ch_exec_read_attr(const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len, struct attrtibute_map *map)
{
	int16_t attribute_id;
	const struct channel_info channel_info = {
		get_channel_number(channel),
		ch_out
	};
	attribute_id = get_attribute_id(attr, map);
	if(attribute_id >= 0) {
		return map[attribute_id].exec(buf, len, &channel_info);
	}
	if(strequal(attr, "")) {
		return read_all_attr(buf, len, &channel_info, map);
	}
	return -ENOENT;
}

ssize_t ch_exec_write_attr(const char *channel,
			    bool ch_out, const char *attr, const char *buf, size_t len, struct attrtibute_map *map)
{
	int16_t attribute_id;
	const struct channel_info channel_info = {
		get_channel_number(channel),
		ch_out
	};
	attribute_id = get_attribute_id(attr, map);
	if(attribute_id >= 0) {
		return map[attribute_id].exec((char*)buf, len, &channel_info);
	}
	if(strequal(attr, "")) {
		return write_all_attr((char*)buf, len, &channel_info, map);
	}
	return -ENOENT;
}

/**
 * Compare two strings
 * @param *str1 pointer to string 1
 * @param *str2 pointer to string 2
 * @return TRUE if strings are equal, 0 otherwise
 */
bool strequal(const char *str1, const char *str2)
{
	return !strcmp(str1, str2);
}

/**
 * Converts from string to long value
 * @param *str
 * @return long value
 */
int32_t read_value(const char *str)
{
	char *end;
	int32_t value = strtol(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}

/**
 * Converts from string to unsigned long value
 * @param *str
 * @return long value
 */
uint32_t read_ul_value(const char *str)
{
	char *end;
	uint32_t value = strtoul(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}

