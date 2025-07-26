/**
 * @file
 * @brief
 *
 * @date 26.07.25
 * @author Anton Bondarev
 */

#ifndef EMBOX_NET_WIFI_CFG80211_H_
#define EMBOX_NET_WIFI_CFG80211_H_

#include <stdint.h>

#include <lib/libds/dlist.h>

struct net_device;

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

struct wiphy {
	struct ieee80211_supported_band *bands[NUM_NL80211_BANDS];
};

struct wireless_dev {
	/* the remainder of this struct should be private to cfg80211 */
	struct dlist_head list;
	struct net_device *netdev;
};

struct cfg80211_ops {
	int	(*connect)(struct wiphy *wiphy, struct net_device *dev,
						struct cfg80211_connect_params *sme);
	int	(*update_connect_params)(struct wiphy *wiphy,
						struct net_device *dev,
						struct cfg80211_connect_params *sme,
						uint32_t changed);
	int	(*disconnect)(struct wiphy *wiphy, struct net_device *dev,
						uint16_t reason_code);
};

#endif /* EMBOX_NET_WIFI_CFG80211_H_ */
