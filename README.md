# eAuth

The eAuth module is an implementation for handling user authentication on the ESP32. This module allows you to manage user login, logout, and session handling.

## Dependencies

This module depends on the following components:

- [eWeb](https://github.com/acevedoesteban999/eWeb)

## How tu Use

#### Init Handlers

uri_handlers.c

```c
#include "eWeb.h"

extern const char login_min_html_asm_start[] asm("_binary_login_min_css_start");
extern const char login_min_html_asm_end[] asm("_binary_login_min_css_end");

extern const char home_min_html_asm_start[] asm("_binary_home_min_css_start");
extern const char home_min_html_asm_end[] asm("_binary_home_min_css_end");

extern const char home_min_css_asm_start[] asm("_binary_home_min_css_start");
extern const char home_min_css_asm_end[] asm("_binary_home_min_css_end");

extern const char home_min_js_asm_start[] asm("_binary_home_min_js_start");
extern const char home_min_js_asm_end[] asm("_binary_home_min_js_end");

uri_ctx_hanlder STATIC_URIS[] = {
    /*eWeb exampleinit system static uris
    {{"/example.min.html", HTTP_GET, eweb_static_html_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/html"}},
    {{"/example.min.js", HTTP_GET, eweb_static_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/javascript"}},
    {{"/example.min.css", HTTP_GET, eweb_static_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/css"}},
    */

    {{"/home.min.html", HTTP_GET, eaut_static_min_html_handler, NULL}, true, {home_min_html_asm_start,home_min_html_asm_end,"text/html"}},
    {{"/home.min.css", HTTP_GET, eauth_static_handler, NULL}, true, {home_min_css_asm_start,home_min_css_asm_end,"text/css"}},
    {{"/home.min.js", HTTP_GET, eauth_static_handler, NULL}, true, {home_min_js_asm_start,home_js_css_asm_end,"text/javascript"}},

};

const int STATIC_URIS_LEN = sizeof(STATIC_URIS)/sizeof(uri_ctx_hanlder);

```

main.c

```c
#include "eAuth.h"
#include "uri_handlers.c"


void app_main() {
    eauth_init();
    eweb_preapare_uri_hanlders(STATIC_URIS,STATIC_URIS_LEN);
    eweb_init(STATIC_URIS_LEN);
    eweb_set_uri_hanlders(STATIC_URIS,STATIC_URIS_LEN);
    eauth_set_redirect_404("/home.min.html"); 
}
```

CMakeLists.txt

``` CMake
idf_component_register(
    SRCS 
        "main.c"
        "uri_handlers.c"
        
    INCLUDE_DIRS 
        "include"
        
    EMBED_FILES 
        "src/home.min.html" 
        "src/login.min.html"
        
        "src/css/home.min.css" 
        "src/js/home.min.js"
    
    REQUIRES  
        eAuth
)
```
## Example Login.min.html

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Login</title>
    <link rel="stylesheet" href="./css/login.min.css" />
    <link rel="icon" href="data:," />
  </head>
  <body>
    <div class="login-container">
      <form action="./login" method="POST" class="login-form">
        <div style="text-align: end;">
          <h6 style="margin: 0;">Login Page</h6>
        </div>
        <h2>Login Page</h2>
        <div class="input-group">
          <label for="username">Username</label>
          <input type="text" id="username" name="username" required />
        </div>
        <div class="input-group">
          <label for="password">Password</label>
          <input type="password" id="password" name="password" required />
        </div>
        <input type="hidden" name="uri" id="inputURI" />
        <button type="submit">Login</button>
      </form>
    </div>
  </body>
  <script>
    document.addEventListener("DOMContentLoaded", () => {
      inputURI = document.getElementById("inputURI").value =
        new URLSearchParams(window.location.search).get("uri");
    });
  </script>
</html>
```
