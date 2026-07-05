#include <arpa/inet.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>

#include "myheader.h"

void got_packet(u_char* args, const struct pcap_pkthdr* header,
                const u_char* packet) {
  // Ehternet header : src mac / dst mac
  struct ethheader* eth = (struct ethheader*)packet;

  printf("=======Ethernet Header=======\n");
  printf("src mac : %02x:%02x:%02x:%02x:%02x:%02x\n", eth->ether_shost[0],
         eth->ether_shost[1], eth->ether_shost[2], eth->ether_shost[3],
         eth->ether_shost[4], eth->ether_shost[5]);

  printf("dst mac : %02x:%02x:%02x:%02x:%02x:%02x\n", eth->ether_dhost[0],
         eth->ether_dhost[1], eth->ether_dhost[2], eth->ether_dhost[3],
         eth->ether_dhost[4], eth->ether_dhost[5]);

  // IP header : src ip / dst ip
  if (ntohs(eth->ether_type) == 0x0800) {  // 0x0800 is IP type
    struct ipheader* ip = (struct ipheader*)(packet + sizeof(struct ethheader));
    int ip_header_len = ip->iph_ihl * 4;
    int ip_packet_len = ntohs(ip->iph_len);

    printf("=======IP Header=======\n");
    printf("src ip : %s\n", inet_ntoa(ip->iph_sourceip));
    printf("dst ip : %s\n", inet_ntoa(ip->iph_destip));

    // TCP header : src port / dst port
    if (ip->iph_protocol == IPPROTO_TCP) {
      struct tcpheader* tcp =
          (struct tcpheader*)(packet + sizeof(struct ethheader) +
                              ip_header_len);
      int tcp_header_len = TH_OFF(tcp) * 4;
      int src_port = ntohs(tcp->tcp_sport);
      int dst_port = ntohs(tcp->tcp_dport);

      printf("=======TCP Header=======\n");
      printf("src port : %d\n", src_port);
      printf("dst port : %d\n", dst_port);

      // HTTP : message
      if (src_port == 80 || dst_port == 80) {
        const u_char* payload =
            packet + sizeof(struct ethheader) + ip_header_len + tcp_header_len;
        int payload_len = ip_packet_len - ip_header_len - tcp_header_len;

        printf("=======HTTP=======\n");
        printf("message : \n");
        if (payload_len > 0)
          printf("%.*s\n", payload_len, payload);
        else
          printf("no data\n");
      }
    }
  }
  printf("\n\n\n\n");
}

int main() {
  pcap_t* handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "tcp port 80";  // TCP, HTTP
  bpf_u_int32 net;

  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

  pcap_compile(handle, &fp, filter_exp, 0, net);
  if (pcap_setfilter(handle, &fp) != 0) {
    pcap_perror(handle, "Error:");
    exit(EXIT_FAILURE);
  }

  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle);
  return 0;
}