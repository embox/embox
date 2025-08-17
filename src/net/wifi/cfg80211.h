/**
 * @file
 * @brief cfg80211 Wireless Configuration API Header
 *
 * @date 26.07.25
 * @author Anton Bondarev
 */

#ifndef EMBOX_NET_WIFI_CFG80211_H_
#define EMBOX_NET_WIFI_CFG80211_H_

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

#include <kernel/thread/sync/mutex.h>
#include <lib/libds/dlist.h>
#include <net/l2/ethernet.h>
#include <net/util/macaddr.h>

struct net_device;
struct wiphy;

//FIXME move to include/uapi/linux/nl80211.h
enum nl80211_band {
	NL80211_BAND_2GHZ,
	NL80211_BAND_5GHZ,
	NL80211_BAND_60GHZ,
	NL80211_BAND_6GHZ,
	NL80211_BAND_S1GHZ,
	NL80211_BAND_LC,

	NUM_NL80211_BANDS,
};

enum cfg80211_conn_state {
	CFG80211_DISCONNECTED,
	CFG80211_CONNECTING,
	CFG80211_CONNECTED,
	CFG80211_CONN_FAILED,
};

enum nl80211_key_mode {
	NL80211_KEY_RX_TX,
	NL80211_KEY_NO_TX,
	NL80211_KEY_SET_TX
};
// END FIXME move to include/uapi/linux/nl80211.h

//FIXME move to iclude/linux/ieee8021.h
#define IEEE80211_MAX_SSID_LEN          32
#define WLAN_REASON_DEAUTH_LEAVING      3
#define WLAN_STATUS_SUCCESS             0
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1
#define GFP_KERNEL                      0

struct cfg80211_ssid {
	uint8_t ssid[IEEE80211_MAX_SSID_LEN];
	uint8_t ssid_len;
};

struct cfg80211_bss {
	struct dlist_head list;

	uint8_t bssid[ETH_ALEN];
	uint8_t ssid[IEEE80211_MAX_SSID_LEN];
	uint8_t ssid_len;

	int16_t signal;      /* in dBm */
	uint16_t capability; /* capability info */
	uint16_t beacon_interval;

	uint32_t channel; /* channel number */
	enum nl80211_band band;

	unsigned long last_seen; /* jiffies timestamp */

	uint8_t privacy : 1; /* WEP/WPA/WPA2 */
	uint8_t wpa_version; /* 0=open, 1=WPA, 2=WPA2 */
};

struct ieee80211_channel {
	enum nl80211_band band;
	uint32_t center_freq;
	uint16_t freq_offset;
	uint16_t hw_value;
	uint32_t flags;
};

struct cfg80211_connect_params {
	const uint8_t *ssid;
	int ssid_len;
	const uint8_t *key;
	int key_len;
};

struct ieee80211_rate {
	uint32_t flags;
	uint16_t bitrate;
	uint16_t hw_value, hw_value_short;
};

/**
 * struct ieee80211_supported_band - frequency band definition
 *
 * This structure describes a frequency band a wiphy
 * is able to operate in.
 *
 */
struct ieee80211_supported_band {
	struct ieee80211_channel *channels;
	struct ieee80211_rate *bitrates;
	enum nl80211_band band;
	int n_channels;
	int n_bitrates;
	//struct ieee80211_sta_ht_cap ht_cap;
	//struct ieee80211_sta_vht_cap vht_cap;
	//struct ieee80211_sta_s1g_cap s1g_cap;
	//struct ieee80211_edmg edmg_cap;
	uint16_t n_iftype_data;
	//const struct ieee80211_sband_iftype_data __iftd *iftype_data;
};

struct wireless_dev {
	struct wiphy *wiphy;
	struct mutex wdev_mutex;
	/* the remainder of this struct should be private to cfg80211 */
	struct dlist_head list;
	struct net_device *netdev;

	enum cfg80211_conn_state conn_state;

	uint8_t current_ssid[IEEE80211_MAX_SSID_LEN];
	uint8_t ssid_len;
	uint8_t current_bssid[ETH_ALEN];
	int16_t current_signal;

	struct dlist_head scan_list; /* cfg80211_bss list */
	int scan_count;
	unsigned long last_scan_complete; /* last scan complete time */
	bool scanning;                    /* scanning in progress flag */

	unsigned long connect_time; /* connection establishment time */
	uint32_t tx_packets;
	uint32_t rx_packets;
	uint32_t tx_failed;
};

// FIXME move to util/compiler.h ?
#define __aligned(b) __attribute__((aligned(b)))

#if __has_attribute(__counted_by__)
#define __counted_by(member) __attribute__((__counted_by__(member)))
#else
#define __counted_by(member)
#endif

struct cfg80211_scan_info {
	uint64_t scan_start_tsf;
	uint8_t tsf_bssid[ETH_ALEN] __aligned(2);
	bool aborted;
};

struct cfg80211_scan_request {
	struct cfg80211_ssid *ssids;
	int n_ssids;
	//	u32 n_channels;
	//	const uint8_t *ie;
	//	size_t ie_len;
	//	u16 duration;
	//	bool duration_mandatory;
	uint32_t flags;

	uint32_t rates[NUM_NL80211_BANDS];

	struct wireless_dev *wdev;

	uint8_t mac_addr[ETH_ALEN] __aligned(2);
	uint8_t mac_addr_mask[ETH_ALEN] __aligned(2);
	uint8_t bssid[ETH_ALEN] __aligned(2);

	/* internal */
	struct wiphy *wiphy;
	unsigned long scan_start;
	struct cfg80211_scan_info info;
	//bool notified;
	//bool no_cck;
	//bool scan_6ghz;
	//u32 n_6ghz_params;
	//struct cfg80211_scan_6ghz_params *scan_6ghz_params;
	//s8 tsf_report_link_id;

	/* keep last */
	struct ieee80211_channel *channels[] __counted_by(n_channels);
};

struct key_params {
	const uint8_t *key;
	const uint8_t *seq;
	int key_len;
	int seq_len;
	uint16_t vlan_id;
	uint32_t cipher;
	enum nl80211_key_mode mode;
};

struct cfg80211_ops {
	int (*connect)(struct wiphy *wiphy, struct net_device *dev,
	    struct cfg80211_connect_params *sme);
	int (*update_connect_params)(struct wiphy *wiphy, struct net_device *dev,
	    struct cfg80211_connect_params *sme, uint32_t changed);
	int (*disconnect)(struct wiphy *wiphy, struct net_device *dev,
	    uint16_t reason_code);
	int (*scan)(struct wiphy *wiphy, struct cfg80211_scan_request *request);

	int (*add_key)(struct wiphy *wiphy, struct net_device *netdev, int link_id,
	    uint8_t key_index, bool pairwise, const uint8_t *mac_addr,
	    struct key_params *params);
	int (*get_key)(struct wiphy *wiphy, struct net_device *netdev, int link_id,
	    uint8_t key_index, bool pairwise, const uint8_t *mac_addr, void *cookie,
	    void (*callback)(void *cookie, struct key_params *));
	int (*del_key)(struct wiphy *wiphy, struct net_device *netdev, int link_id,
	    uint8_t key_index, bool pairwise, const uint8_t *mac_addr);
	int (*set_default_key)(struct wiphy *wiphy, struct net_device *netdev,
	    int link_id, uint8_t key_index, bool unicast, bool multicast);
};

#define NETDEV_ALIGN 4

struct wiphy {
	struct ieee80211_supported_band *bands[NUM_NL80211_BANDS];
	char priv[] __aligned(NETDEV_ALIGN);
};

#include <util/member.h>

//FIXME move to cfg8021_core.h ?
struct cfg80211_registered_device {
	const struct cfg80211_ops *ops;
	struct dlist_head list; /* Link for global device list */

	/* must be last because of the way we do wiphy_priv(),
	 * and it should at least be aligned to NETDEV_ALIGN */
	struct wiphy wiphy __aligned(NETDEV_ALIGN);
};
//FIXME END move to cfg8021_core.h ?

static inline struct cfg80211_registered_device *wiphy_to_rdev(struct wiphy *wiphy) {
	//return container_of(wiphy, struct cfg80211_registered_device, wiphy);
	return member_cast_out(wiphy, struct cfg80211_registered_device, wiphy);
}

extern struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops,
    int sizeof_priv, const char *requested_name);
static inline void *wiphy_priv(struct wiphy *wiphy) {
	return &wiphy->priv;
}

extern void wiphy_free(struct wiphy *wiphy);

extern int cfg80211_register_netdevice(struct net_device *dev);

extern int cfg80211_connect(struct net_device *dev,
    struct cfg80211_connect_params *sme);
extern int cfg80211_disconnect(struct net_device *dev, uint16_t reason_code);
extern void cfg80211_scan_done(struct wireless_dev *wdev, bool aborted);
extern struct cfg80211_bss *cfg80211_add_bss(struct wireless_dev *wdev,
    const uint8_t *bssid, const uint8_t *ssid, uint8_t ssid_len, int16_t signal,
    uint32_t channel, uint8_t enc_type);
extern int cfg80211_scan(struct net_device *dev,
    struct cfg80211_scan_request *request);
extern void cfg80211_clear_scan_results(struct wireless_dev *wdev);
extern void cfg80211_update_connection_state(struct wireless_dev *wdev,
    enum cfg80211_conn_state new_state);
extern void cfg80211_connect_result(struct net_device *dev, const uint8_t *bssid,
    const uint8_t *req_ie, size_t req_ie_len, const uint8_t *resp_ie,
    size_t resp_ie_len, uint16_t status, unsigned int gfp);

extern void cfg80211_disconnected(struct net_device *dev, uint16_t reason,
    const uint8_t *ie, size_t ie_len, bool locally_generated, unsigned int gfp);

extern void cfg80211_unregister_netdevice(struct net_device *dev);

#endif /* EMBOX_NET_WIFI_CFG80211_H_ */
