#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libconfig.h>
#include <inttypes.h>
#include "hlist.h"

#define TAG_LEN     16
#define REGEX_LEN 128
#define PATH_LEN   128

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;
typedef unsigned char u_char;
typedef long int      l_int;
typedef unsigned long int ul_int;

#define MAX_CFG_HASH 128
struct hlist_head MTP_CFG[MAX_CFG_HASH];
typedef struct CFG_NODE
{
	struct hlist_node hlist;
	u32 crc;
	char key[128];
	char key_len;
	int type;
	union
	{
		const char * s_value;
		int i_value;
	} v;
} CFG_NODE;


#define   NAME_LEN 32
typedef struct MTP_CFG_ITEMS
{
	char name[NAME_LEN];
	int type;
} MTP_CFG_ITEMS;

#define CFG_ITEM_MAX 128

#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

MTP_CFG_ITEMS mtp_cfg_items[CFG_ITEM_MAX] =
{
	{"version",CONFIG_TYPE_STRING},
	{"vp_firm",CONFIG_TYPE_STRING},
	{"vp_version",CONFIG_TYPE_INT},
	{"province",CONFIG_TYPE_STRING},
	{"city",CONFIG_TYPE_STRING},
	{"author",CONFIG_TYPE_STRING},

	{"address.tms_ip",CONFIG_TYPE_STRING},
	{"address.ums_ip",CONFIG_TYPE_STRING},
	{"address.vp_total",CONFIG_TYPE_INT},
	{"address.vp_ip1",CONFIG_TYPE_STRING},
	{"address.vp_ip2",CONFIG_TYPE_STRING},

	{"ip_pool.begin",CONFIG_TYPE_STRING},
	{"ip_pool.end",CONFIG_TYPE_STRING},
	{"port_pool.begin",CONFIG_TYPE_INT},
	{"port_pool.end",CONFIG_TYPE_INT},
	{"platform",CONFIG_TYPE_GROUP}
};

MTP_CFG_ITEMS mtp_cfg_platform[CFG_ITEM_MAX] =
{
	{"onboot",CONFIG_TYPE_BOOL},
	{"is_tms_side",CONFIG_TYPE_BOOL},
	{"is_zip",CONFIG_TYPE_BOOL},


	{"network.listen_ip",CONFIG_TYPE_STRING},
	{"network.listen_port",CONFIG_TYPE_INT},
	{"network.dest_ip",CONFIG_TYPE_STRING},
	{"network.dest_port",CONFIG_TYPE_INT},
	{"network.bind_ip",CONFIG_TYPE_STRING},
	{"network.bind_port",CONFIG_TYPE_INT},
	{"network.protocol",CONFIG_TYPE_STRING},

	{"acl.rule",CONFIG_TYPE_STRING},

	{"user.pkt_id",CONFIG_TYPE_STRING}	,
	{"user.name",CONFIG_TYPE_STRING},

	{"camera.pkt_id",CONFIG_TYPE_STRING},
	{"camera.name",CONFIG_TYPE_STRING}	,

	{"sip.enable",CONFIG_TYPE_BOOL},


	{"flow.enable",CONFIG_TYPE_BOOL},
	{"flow.is_cmd_type",CONFIG_TYPE_BOOL},

	{"video.enable",CONFIG_TYPE_BOOL},
	{"video.sip_request",CONFIG_TYPE_STRING},
	{"video.sip_response_pkt_id",CONFIG_TYPE_STRING},
	{"video.sip_response_channel",CONFIG_TYPE_STRING},
	{"video.camera_ip",CONFIG_TYPE_STRING},
	{"video.camera_port",CONFIG_TYPE_STRING},
	{"video.ums_ip",CONFIG_TYPE_STRING},
	{"video.ums_port",CONFIG_TYPE_INT},
	{"video.tms_ip",CONFIG_TYPE_STRING},
	{"video.tms_port",CONFIG_TYPE_INT},
	{"video.client_ip",CONFIG_TYPE_STRING}
};

static u32   CRC32[256];
static void init_crc32_table()
{
	int   i,j;
	u32   crc;
	for(i = 0; i < 256; i++)
	{
		crc = i;
		for(j = 0; j < 8; j++)
		{
			if(crc & 1)
			{
				crc = (crc >> 1) ^ 0xEDB88320;
			}
			else
			{
				crc = crc >> 1;
			}
		}
		CRC32[i] = crc;
	}
}

u32 crc32(u8 *buf, int len)
{
	u32 ret = 0xFFFFFFFF;
	static char   init = 0;
	int   i;
	if( !init )
	{
		init_crc32_table();
		init = 1;
	}
	for(i = 0; i < len; i++)
	{
		ret = CRC32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
	}
	ret = ~ret;
	return ret;
}

int key_exists(struct hlist_head *head, CFG_NODE * cfg_node)
{
	struct CFG_NODE  * node;
	struct hlist_node *hlistnode;
	hlist_for_each_entry(node, hlistnode, head,hlist)
	{
		if (node-> crc == cfg_node->crc)
		{
			if(node->key_len == cfg_node->key_len)
			{
				if(!strcmp(node->key,cfg_node->key))
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

int group_read_config_item(config_t * cfg,char * group_name,config_setting_t * channel,MTP_CFG_ITEMS * item)
{
	CFG_NODE * cfg_node = (CFG_NODE *)calloc(1,sizeof(CFG_NODE));

	u32 index = 0;
	//platform config
	sprintf(cfg_node->key,"%s.%s.%s",group_name,channel->name,item->name);
	cfg_node->key_len = strlen(cfg_node->key);
	cfg_node->crc = crc32(cfg_node->key,	cfg_node->key_len);
	cfg_node->type = item->type;
	if(item->type == CONFIG_TYPE_STRING)
	{
		if(CONFIG_FALSE == config_lookup_string(cfg, cfg_node->key, &(cfg_node->v.s_value)))
		{
			printf("read %s string error\n",cfg_node->key);
			return -1;
		}
	}
	else if(item->type == CONFIG_TYPE_INT)
	{
		if(CONFIG_FALSE == config_lookup_int(cfg, cfg_node->key, &(cfg_node->v.i_value)))
		{
			printf("read %s int error\n",item->name);
			return -1;
		}
	}
	else if(item->type == CONFIG_TYPE_BOOL)
	{
		if(CONFIG_FALSE == config_lookup_bool(cfg, cfg_node->key, &(cfg_node->v.i_value)))

		{
			printf("read %s bool error\n", channel, item->name);
			return -1;
		}
	}
	index = cfg_node->crc  % MAX_CFG_HASH;
	if (key_exists(&MTP_CFG[index], cfg_node) == 0) {
		hlist_add_head(&cfg_node->hlist, &MTP_CFG[index]);
	}

	return 0;
}

int read_config_item(config_t * cfg,MTP_CFG_ITEMS * item)
{
	u32 i = 0;
	u32 j = 0;
	config_setting_t * setting;
	CFG_NODE * cfg_node = (CFG_NODE *)calloc(1,sizeof(CFG_NODE));

	u32 index = 0;
	//config item
	strcpy(cfg_node->key,item->name);
	cfg_node->key_len = strlen(cfg_node->key);
	cfg_node->crc = crc32(cfg_node->key,cfg_node->key_len);
	cfg_node->type = item->type;
	if(item->type == CONFIG_TYPE_STRING)
	{
		if(CONFIG_FALSE == config_lookup_string(cfg, cfg_node->key, &(cfg_node->v.s_value)))
		{
			printf("read %s string error\n",item->name);
			return -1;
		}
	}
	else if(item->type == CONFIG_TYPE_INT)
	{
		if(CONFIG_FALSE == config_lookup_int(cfg, cfg_node->key, &(cfg_node->v.i_value)))
		{
			printf("read %s int error\n",item->name);
			return -1;
		}
	}
	else if(item->type == CONFIG_TYPE_BOOL)
	{
		if(CONFIG_FALSE == config_lookup_bool(cfg, cfg_node->key, &(cfg_node->v.i_value)))
		{
			printf("read %s bool error\n",item->name);
			return -1;
		}
	}
	else if(item->type == CONFIG_TYPE_GROUP)
	{
		setting = config_lookup(cfg, item->name);
		int count = config_setting_length(setting);

		for(i = 0; i < count; ++i)
		{
			config_setting_t *channel = config_setting_get_elem(setting, i);
			for(j = 0; j <ARRAY_SIZE(mtp_cfg_platform); j++)
			{
				if(mtp_cfg_platform[j].name[0])
				{
					group_read_config_item(cfg,item->name,channel,&mtp_cfg_platform[j]);
				}
			}
		}
	}
	index = cfg_node->crc  % MAX_CFG_HASH;
	if (key_exists(&MTP_CFG[index], cfg_node) == 0) {
		hlist_add_head(&cfg_node->hlist, &MTP_CFG[index]);
	}

}


int mtp_init_cfg(config_t * cfg,char * file)
{
	config_setting_t *setting;
	u32 i = 0;
	config_init(cfg);
	init_crc32_table();

	for (i = 0; i < MAX_CFG_HASH; i++) {
		INIT_HLIST_HEAD(&MTP_CFG[i]);
	}
	config_read_file(cfg, file);

	for(i = 0; i < ARRAY_SIZE(mtp_cfg_items); i++)
	{
		if(mtp_cfg_items[i].name[0])
		{
			read_config_item(cfg,&mtp_cfg_items[i]);
		}
	}
	return 0;
}

int mtp_print_cfg( )
{
	CFG_NODE * node;
	struct hlist_node *hlistnode;
	u32 i = 0;

	for (i = 0; i < MAX_CFG_HASH; i++) {
		hlist_for_each_entry(node, hlistnode, &MTP_CFG[i],hlist) {
			if(node->type == CONFIG_TYPE_STRING)
				printf("[%s]=%s\n", node->key,node->v.s_value);
			else 	if(node->type == CONFIG_TYPE_INT)
				printf("[%s]=%d\n", node->key,node->v.i_value);
		}
	}
	return 0;
}


int mtp_clear_cfg(config_t * cfg)
{
	CFG_NODE * node;
	CFG_NODE * tmp_node;
	struct hlist_node *hlistnode;
	struct hlist_node *t_hlistnode;
	u32 i = 0;
	config_destroy(cfg);
	for (i = 0; i < MAX_CFG_HASH; i++) {
		hlist_for_each_entry_safe(node, hlistnode, t_hlistnode,&MTP_CFG[i],hlist) {
			hlist_del(&node->hlist);
			free(node);
		}
	}
	return 0;
}



int main(int argc, char* argv[])
{
	config_t* cfg = &(config_t){};
	mtp_init_cfg(cfg,"./mtp.conf");
	mtp_print_cfg();	
	mtp_clear_cfg(cfg);
	mtp_print_cfg();
	return EXIT_SUCCESS;
}
