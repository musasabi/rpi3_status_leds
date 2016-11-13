#include "NetworkData.h"
#include "LEDControl.h"

#include <fcntl.h>
#include <cerrno>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

struct packet
{
    struct icmphdr header;
    char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

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
  for(uint32_t host = 0; host < MAX_HOSTS; host++)
  {
    if(hosts_reached[host]) led->turn_on(host, NET_LED);
    else led->turn_off(host, NET_LED);
  }
}

bool NetworkData::ping(string host)
{
  int pid = getpid();
  struct protoent *proto = getprotobyname("ICMP");

  struct hostent *hname = gethostbyname(host.c_str());
  if(!hname)
  {
    cerr << "Failed to get host by name for " << host << "!" << endl;
    return false;
  }

  struct sockaddr_in addr_ping,*addr;
  memset(&addr_ping, 0, sizeof(addr_ping));
  addr_ping.sin_family = hname->h_addrtype;
  addr_ping.sin_port = 0;
  addr_ping.sin_addr.s_addr = *(long*)hname->h_addr;

  addr = &addr_ping;

  int sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
  if(sd < 0)
  {
    cerr << "Could not create socket." << endl;
    return false;
  }

  const int val=255;
  if(setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
  {
    cerr << "Could not set socket options." << endl;
    close(sd);
    return false;
  }

  if(fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
  {
    cerr << "Error requesting nonblocking I/O" << endl;
    close(sd);
    return false;
  }

  int cnt = 1;
  struct packet pckt;
  struct sockaddr_in r_addr;

  for(int loop=0;loop < 5; loop++)
  {
    socklen_t len = sizeof(r_addr);

    if(recvfrom(sd, &pckt, sizeof(pckt), 0,
       (struct sockaddr*)&r_addr, &len) > 0)
    {
      close(sd);
      return true;
    }

    memset(&pckt, 0, sizeof(pckt));
    pckt.header.type = ICMP_ECHO;
    pckt.header.un.echo.id = pid;

    int i;

    for(i = 0; i < (int) sizeof(pckt.msg)-1; i++)
    {
      pckt.msg[i] = i+'0';
    }

    pckt.msg[i] = 0;
    pckt.header.un.echo.sequence = cnt++;
    pckt.header.checksum = checksum(&pckt, sizeof(pckt));

    if(sendto(sd, &pckt, sizeof(pckt), 0,
       (struct sockaddr*)addr, sizeof(*addr)) <= 0)
    {
      cerr << "sendto failed" << endl;
      return false;
    }

    usleep(300000);
  }

  close(sd);
  return false;
}

uint16_t NetworkData::checksum(void *b, int32_t len)
{
  unsigned short *buf = (short unsigned int *) b;
  unsigned int sum=0;
  unsigned short result;

  for(sum = 0; len > 1; len -= 2)
  {
    sum += *buf++;
  }

  if(len == 1) sum += *(unsigned char*)buf;
 
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;

  return result;
}
