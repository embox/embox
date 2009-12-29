/**
 * \fiile ifconfig.c
 *
 * \date Mar 22, 2009
 * \author anton
 */
#include "shell_command.h"
#include "string.h"
#include "net/net.h"
#include "net/etherdevice.h"
#include "net/inetdevice.h"
#include "net/netdevice.h"
#include "misc.h"

#define COMMAND_NAME     "ifconfig"
#define COMMAND_DESC_MSG "configure a network interface"
static const char *man_page =
	#include "ifconfig_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

/**
 * Show interace (IP/MAC address)
 * @param in_dev interface handler
 */
static void inet_dev_show_info(void *handler) {
	in_device_t *in_dev = (in_device_t *) handler;
	net_device_t *dev = in_dev->dev;
	unsigned char mac[18];
	macaddr_print(mac, dev->dev_addr);

	net_device_stats_t *eth_stat;
	eth_stat = dev->netdev_ops->ndo_get_stats(dev);
	TRACE("%s\tencap:", dev->name);
	if(dev->flags & IFF_LOOPBACK) {
    		TRACE("Local Loopback");
    } else {
            TRACE("Ethernet");
    }
    TRACE(" HWaddr %s\n", mac);
	struct in_addr ip, bcast, mask;
	ip.s_addr = in_dev->ifa_address;
	bcast.s_addr = in_dev->ifa_broadcast;
	mask.s_addr = in_dev->ifa_mask;
	char *s_ip = inet_ntoa(ip);
	char *s_bcast = inet_ntoa(bcast);
	char *s_mask = inet_ntoa(mask);
	TRACE("\tinet addr:%s Bcast:%s  Mask:%s\n\t", s_ip, s_bcast, s_mask);
	free(s_ip);
	free(s_bcast);
	free(s_mask);
	if(dev->flags & IFF_UP)
	        TRACE("UP ");
	if(dev->flags & IFF_BROADCAST)
	        TRACE("BROADCAST ");
	if(dev->flags & IFF_DEBUG)
	        TRACE("DEBUG ");
	if(dev->flags & IFF_LOOPBACK)
	        TRACE("LOOPBACK ");
	if(dev->flags & IFF_RUNNING)
	        TRACE("RUNNING ");
	if(dev->flags & IFF_PROMISC)
	        TRACE("PROMISC ");
	if(dev->flags & IFF_MULTICAST)
	        TRACE("MULTICAST ");
	TRACE("MTU:%d  Metric:%d\n",dev->mtu,0);
	TRACE("\tRX packets:%d errors:%d dropped:%d overruns:%d frame:%d\n",
		eth_stat->rx_packets, eth_stat->rx_err,
	        eth_stat->rx_dropped, eth_stat->rx_over_errors,
	                            eth_stat->rx_frame_errors);
	TRACE("\tTX packets:%d errors:%d dropped:%d overruns:%d carrier:%d\n",
		eth_stat->tx_packets, eth_stat->tx_err,
	        eth_stat->tx_dropped, eth_stat->rx_over_errors,
	                            eth_stat->tx_carrier_errors);
	TRACE("\tcollisions:%d txqueuelen:%d\n",
		eth_stat->collisions, dev->tx_queue_len);
	TRACE("\tRX bytes:%d (%d MiB)  TX bytes:%d (%d MiB)\n",
		eth_stat->rx_bytes, eth_stat->rx_bytes/1048576,
                eth_stat->tx_bytes, eth_stat->tx_bytes/1048576);
    TRACE("\tInterrupt:%d Base address:0x%08X\n", dev->irq, dev->base_addr);
}

/**
 * Show all eth interfaces (IP/MAC address)
 */
static void inet_dev_show_all_info() {
	in_device_t * ifdev = inet_dev_get_fist_used();
        if (NULL != ifdev){
                inet_dev_show_info(ifdev);
        }
        while(NULL != (ifdev = inet_dev_get_next_used())){
                inet_dev_show_info(ifdev);
        }
}

static int exec(int argsc, char **argsv) {
	in_device_t *in_dev = NULL;
	struct in_addr ipaddr;
	ipaddr.s_addr = 0;
	struct in_addr mask;
	mask.s_addr = 0;
	unsigned char macaddr[ETH_ALEN];
	macaddr[0] = 0;
	unsigned char broadcastaddr[ETH_ALEN];
	broadcastaddr[0] = 0;
	int up = 0, down = 0, i = 0;
	int arp = 0, promisc = 0, allmulti = 0, multicast = 0, mtu = 0;
	unsigned int irq_num = 0;
	unsigned long tx_queue_len = 0, base_addr = 0;
	unsigned char iname[IFNAMSIZ];
        int nextOption;
        getopt_init();
        //and what about loopback, pointopoint and debug??
        do {
                nextOption = getopt(argsc, argsv, "a:p:m:udx:r:f:c:g:l:b:i:t:w:h");
                switch(nextOption) {
                case 'h':   //++
                        show_help();
                        return 0;
                case 'u':   //++
            		up = 1;
            		break;
            	case 'd':  //++
            		down = 1;
            		break;

            	case 'x':  //++
            	    sscanf(optarg, "eth%d", &i);
            	    strncpy(iname, optarg, array_len(iname));
            	    TRACE("iface = %s\n", optarg);
            	    if (!(in_dev = inet_dev_find_by_name(optarg))) {
            	        LOG_ERROR("can't find interface %s\n", optarg);
            	        return -1;
            	    }
            	    break;

            	case 'a':  //++
            	    if (0 == inet_aton(optarg, &ipaddr)) {
                        LOG_ERROR("wrong ip addr format %s\n", optarg);
                        return -1;
            	    }
            	    break;
            	case 'p':  //++
            	    if (0 == inet_aton(optarg, &mask)) {
            	        LOG_ERROR("wrong mask format %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'm':  //++
            	    if (NULL == macaddr_scan(optarg, macaddr)) {
            	        LOG_ERROR("wrong mac addr format %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'r':  //arp
            	    if (1 != sscanf(optarg, "%d", &arp)) {
            	        LOG_ERROR("wrong -b argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'f':  //promisc
                    if (1 != sscanf(optarg, "%d", &promisc)) {
                        LOG_ERROR("wrong -f argument %s\n", optarg);
                        return -1;
                    }
            	    break;
            	case 'c':  //allmulti
                    if (1 != sscanf(optarg, "%d", &allmulti)) {
                        LOG_ERROR("wrong -c argument %s\n", optarg);
                        return -1;
                     }
            	    break;
            	case 'g':  //multicast++
            	    if (1 != sscanf(optarg, "%d", &multicast)) {
            	        LOG_ERROR("wrong -g argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'l': //mtu length++
            	    if (0 == sscanf(optarg, "%d", &mtu)) {
            	        LOG_ERROR("wrong -l argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'b':  //base_addr++
            	    if (0 == sscanf(optarg, "0x%X", &base_addr)) {
            	        LOG_ERROR("wrong -b argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'i':  //irq++
            	    if (0 == sscanf(optarg, "%d", &irq_num)) {
            	        LOG_ERROR("wrong -i argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 't':  //txqueuelen
            	    if (1 != sscanf(optarg, "%d", &tx_queue_len)) {
            	        LOG_ERROR("wrong -t argument %s\n", optarg);
            	        return -1;
            	    }
            	    break;
            	case 'w':
                    if (NULL == macaddr_scan(optarg, broadcastaddr)) {
                         LOG_ERROR("wrong broadcast addr format %s\n", optarg);
                         return -1;
                     }
            	    break;
            	case -1:
                    break;
            	default:
            	    return 0;
                }
        } while(-1 != nextOption);

	if (argsc == 1) {
		inet_dev_show_all_info();
		return 0;
	}
	if (argsc > 1) {
		sscanf(argsv[argsc - 1], "eth%d", &i);
		strncpy(iname, argsv[argsc - 1], array_len(iname));
		if ((NULL == (in_dev = inet_dev_find_by_name(argsv[argsc - 1]))) && (up == 0)) {
			LOG_ERROR("can't find interface %s\n", argsv[argsc - 1]);
			return -1;
		}
	}
	if( up == 1 ) {
	    ifdev_up(iname);//, ipaddr, macaddr);
	    inet_dev_set_interface(iname, ipaddr.s_addr, mask.s_addr, macaddr);
	    in_dev = inet_dev_find_by_name(iname);
	}
	if (arp == 0) {
	    eth_flag_up(in_dev->dev, IFF_NOARP);
	} else {
	    eth_flag_down(in_dev->dev, IFF_NOARP);
	}

	if (promisc == 1) {
	    eth_flag_up(in_dev->dev, IFF_PROMISC);     //inet_dev_listen?
	} else {
	    eth_flag_down(in_dev->dev, IFF_PROMISC);
	}

	if (allmulti == 1) {
	    eth_flag_up(in_dev->dev, IFF_ALLMULTI);    //?
	} else {
	    eth_flag_down(in_dev->dev, IFF_ALLMULTI);
	}

	if (multicast == 1) {
	    eth_flag_up(in_dev->dev, IFF_MULTICAST);
	} else {
	    eth_flag_down(in_dev->dev, IFF_MULTICAST);
	}

	if (mtu != 0) {
	    eth_change_mtu(in_dev->dev, mtu);
	}

	if (irq_num != 0) {
	    eth_set_irq(in_dev->dev, irq_num);
	}

	if (base_addr!= 0) {
	    eth_set_baseaddr(in_dev->dev, base_addr);
	}

	if (tx_queue_len != 0) {
	    eth_set_txqueuelen(in_dev->dev, tx_queue_len);
	}

	if (broadcastaddr[0] != 0) {
	    if (NULL == in_dev) {
	        LOG_ERROR("Enter interface name\n");
	        show_help();
	        return -1;
	    }
	    if (down == 0 ) {
	        eth_set_broadcast_addr(in_dev->dev, broadcastaddr);    //??
	        eth_flag_up(IFF_BROADCAST);
	    }
	}

	if( down == 1 ) {
	        ifdev_down(iname);//, ipaddr, macaddr);
	        return 0;
	}
	if (ipaddr.s_addr != 0) {
		if (NULL == in_dev) {
			LOG_ERROR("Enter interface name\n");
			show_help();
			return -1;
		}
		if (down == 0 ) {
			inet_dev_set_ipaddr(in_dev, ipaddr.s_addr);
		}
	}
	if (mask.s_addr != 0) {
		if (NULL == in_dev) {
			LOG_ERROR("Enter interface name\n");
			show_help();
			return -1;
		}
		if (down == 0 ) {
		        inet_dev_set_mask(in_dev, mask.s_addr);
		}
	}
	if (macaddr[0] != 0) {
		if (NULL == in_dev) {
			LOG_ERROR("Enter interface name\n");
			show_help();
			return -1;
		}
		if (down == 0 ) {
			inet_dev_set_macaddr(in_dev, macaddr);
		}
	}
	if (up == 1) return 0;     //whats?
	if (NULL == in_dev) {
		inet_dev_show_all_info();
	} else {
		inet_dev_show_info(in_dev);
	}

	return 0;
}
