#include "pico_wifi_manager/pico_wifi_manager.hpp"

#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"
#include "cyw43.h"

using namespace WM;

WifiManager WifiManager::s_wm;

int auth_modes[] = {
  [AUTH_WPA2_AES_PSK] = CYW43_AUTH_WPA2_AES_PSK,
};

static const char* cyw43_tcpip_link_status_name(int status)
{
    switch (status) {
    case CYW43_LINK_DOWN:
        return "link down";
    case CYW43_LINK_JOIN:
        return "joining";
    case CYW43_LINK_NOIP:
        return "no ip";
    case CYW43_LINK_UP:
        return "link up";
    case CYW43_LINK_FAIL:
        return "link fail";
    case CYW43_LINK_NONET:
        return "network fail";
    case CYW43_LINK_BADAUTH:
        return "bad auth";
    }
    return "unknown";
}

WifiManager::WifiManager(){
}

WifiManager::~WifiManager(){}

void WifiManager::init()
{
  m_linkStatus = CYW43_LINK_UP+1;
  cyw43_arch_init();
  cyw43_arch_enable_sta_mode();
}

void WifiManager::addNetwork(const char *ssid, const uint8_t *bssid, const char *passwd, uint32_t auth){
  KnownNetwork *n = new KnownNetwork;
  n->ssid = ssid;
  // memcpy(n->bssid, bssid, 6);
  n->bssid = bssid;
  n->password = passwd;
  n->auth = auth_modes[auth];

  KnownNetwork **_ = &m_knownNetworks;
  while(*_)_ = &(*_)->next;
  *_ = n;
}

void WifiManager::addEventHandler(EventHandler handler, void* arg){
  EHList *l = new EHList;
  l->handler = handler;
  l->data = arg;
  l->next = m_eventHandlers;
  m_eventHandlers = l;

}

void WifiManager::poll(){
  WifiEvent ev;
  while(m_eventQueue.try_remove(ev)){
    switch(ev.type){
      case LinkStatusChange:
        onLinkStatusChange();
      break;
    }
    for(EHList *eh = m_eventHandlers; eh; eh = eh->next){
      eh->handler(ev, eh->data);
    }
  }
  //check link status;
  int newLinkStatus = getTcpLinkStatus();
  if(newLinkStatus != m_linkStatus){
    m_linkStatus = newLinkStatus;
    printf("connect status: %s\n", cyw43_tcpip_link_status_name(m_linkStatus));
    m_eventQueue.try_add({WifiEventType::LinkStatusChange});

  }

  if(m_linkStatus == TcpLinkStatus::NONET
  || m_linkStatus == TcpLinkStatus::DOWN
  || m_linkStatus == TcpLinkStatus::FAIL){
    selectNextNetwork();
    if(m_currentNetwork){
      printf("[WM] trying to connect to %s, bssid: %p, password: %s, auth: %08x\n", m_currentNetwork->ssid, m_currentNetwork->bssid, m_currentNetwork->password, m_currentNetwork->auth);
      cyw43_arch_wifi_connect_bssid_async(m_currentNetwork->ssid, m_currentNetwork->bssid, m_currentNetwork->password, m_currentNetwork->auth);
    }
  }

  cyw43_arch_poll();
}

int WifiManager::getTcpLinkStatus(){
  return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
}
int WifiManager::getWifiLinkStatus(){
  return cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
}

void WifiManager::selectNextNetwork(){
  if(!m_knownNetworks)
    return;
  if(!m_currentNetwork){
    m_currentNetwork = m_knownNetworks;
    return;
  }
  if(m_currentNetwork->next)
    m_currentNetwork = m_currentNetwork->next;
  else{
    m_currentNetwork = m_knownNetworks;
    //set retry timeout?
  }
}