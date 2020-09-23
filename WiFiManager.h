/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEADER[] PROGMEM          = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#e6856e;color:#434243;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END[] PROGMEM      = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configurar Wi-Fi</button></form><br/><form action=\"/r\" method=\"post\"><button>Resetear</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='Nombre Wi-Fi'><br/><input id='p' name='p' length=64 type='password' placeholder='Contraseña'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>Conectar</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Escanear</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Configuración guardada.<br />Intentando conectar.<br />Si la conexión falla repita este procedimiento.</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

const char HTTP_PORTAL_LOGO[] PROGMEM     = "<div style='with:100%; margin: 0 auto;'><img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAARkAAA\
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'/></div>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEADER = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
