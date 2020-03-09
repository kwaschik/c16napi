#include <node_api.h>
#include "Include\c16_pgx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

napi_value hello(napi_env env, napi_callback_info args) {
    //napi_value greeting;
    napi_status status;
    size_t argc = 1;
    napi_value argv[1];
    char argval[42];
    size_t bufsize;

    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;

    status = napi_get_value_string_utf8(env, argv[0], argval, 42, &bufsize);
    if (status != napi_ok) return NULL;
    
    if (strncmp(argval, "error", 5) == 0) {
        napi_throw_error(env, "WILLY", "Ein Willy-Fehler ist aufgetreten");
        return NULL; //TODO was gibt man in diesem Fall zurück?
    }

    return argv[0];

    //status = napi_create_string_utf8(env, "hello", NAPI_AUTO_LENGTH, &greeting);
    //if (status != napi_ok) return NULL;
    //return greeting;
}

// CONZEPT 16 C-API initialisieren
napi_value c16InitModule(napi_env env, napi_callback_info args) {
    napi_status status;
    
    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;

    vPHANDLE hModule;
    // Weitergabe des C16-Modul-Handles
    napi_value module;
    
    // TODO: MemoryLimit fix vorgeben?
    // CONZEPT 16 C-API initialisieren
    nErg = C16_InitPgif(0x00400000, &hModule);
    if (nErg != C16ERR_OK) {
        // C16-Rückgabewert in napi_value f. JavaScript umwandeln
        status = napi_create_int32(env, nErg, &erg);
        if (status != napi_ok) {
            napi_throw_error(env, "VALUE", "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
            return NULL;
        }
        return erg;
    }

    // initialisierten Module-Handle an JavaScript zurückgeben
    int64_t mod64 = (int64_t)hModule;
    status = napi_create_int64(env, mod64, &module);
    if (status != napi_ok) {
        // Modul schliessen, Speicher freigeben usw.
        C16_TermPgif(hModule);
        napi_throw_error(env, "MODHANDLE", "Fehler beim Umwandeln des CONZEPT 16-Module-Handles in einen napi_value!");
        return NULL;
    }
    
    return module;
}

// CONZEPT 16 C-API beenden (Speicher freigeben usw.)
napi_value c16TermModule(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[1];
    size_t argc = 1;
    
    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;

    vPHANDLE hModule;
    // Übernahme des C16-Modulhandles aus JavaScript
    int64_t module;

    // Funktionsargumente lesen, erwartet wird das Modul-Handle, also mind. ein int64-Wert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, "ARGS", "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, "ARGCOUNT", "Es muss mindestens ein Funktionsargument übergeben werden!");
        return NULL;
    }
    // übergebenen Wert als int64 interpretieren
    status = napi_get_value_int64(env, argv[0], &module);
    if (status == napi_ok) {
        // übergebenen int64-Wert als Modul-Handle interpretieren
        hModule = (vPHANDLE)module;
        // Modul schliessen, Speicher freigeben usw.
        nErg = C16_TermPgif(hModule);
    } else {
        napi_throw_error(env, "VALUE", "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) return NULL;

    return erg;
}

// CONZEPT 16 DB-Instanz initialisieren
napi_value c16InitInstance(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[1];
    size_t argc = 1;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;

    vPHANDLE hModule;
    // Übernahme des C16-Modulhandles aus JavaScript
    int64_t module;

    vPHANDLE hInstance;
    // Weitergabe des C16-Instanz-Handles
    napi_value instance;

    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;
    if (argc < 1) return NULL;

    status = napi_get_value_int64(env, argv[0], &module);
    if (status == napi_ok) {
        hModule = (vPHANDLE)module;

        nErg = C16_InitInstance(hModule, &hInstance);
        if (nErg != C16ERR_OK)
            return NULL;

        int64_t inst64 = (int64_t)hInstance;
        status = napi_create_int64(env, inst64, &instance);
        if (status != napi_ok) {
            nErg = C16_TermInstance(hInstance);
            return NULL;
        }

        return instance;
    }

    return NULL;
}
napi_value c16TermInstance(napi_env env, napi_callback_info args) {
    vERROR nErg;
    vPHANDLE hInstance;
    napi_value argv[1];
    napi_status status;
    size_t argc = 1;
    int64_t instance;
    napi_value erg;
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;
    if (argc < 1) return NULL;

    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        hInstance = (vPHANDLE)instance;
        nErg = C16_TermInstance(hInstance);
    } else
        return NULL;
    
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) return NULL;

    return erg;
}

napi_value c16OpenArea(napi_env env, napi_callback_info args) {
    vERROR nErg;
    vPHANDLE hInstance;
    napi_value argv[1];
    napi_status status;
    size_t argc = 1;
    int64_t instance;
    napi_value erg;
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;
    if (argc < 1) return NULL;

    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        hInstance = (vPHANDLE)instance;
        nErg = C16_OpenArea(hInstance, "TCP:*", NULL, "CodeLibrary", "DEV", NULL, NULL, NULL, 32);
    } else
        return NULL;
    
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) return NULL;

    return erg;
}

napi_value c16CloseArea(napi_env env, napi_callback_info args) {
    vERROR nErg;
    vPHANDLE hInstance;
    napi_value argv[1];
    napi_status status;
    size_t argc = 1;
    int64_t instance;
    napi_value erg;
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;
    if (argc < 1) return NULL;

    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        hInstance = (vPHANDLE)instance;
        nErg = C16_CloseArea(hInstance);
    } else
        return NULL;
    
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) return NULL;

    return erg;
}

napi_value c16LockProc(napi_env env, napi_callback_info args) {
    vERROR nErg;
    vPHANDLE hInstance;
    vPHANDLE hProc;
    napi_value argv[2];
    vC16_TextInfo txtInfo; txtInfo.InfoSize = sizeof(vC16_TextInfo);
    char procName[42];
    napi_status status;
    size_t argc = 2;
    int64_t instance;
    size_t bufsize;
    napi_value erg;

    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) return NULL;
    if (argc < 1) return NULL;

    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        hInstance = (vPHANDLE)instance;
        nErg = C16_ProcOpen(hInstance, &hProc);
        if (nErg == C16ERR_OK) {
            status = napi_get_value_string_utf8(env, argv[1], procName, 42, &bufsize);
            if (status != napi_ok) {

            }
            printf("%s\n", procName);
            nErg = C16_TextRead(hProc, procName, _TextLock, &txtInfo);
        }

    } else
        return NULL;
    
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) return NULL;

    return erg;

}

napi_value init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fn, c16InitMod, c16TermMod, c16InitInst, c16TermInst, c16OpenDB, c16CloseDB, c16LockPrc;

    status = napi_create_function(env, NULL, 0, hello, NULL, &fn);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "hello", fn);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16InitModule, NULL, &c16InitMod);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16InitModule", c16InitMod);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16TermModule, NULL, &c16TermMod);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16TermModule", c16TermMod);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16InitInstance, NULL, &c16InitInst);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16InitInstance", c16InitInst);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16TermInstance, NULL, &c16TermInst);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16TermInstance", c16TermInst);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16OpenArea, NULL, &c16OpenDB);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16OpenArea", c16OpenDB);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16CloseArea, NULL, &c16CloseDB);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16CloseArea", c16CloseDB);
    if (status != napi_ok) return NULL;

    status = napi_create_function(env, NULL, 0, c16LockProc, NULL, &c16LockPrc);
    if (status != napi_ok) return NULL;
    status = napi_set_named_property(env, exports, "c16LockProc", c16LockPrc);
    if (status != napi_ok) return NULL;

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
