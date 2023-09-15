#pragma once

// #include "queue.hpp"
#include "pico/util/queue.h"


namespace WM{


  template<class T>
  class Queue{
  public:
    Queue(size_t count){
      queue_init(&queue, sizeof(T), count);
    }
    ~Queue(){
      queue_free(&queue);
    }
    bool try_add(const T& t){
      return queue_try_add(&queue, &t);
    }
    bool try_remove(T& t){
      return queue_try_remove(&queue, &t);
    }
    bool is_empty() const{
      return queue_is_empty(&queue);
    }
    bool is_full() const{
      return queue_is_full(&queue);
    }


  private:
    queue_t queue;
  };
  class WifiManager;

  enum WifiAuth{
      AUTH_WPA2_AES_PSK,
  };

  enum TcpLinkStatus{
      BADAUTH = -3,
      NONET = -2,
      FAIL = -1,
      DOWN = 0,
      JOIN = 1,
      NOIP = 2,
      UP = 3,
  };


  enum WifiEventType{
      LinkStatusChange,
  };
  struct WifiEvent{
      WifiEventType type;
      // WifiManager *wm;
  };

  class WifiManager final{
  public:
      WifiManager();
      ~WifiManager();

      void init();

      void poll();
      int getTcpLinkStatus();
      int getWifiLinkStatus();

      /**
       * Get the connection status/signal strength of network
       * returns -x for signal strength (dB)
       * returns 0 for not connected
      */
      int getWifiConnectionStatus();

      void addNetwork(const char *ssid, const uint8_t *bssid, const char *passwd, uint32_t auth);

      using EventHandler = void (*)(WifiEvent, void*);
      void addEventHandler(EventHandler handler, void*);
      void removeEventHandler(EventHandler handler, void*);

      static WifiManager &instance(){
        return s_wm;
      }

  protected:
      void onLinkStatusChange(){};
  private:
      static WifiManager s_wm;
      Queue<WifiEvent> m_eventQueue{32};
      struct EHList{
          EventHandler handler;
          void *data;
          EHList *next;
      } *m_eventHandlers = nullptr;
      struct KnownNetwork{
          const char *ssid;
          const uint8_t *bssid;
          const char *password;
          uint32_t auth;
          KnownNetwork *next = nullptr;
      } *m_knownNetworks = nullptr, *m_currentNetwork = nullptr;


      int m_linkStatus = 100;

      void selectNextNetwork();
  };
    
} // namespace WM