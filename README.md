# eAuth

The eAuth module is an implementation for handling user authentication on the ESP32. This module allows you to manage user login, logout, and session handling.

## Dependencies

This module depends on the following components:
- [eWeb](https://github.com/acevedoesteban999/eWeb)

## How tu Use

TODO
#### Init
eAuth depends eWeb , first inicialice like [eWeb init system](https://github.com/acevedoesteban999/eWeb)  

uri_handlers.c
```c
#include "eWeb.h"

extern const char login_html_asm_start[] asm("_binary_login_css_start");
extern const char login_html_asm_end[] asm("_binary_login_css_end");

extern const char home_html_asm_start[] asm("_binary_home_css_start");
extern const char home_html_asm_end[] asm("_binary_home_css_end");

extern const char home_css_asm_start[] asm("_binary_home_css_start");
extern const char home_css_asm_end[] asm("_binary_home_css_end");

uri_ctx_hanlder static_uris[] = {
    /*eWeb init system static uris
    {{"/example.html", HTTP_GET, eweb_static_html_handler, NULL}, true, {example_html_asm_start,example_html_asm_end,"text/html"}},
    {{"/example.js", HTTP_GET, eweb_static_handler, NULL}, true, {example_html_asm_start,example_html_asm_end,"text/javascript"}},
    {{"/example.css", HTTP_GET, eweb_static_handler, NULL}, true, {example_html_asm_start,example_html_asm_end,"text/css"}},
    */
    
    {{"/home.html", HTTP_GET, eaut_static_html_handler, NULL}, true, {home_html_asm_start,home_html_asm_end,"text/html"}},
    {{"/home.css", HTTP_GET, eauth_static_handler, NULL}, true, {home_css_asm_start,home_css_asm_end,"text/css"}},
    
};

```

```c
#include "eAuth.h"
#include "uri_handlers.c"


void app_main() {
    //eWeb init system
        //uri_ctx_hanlder *uris = static_uris;
        //size_t uri_size = get_uri_handlers();
        eweb_init(uri_size + 1);                        // Necesary Inc 1 for login uri
        //eweb_set_uri_hanlders(uris,uri_size);
    

    eauth_init();
    eauth_set_redirect_404(login_html_asm_start,login_html_asm_end,"/home.html");
}
```
