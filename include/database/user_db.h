#ifndef USER_DB_H
#define USER_DB_H

#include <crypt.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Configuración de conexión
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "manage_system"

#define BUFFER_SIZE 256
#define MAX_NAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_ROLE_LENGTH 100
#define MAX_QUERY_LENGTH 512
#define PORT_DB 3307

// 📦 Conexión a la base de datos
MYSQL* connectToDb(void);

// 📋 Menú de opciones para el usuario
void showMenuUser(void);

// 📥 Obtener la lista de usuarios desde la base de datos
int getUsers(void);

// ➕ Crear un nuevo usuario en la base de datos
int createUser(const char* name, const char* password, const char* email, const char* role);

// ❌ Eliminar un usuario por email
int deleteUser(const char* email);

// 🧭 Función principal para manejar el menú y las operaciones de usuario
void manageDbUser(void);

#endif // USER_DB_H
