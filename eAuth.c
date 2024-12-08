#include "eAuth.h"


User users[MAX_USERS];         // Declaración del array de usuarios
unsigned int count_users=0;

// Inicializa los usuarios con un usuario predeterminado (admin)
void init_auth() {
    //init_nvs();
    // Inicializar usuario admin
    strcpy(users[0].username, "admin");
    strcpy(users[0].password, "admin");
    users[0].is_authenticated = false; // Por defecto, no está autenticado
    users[0].session_token[0] = '\0';  // Sin token inicial
    count_users = 1;                   // Solo tenemos un usuario inicial
}

bool isAuth(httpd_req_t *req)
{
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    return (strlen(session_token) && get_user_by_session_token(session_token));
}

void save_user(){

}

// Genera un token de sesión aleatorio
void generate_session_token(char *token) {
    // Generar un token aleatorio simple
    snprintf(token, TOKEN_LEN, "%lu_%d", (unsigned long)time(NULL), rand());
}

// Autentica al usuario comparando el nombre de usuario y la contraseña
bool authenticate_user(const char *username, const char *password) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            generate_session_token(users[i].session_token); // Generar token
            users[i].is_authenticated = true; // Marcar como autenticado
            return true; // Autenticación exitosa
        }
    }
    return false; // Falló la autenticación
}

// Busca un usuario por su nombre de usuario
User* find_user_by_username(const char *username) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // Falló la búsqueda
}

// Obtiene un usuario por su token de sesión
User* get_user_by_session_token(const char *session_token) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].session_token, session_token) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // No se encontró el usuario
}

// Cierra sesión del usuario dado un token de sesión
bool logout_user(const char *session_token) {
    User *user = get_user_by_session_token(session_token);
    if (user) {
        user->is_authenticated = false; // Cerrar sesión
        user->session_token[0] = '\0';  // Limpiar el token
        return true; // Cierre de sesión exitoso
    }
    return false; // Falló el cierre de sesión
}



// Login(GET)
esp_err_t login_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, _login_asm_start, _login_asm_end - _login_asm_start);
    return ESP_OK;
}

// Login (POST)
esp_err_t login_post_handler(httpd_req_t *req) {
    char buff[BUFF_LEN];
    int ret = httpd_req_recv(req, buff, sizeof(buff) - 1);
    if (ret <= 0) {
        // Manejo de error
        return ESP_FAIL;
    }
    buff[ret] = '\0'; 

    // Extraer nombre de usuario y contraseña
    char *username = strtok(buff, "&");
    char *password = strtok(NULL, "&");
    
    username = username + strlen("username="); // Saltar "username="
    password = password + strlen("password="); // Saltar "password="
    
    if (authenticate_user(username, password)) {
        httpd_resp_set_hdr(req, "Set-Cookie", find_user_by_username(username)->session_token); // Establecer cookie
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/home");
        httpd_resp_send(req, NULL, 0);
    } else {
        httpd_resp_send(req, "Authentication Failed", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

// Logout (get)
esp_err_t logout_handler(httpd_req_t *req) {
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    if(logout_user(session_token)) {
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }
    return ESP_OK;
}

// Error 404
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", redirect_404);
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGW("", "Redirecting to: %s",redirect_404);
    return ESP_OK;
}

// Required char* to login start and end EMBED_FILE
void set_auth_uri_handlers(const char*__login_asm_start,const char*__login_asm_end,const char*__redirect_404){
    _login_asm_start = __login_asm_start;
    _login_asm_end = __login_asm_end;
    if (strlen(__redirect_404) < MAX_404_BUFFER_SIZE)
        strcpy(redirect_404,__redirect_404);
    else
        strcpy(redirect_404,"/login.html");
    httpd_uri_t uri;
    uri.uri = "/login.html";
    uri.method = HTTP_GET;
    uri.handler = login_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);
    
    uri.uri = "/login.html";
    uri.method = HTTP_POST;
    uri.handler = login_post_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);
    
    uri.uri = "/logout";
    uri.method = HTTP_GET;
    uri.handler = logout_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);

    
    httpd_register_err_handler(WebServer, HTTPD_404_NOT_FOUND, http_404_error_handler);
}
