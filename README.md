# eAuth

The eAuth module is an implementation for handling user authentication on the ESP32. This module allows you to manage user login, logout, and session handling.

## Features

- Initialize user authentication.
- Authenticate users.
- Generate session tokens.
- Handle login and logout requests.
- Protect static resources with authentication.

## Dependencies

This module depends on the following components:
- [eWeb](../eWeb/README.md)
- [eStore](../eStore/README.md)

## Main Functions

- `void eauth_init()`: Initializes the user authentication system.
- `bool eauth_isAuth(httpd_req_t *req)`: Checks if a user is authenticated.
- `void eauth_generate_session_token(char *token)`: Generates a random session token.
- `bool eauth_authenticate_user(const char *username, const char *password)`: Authenticates a user with a username and password.
- `User* eauth_find_user_by_username(const char *username)`: Finds a user by their username.
- `User* eauth_get_user_by_session_token(const char *session_token)`: Gets a user by their session token.
- `bool eauth_logout_user(const char *session_token)`: Logs out a user by their session token.
- `esp_err_t eauth_login_handler(httpd_req_t *req)`: Handles login GET requests.
- `esp_err_t eauth_login_post_handler(httpd_req_t *req)`: Handles login POST requests.
- `esp_err_t eauth_logout_handler(httpd_req_t *req)`: Handles logout requests.
- `esp_err_t eauth_static_html_auth_handler(httpd_req_t *req)`: Handles authenticated static HTML requests.
- `esp_err_t eauth_static_auth_handler(httpd_req_t *req)`: Handles authenticated static file requests.
- `esp_err_t eauth_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)`: Handles 404 errors.
- `void eauth_redirect_to_login(httpd_req_t *req)`: Redirects to the login page if not authenticated.
- `void eauth_set_auth_uri(const char *__login_asm_start, const char *__login_asm_end, const char *__redirect_404)`: Sets the URI handlers for authentication.

## URI Handling

### Login URI

The login URI handles user login requests. It supports both GET and POST methods.

- **GET**: Displays the login page.
- **POST**: Processes the login form submission.

Example:

```c
httpd_uri_t login_uri = {
    .uri = "/login.html",
    .method = HTTP_GET,
    .handler = eauth_login_handler,
    .user_ctx = NULL
};
httpd_register_uri_handler(WebServer, &login_uri);

login_uri.method = HTTP_POST;
login_uri.handler = eauth_login_post_handler;
httpd_register_uri_handler(WebServer, &login_uri);
```

### Custom URI (Home)

You can define custom URIs for your application. For example, a home page that requires authentication.

Example:

```c
static const char home_html_start[] asm("_binary_home_html_start");
static const char home_html_end[] asm("_binary_home_html_end");

static_ctx_handler home_ctx = {
    .asm_start = home_html_start,
    .asm_end = home_html_end,
    .resp_type = "text/html"
};

httpd_uri_t home_uri = {
    .uri = "/home.html",
    .method = HTTP_GET,
    .handler = eauth_static_html_auth_handler,
    .user_ctx = &home_ctx
};
httpd_register_uri_handler(WebServer, &home_uri);
```

### Using URIs with Authentication

To protect URIs with authentication, use the `eauth_static_html_auth_handler` or `eauth_static_auth_handler` as the handler. These handlers check if the user is authenticated before serving the content.

Example:

```c
httpd_uri_t protected_uri = {
    .uri = "/protected.html",
    .method = HTTP_GET,
    .handler = eauth_static_html_auth_handler,
    .user_ctx = &protected_ctx
};
httpd_register_uri_handler(WebServer, &protected_uri);
```

### Redirect to Login

If a user is not authenticated, they can be redirected to the login page using the `eauth_redirect_to_login` function.

Example:

```c
void eauth_redirect_to_login(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        char redirect[strlen("/login.html") + strlen("?uri=") + strlen(req->uri) + 1];
        httpd_resp_set_status(req, "302 Temporary Redirect");
        strcpy(redirect, "/login.html?uri=");
        strcat(redirect, req->uri);
        httpd_resp_set_hdr(req, "Location", redirect);
        httpd_resp_send(req, NULL, 0);
    } else {
        httpd_resp_set_status(req, "402 Not Found");
        httpd_resp_send(req, NULL, 0);
    }
}
```

### Handling Static Files with Authentication

To serve static files that require authentication, use the `eauth_static_auth_handler`.

Example:

```c
static_ctx_handler static_ctx = {
    .asm_start = static_file_start,
    .asm_end = static_file_end,
    .resp_type = "application/octet-stream"
};

httpd_uri_t static_uri = {
    .uri = "/static/file.bin",
    .method = HTTP_GET,
    .handler = eauth_static_auth_handler,
    .user_ctx = &static_ctx
};
httpd_register_uri_handler(WebServer, &static_uri);
```

## Example Usage

```c
#include "eAuth.h"

void app_main() {
    // Initialize user authentication
    eauth_init();

    // Set authentication URI handlers
    eauth_set_auth_uri(_binary_login_html_start, _binary_login_html_end, "/login.html");

    // Register custom URIs
    httpd_uri_t home_uri = {
        .uri = "/home.html",
        .method = HTTP_GET,
        .handler = eauth_static_html_auth_handler,
        .user_ctx = &home_ctx
    };
    httpd_register_uri_handler(WebServer, &home_uri);
}
```
