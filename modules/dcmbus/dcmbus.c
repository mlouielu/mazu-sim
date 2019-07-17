#include "dcmbus.h"

static struct channel_config g_channel_config[8];

static int dcmbus_load_channel_conf(const char *path) {
    int numEntries,idx;
    char specifier[] = CHANNEL_SPECIFIER;
    memset(g_channel_config, 0, sizeof(g_channel_config));
    read_config(g_channel_config, &numEntries, path, specifier);
    printf("Load Entries: %d \n", numEntries);
    printf("%16s %16s %16s %16s %16s %16s %16s %16s %16s\n", CHANNEL_FIELDS_NAME);
    for (idx = 0; idx < numEntries; ++idx) {
        fprintf(stdout, CHANNEL_PRINTF_FORMAT,
                        g_channel_config[idx].ch_name,
                        g_channel_config[idx].enable,
                        g_channel_config[idx].direction,
                        g_channel_config[idx].type,
                        g_channel_config[idx].ifname,
                        g_channel_config[idx].netport,
                        g_channel_config[idx].driver_idx,
                        g_channel_config[idx].blocking,
                        g_channel_config[idx].options);
    }
    return numEntries;
}


static int dcmbus_channel_create(struct dcmbus_ctrlblk_t* D, struct channel_config* conf) {
    struct dcmbus_channel_blk_t *item = NULL;
    item = (struct dcmbus_channel_blk_t *) malloc(sizeof(*item));
    memset(item, 0, sizeof(*item));
    if(!item)
        goto error_malloc;
    item->drv_ops = dcmbus_drivers[conf->driver_idx];
    strncpy(item->ch_name, conf->ch_name, 16);
    strncpy(item->ifname, conf->ifname, 16);
    item->netport = conf->netport;
    item->enable = conf->enable;
    list_add_tail(&item->list, &(D->channel_lhead));
    return 0;
error_malloc:
    fprintf(stderr, "[%s:%d] Allocate Fail\n", __func__, __LINE__);
    return -1;
}

int dcmbus_ctrlblk_init(struct dcmbus_ctrlblk_t* D, const char *path, int system_type) {

    int num_channels;
    int idx, rc = 0;
    struct dcmbus_channel_blk_t *item = NULL, *is = NULL;
    D->system_type = system_type;
    INIT_LIST_HEAD(&D->channel_lhead);

    num_channels = dcmbus_load_channel_conf(path);
    for (idx = 0; idx < num_channels; ++idx) {
        if ((rc = dcmbus_channel_create(D, &g_channel_config[idx])) < 0) {
            fprintf(stderr, "[%s:%d] Channel %d create fail !!\n", __func__, __LINE__, idx);
        }
    }

    list_for_each_entry_safe(item, is, &D->channel_lhead, list) {
        if (item->enable) {
            printf("[%s] %s:%d\n", item->ch_name, item->ifname, item->netport);
            item->drv_ops->open_interface(&item->drv_priv_data, item->ifname, item->netport);
        }
    }
    return 0;
}