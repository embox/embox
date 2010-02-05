OBJS-$(MONITOR_NET_ARP)     += arp.o
OBJS-$(MONITOR_NET_IP_V4)   += ip_input.o
OBJS-$(MONITOR_NET_IP_V4)   += ip_output.o
OBJS-$(MONITOR_NET_ICMP)    += icmp.o
OBJS-$(MONITOR_NET_UDP)     += udp.o
OBJS-$(MONITOR_NET_SOCKRAW) += raw.o
#OBJS-$(MONITOR_NET_TFTP)    += tftp.o
#OBJS-$(MONITOR_NET_DHCP)    += dhcp.o
#OBJS-$(MONITOR_NET_BOOTP)   += bootp.o
OBJS-y                      += protocol.o
OBJS-y                      += route.o
OBJS-y                      += af_inet.o
OBJS-y                      += ipconfig.o

include $(SCRIPTS_DIR)/common.mk
