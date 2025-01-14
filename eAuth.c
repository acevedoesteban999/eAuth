#include "eAuth.h"


User users[MAX_USERS];         // Declaración del array de usuarios
unsigned int count_users=0;
char redirect_404[MAX_404_BUFFER_SIZE];
// Inicializa los usuarios con un usuario predeterminado (admin)
void eauth_init() {
    //estore_init();
    // Inicializar usuario admin
    strcpy(users[0].username, "admin");
    strcpy(users[0].password, "admin");
    users[0].is_authenticated = false; // Por defecto, no está autenticado
    users[0].session_token[0] = '\0';  // Sin token inicial
    count_users = 1;                   // Solo tenemos un usuario inicial
}

bool eauth_isAuth(httpd_req_t *req)
{
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    return (strlen(session_token) && eauth_get_user_by_session_token(session_token));
}

void save_user(){

}

// Genera un token de sesión aleatorio
void eauth_generate_session_token(char *token) {
    // Generar un token aleatorio simple
    snprintf(token, TOKEN_LEN, "%lu_%d", (unsigned long)time(NULL), rand());
}

// Autentica al usuario comparando el nombre de usuario y la contraseña
bool eauth_authenticate_user(const char *username, const char *password) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            eauth_generate_session_token(users[i].session_token); // Generar token
            users[i].is_authenticated = true; // Marcar como autenticado
            return true; // Autenticación exitosa
        }
    }
    return false; // Falló la autenticación
}

// Busca un usuario por su nombre de usuario
User* eauth_find_user_by_username(const char *username) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // Falló la búsqueda
}

// Obtiene un usuario por su token de sesión
User* eauth_get_user_by_session_token(const char *session_token) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].session_token, session_token) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // No se encontró el usuario
}

// Cierra sesión del usuario dado un token de sesión
bool eauth_logout_user(const char *session_token) {
    User *user = eauth_get_user_by_session_token(session_token);
    if (user) {
        user->is_authenticated = false;
        user->session_token[0] = '\0';  
        return true;
    }
    return false;
}

// Login(GET)
esp_err_t eauth_login_handler(httpd_req_t *req)
{
    if (!eauth_isAuth(req))
        return eweb_static_html_handler(req);
    else{
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", redirect_404);
        httpd_resp_send(req, NULL, 0);
    }
    return ESP_OK;
}

void eauth_replace_percent_2F(char *str) {
    char *pos = str;
    char *tmp;
    while ((pos = strstr(pos, "%2F")) != NULL) {
        tmp = pos;
        
        memmove(tmp + 1, tmp + 3, strlen(tmp + 3) + 1);
        *tmp = '/';
        
        pos = tmp + 1;
    }
}

// Login (POST)
esp_err_t eauth_login_post_handler(httpd_req_t *req) {
    char uri_redirect[MAX_STRING_REQUEST_LEN + 1];
    char *buff = malloc(req->content_len + 1);
    if (buff == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Request Body too long");
        return ESP_FAIL;
    }
    eweb_get_all_data_request(req,buff);
    eauth_replace_percent_2F(buff);
    bool uri_redirect_bool = eweb_get_string_urlencoded(buff,"uri",uri_redirect,MAX_STRING_REQUEST_LEN);

    if(! eauth_isAuth(req)){
        char username[MAX_STRING_REQUEST_LEN + 1];
        char password[MAX_STRING_REQUEST_LEN + 1];
        
        eweb_get_string_urlencoded(buff,"username",username,MAX_STRING_REQUEST_LEN);
        eweb_get_string_urlencoded(buff,"password",password,MAX_STRING_REQUEST_LEN);
        
        if (eauth_authenticate_user(username, password)) {
            httpd_resp_set_hdr(req, "Set-Cookie", eauth_find_user_by_username(username)->session_token); // Establecer cookie
            httpd_resp_set_status(req, "302 Found");
            if(uri_redirect_bool)
                httpd_resp_set_hdr(req, "Location", uri_redirect);
            else
                httpd_resp_set_hdr(req, "Location", redirect_404);
            httpd_resp_send(req, NULL, 0);
        } 
        else 
            httpd_resp_send(req, "Authentication Failed", HTTPD_RESP_USE_STRLEN);
    }
    else{
        httpd_resp_set_status(req, "302 Found");
        if(uri_redirect_bool)
            httpd_resp_set_hdr(req, "Location", uri_redirect);
        else
            httpd_resp_set_hdr(req, "Location", redirect_404);

        httpd_resp_send(req, NULL, 0);
    }
    free(buff);
    return ESP_OK;
}

// Logout (get)
esp_err_t eauth_logout_handler(httpd_req_t *req) {
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    if(eauth_logout_user(session_token)) {
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }
    return ESP_OK;
}

// STATIC HTML(GET)
esp_err_t eauth_static_html_handler(httpd_req_t *req) {
    if (eauth_isAuth(req))
        return eweb_static_html_handler(req);

    eauth_redirect_to_login(req);
    return ESP_OK;
    
}

// Static  (GET)
esp_err_t eauth_static_handler(httpd_req_t *req) {
    if (eauth_isAuth(req))
        return eweb_static_handler(req);
    
    httpd_resp_set_status(req, "401 Unauthorized");
    httpd_resp_set_hdr(req, "Content-Type", "text");
    httpd_resp_send(req, "Unauthorized", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
    
}

// Error 404
esp_err_t eauth_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if(req->method == HTTP_GET){
        httpd_resp_set_status(req, "302 Temporary Redirect");
        httpd_resp_set_hdr(req, "Location", redirect_404);
        httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
    return ESP_FAIL;
}

void eauth_redirect_to_login(httpd_req_t*req){
    if(req->method == HTTP_GET){
        char redirect[strlen("/login.html") + strlen("?uri=") + strlen(req->uri) + 1];
        httpd_resp_set_status(req, "302 Temporary Redirect");
        strcpy(redirect,"/login.html?uri=");
        strcat(redirect,req->uri);
        httpd_resp_set_hdr(req, "Location", redirect);
        httpd_resp_send(req, NULL, 0);              
    }
    else{
        httpd_resp_set_status(req, "402 Not Found");
        httpd_resp_send(req, NULL, 0);
    }
}

void eauth_set_redirect_404(const char*__redirect_404){
    if (strlen(__redirect_404) < MAX_404_BUFFER_SIZE)
        strcpy(redirect_404,__redirect_404);
    else
        strcpy(redirect_404,"/login.html");

    httpd_register_err_handler(WebServer, HTTPD_404_NOT_FOUND, eauth_http_404_error_handler);
}

esp_err_t eauth_condicional_function(httpd_req_t *req){
    if (eauth_isAuth(req)){
        return eweb_call_condicional_function(req);
    }
    else{
        httpd_resp_set_status(req, "401 Unauthorized");
        httpd_resp_set_hdr(req, "Content-Type", "text");
        httpd_resp_send(req, "Unauthorized", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}