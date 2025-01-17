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

extern const char login_min_html_asm_start[] asm("_binary_login_min_css_start");
extern const char login_min_html_asm_end[] asm("_binary_login_min_css_end");

extern const char home_min_html_asm_start[] asm("_binary_home_min_css_start");
extern const char home_min_html_asm_end[] asm("_binary_home_min_css_end");

extern const char home_min_css_asm_start[] asm("_binary_home_min_css_start");
extern const char home_min_css_asm_end[] asm("_binary_home_min_css_end");

uri_ctx_hanlder static_uris[] = {
    /*eWeb init system static uris
    {{"/example.min.html", HTTP_GET, eweb_static_min_html_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/html"}},
    {{"/example.min.js", HTTP_GET, eweb_static_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/javascript"}},
    {{"/example.min.css", HTTP_GET, eweb_static_handler, NULL}, true, {example_min_html_asm_start,example_min_html_asm_end,"text/css"}},
    */

    {{"/home.min.html", HTTP_GET, eaut_static_min_html_handler, NULL}, true, {home_min_html_asm_start,home_min_html_asm_end,"text/html"}},
    {{"/home.min.css", HTTP_GET, eauth_static_handler, NULL}, true, {home_min_css_asm_start,home_min_css_asm_end,"text/css"}},

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
    eauth_set_redirect_404(login_min_html_asm_start,login_min_html_asm_end,"/home.min.html");
}
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
