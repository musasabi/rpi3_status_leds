#include "NetworkData.h"
#include "LEDControl.h"

#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <netdb.h>

using namespace std;

NetworkData::NetworkData(LEDControl *led_control)
{
  led = led_control;

  for(auto& host : hosts_reached)
  {
    host = false;
  }
  update();
}

void NetworkData::update()
{
  hosts_reached[serverpi] = ping(hostnames[serverpi]);
  hosts_reached[eah] = ping(hostnames[eah]);

  update_leds();
}

void NetworkData::display()
{
  for(uint32_t host = 0; host < MAX_HOSTS; host++)
  {
    cout << hostnames[host] << " reached? "
         << (hosts_reached[host]?"yes":"no") << endl;
  }
}

void NetworkData::update_leds()
{
  if(hosts_reached[serverpi]) led->turn_on(net_led_1);
  else led->turn_off(net_led_1);

  if(hosts_reached[eah]) led->turn_on(net_led_2);
  else led->turn_off(net_led_2);
}

bool NetworkData::ping(string host)
{
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if(sock < 0)
  {
    cerr << "Failed to create socket!" << endl;
    close(sock);
    return false;
  }

  sockaddr_in pingaddr;
  memset(&pingaddr, 0, sizeof(sockaddr_in));
  pingaddr.sin_family = AF_INET;

  hostent *h = gethostbyname(host.c_str());
  if(not h)
  {
    cerr << "Failed to get host by name for " << host << "!" << endl;
    close(sock);
    return false;
  }

  memcpy(&pingaddr.sin_addr, h->h_addr, sizeof(pingaddr.sin_addr));

  // Set the ID of the sender (will go into the ID of the echo msg)
  int pid = getpid();

  // Only want to receive the following messages
  icmp_filter filter;
  filter.data = ~((1<<ICMP_DEST_UNREACH) |
                  (1<<ICMP_ECHOREPLY));
  if(setsockopt(sock, SOL_RAW, ICMP_FILTER, (char *)&filter, sizeof(filter))
     < 0)
  {
    perror("setsockopt(ICMP_FILTER)");
    close(sock);
    return false;
  }

  // Send the packet
  char packet[sizeof(icmphdr)];
  memset(packet, 0, sizeof(packet));

  icmphdr *pkt = (icmphdr *)packet;
  pkt->type = ICMP_ECHO;
  pkt->code = 0;
  pkt->checksum = 0;
  pkt->un.echo.id = htons(pid & 0xFFFF);
  pkt->un.echo.sequence = 0;
  pkt->checksum = checksum((uint16_t *)pkt, sizeof(packet));

  int bytes = sendto(sock, packet, sizeof(packet), 0, (sockaddr *)&pingaddr,
                     sizeof(sockaddr_in));
  if(bytes < 0)
  {
    cerr << "Failed to send to receiver" << endl;
    close(sock);
    return false;
  }
  else if(bytes != sizeof(packet))
  {
    cerr << "Failed to write the whole packet." << endl;
    close(sock);
    return false;
  }

  while(true)
  {
    char inbuf[192];
    memset(inbuf, 0, sizeof(inbuf));
    int addrlen = sizeof(sockaddr_in);

    memset(&pingaddr, 0, sizeof(sockaddr_in));
    pingaddr.sin_family = AF_INET;

    hostent *h = gethostbyname(host.c_str());
    if(not h)
    {
      cerr << "Failed to get host by name!" << endl;
      close(sock);
      return false;
    }

    bytes = recvfrom(sock, inbuf, sizeof(inbuf), 0, (sockaddr *)&pingaddr,
                     (socklen_t *)&addrlen);
    if(bytes < 0)
    {
      cerr << "Error on recvfrom" << endl;
      close(sock);
      return false;
    }
    else
    {
      if(bytes < (int) (sizeof(iphdr) + sizeof(icmphdr)))
      {
        cerr << "Incorrect read bytes!" << endl;
        continue;
      }

      iphdr *iph = (iphdr *)inbuf;
      int hlen = (iph->ihl << 2);
      bytes -= hlen;

      pkt = (icmphdr *)(inbuf + hlen);
      if(pkt->type == ICMP_ECHOREPLY)
      {
//        cout << "ICMP_ECHOREPLY" << endl;
        return true;
      }
      else if(pkt->type == ICMP_DEST_UNREACH)
      {
        cerr << "ICMP_DEST_UNREACH" << endl;
        return false;
      }
      else return false;
    }
  }
}

int32_t NetworkData::checksum(uint16_t *buf, int32_t len)
{
  int32_t nleft = len;
  int32_t sum = 0;
  uint16_t *w = buf;
  uint16_t answer = 0;

  while(nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }

  if(nleft == 1)
  {
    *(uint16_t *)(&answer) = *(uint8_t *)w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;

  return answer;
}
