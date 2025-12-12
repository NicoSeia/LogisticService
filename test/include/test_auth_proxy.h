/**
 * @file test_auth_proxy.h
 * @brief Definiciones para las pruebas unitarias de AuthProxy.
 *
 * Este archivo contiene la declaración de variables, funciones de configuración,
 * limpieza y prototipos de pruebas unitarias para el módulo de autenticación `AuthProxy`.
 *
 * @gcapdevila
 * @26/4/25
 */

#ifndef TEST_AUTH_PROXY_H
#define TEST_AUTH_PROXY_H

#include "auth_proxy.h"
#include "unity.h"
#include <stdbool.h>
#include <string.h>

/**
 * @brief Estructura global para pruebas de autenticación.
 */
extern AuthProxy test_proxy;

/**
 * @brief Configura el entorno de prueba antes de cada test.
 *
 * Reinicia el estado de `test_proxy` para asegurar independencia entre casos de prueba.
 */
void setUp(void);

/**
 * @brief Limpia el entorno de prueba después de cada test.
 *
 * Cierra sesión si quedó alguna sesión abierta en `test_proxy`.
 */
void tearDown(void);

/**
 * @brief Prueba la creación de un usuario en la base de datos.
 */
void test_create_user(void);

/**
 * @brief Prueba de autenticación exitosa con credenciales correctas.
 */
void test_authenticate_success(void);

/**
 * @brief Prueba de autenticación fallida debido a contraseña incorrecta.
 */
void test_authenticate_wrong_password(void);

/**
 * @brief Prueba de autenticación fallida debido a usuario no existente.
 */
void test_authenticate_user_not_found(void);

/**
 * @brief Prueba de cierre de sesión (`logout`) con un usuario autenticado.
 */
void test_logout_proxy_authenticated(void);

/**
 * @brief Prueba de cierre de sesión (`logout`) cuando no hay usuario autenticado.
 */
void test_logout_proxy_not_authenticated(void);

/**
 * @brief Prueba de gestión de usuario en la base de datos con autenticación exitosa.
 */
void test_manageDbUserWithAuth_success(void);

/**
 * @brief Prueba de gestión de usuario en la base de datos pasando un `proxy` nulo.
 */
void test_manageDbUserWithAuth_null_proxy(void);

/**
 * @brief Prueba de eliminación de un usuario de la base de datos.
 */
void test_delete_user(void);

#endif /* TEST_AUTH_PROXY_H */
