#pragma once
#include <stdbool.h>
#include "esp_err.h"
#include "string.h"
#include <time.h>
#include "eStore.h"
#include "esp_http_server.h"
#include "eWeb.h"

#define MAX_USERS 10            // Máximo número de usuarios permitidos
#define USERNAME_LEN 20         // Longitud máxima para nombres de usuario
#define PASSWORD_LEN 20         // Longitud máxima para contraseñas
#define TOKEN_LEN 64            // Longitud máxima para tokens de sesión
#define MAX_404_BUFFER_SIZE 20  
#define MAX_STRING_REQUEST_LEN 20

#define EAUTH_HANDLERS(login_min_html_asm_start,login_min_html_asm_end) \
    {{"/login.min.html", HTTP_GET , eauth_login_handler , NULL}, true, {login_min_html_asm_start,login_min_html_asm_end,"",NULL,NULL}}, \
    {{"/logout", HTTP_GET , eauth_logout_handler , NULL}, false, {}}, \
    {{"/login", HTTP_POST , eauth_login_post_handler , NULL}, false, {}} \

// eSTRuctura para almacenar información de un usuario
typedef struct {
    char username[USERNAME_LEN];      // Nombre de usuario
    char password[PASSWORD_LEN];      // Contraseña
    char session_token[TOKEN_LEN];    // Token de sesión
    bool is_authenticated;             // Estado de autenticación
} User;

extern User users[MAX_USERS];         // Declaración del array de usuarios
extern unsigned int count_users;      // Declaración del contador de usuarios




// Inicializa los usuarios
void eauth_init(void);

// Genera un token de sesión aleatorio
void eauth_generate_session_token(char *token);

bool eauth_isAuth(httpd_req_t *req);

// Autentica al usuario
bool eauth_authenticate_user(const char *username, const char *password);

// Busca un usuario por su nombre de usuario
User* eauth_find_user_by_username(const char *username);

// Obtiene un usuario por su token de sesión
User* eauth_get_user_by_session_token(const char *session_token);

// Cierra sesión del usuario
bool eauth_logout_user(const char *session_token);

esp_err_t eauth_login_handler(httpd_req_t *req);

esp_err_t eauth_login_post_handler(httpd_req_t *req);

esp_err_t eauth_logout_handler(httpd_req_t *req);

void eauth_redirect_to_login(httpd_req_t*req);

void eauth_set_redirect_404(const char *__redirect_404);

bool eauth_conditional_function(httpd_req_t *req);

esp_err_t eauth_excecution_function(httpd_req_t *req);

esp_err_t eauth_static_html_handler(httpd_req_t *req);

esp_err_t eauth_static_handler(httpd_req_t *req);