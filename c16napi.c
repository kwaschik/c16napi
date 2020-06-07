#include <node_api.h>
#include "include\c16_pgx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "c16napi.h"

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
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
            return NULL;
        }
        return erg;
    }

    // initialisierten Module-Handle an JavaScript zurückgeben
    int64_t moduleHandle = (int64_t)hModule;
    status = napi_create_int64(env, moduleHandle, &module);
    if (status != napi_ok) {
        // Modul schliessen, Speicher freigeben usw.
        C16_TermPgif(hModule);
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Module-Handles in einen napi_value!");
        return NULL;
    }
    
    return module;
}

// Informationen über gestartete CONZEPT 16 C-API abrufen
napi_value c16GetModuleInfo(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[1];
    size_t argc = 1;
    
    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    //napi_value erg;

    vPHANDLE hModule;
    // Übernahme des C16-Modulhandles aus JavaScript
    int64_t module;

    // Funktionsargumente lesen, erwartet wird das Modul-Handle, also mind. ein int64-Wert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
        return NULL;
    }

    napi_value keyModuleType;
    napi_value moduleType;
    napi_value keyModuleLicense;
    napi_value moduleLicense;
    napi_value keyModuleRelease;
    napi_value moduleRelease;
    napi_value keyModuleUserLimit;
    napi_value moduleUserLimit;
    napi_value keyModuleMemory;
    napi_value moduleMemory;
    napi_value keyModuleMemoryPeak;
    napi_value moduleMemoryPeak;
    napi_value moduleInfo;

    // die Keys können schon vorbereitet werden
    if (   napi_create_string_utf8(env, "ModuleType", NAPI_AUTO_LENGTH, &keyModuleType) != napi_ok 
        || napi_create_string_utf8(env, "ModuleLicense", NAPI_AUTO_LENGTH, &keyModuleLicense) != napi_ok
        || napi_create_string_utf8(env, "ModuleRelease", NAPI_AUTO_LENGTH, &keyModuleRelease) != napi_ok
        || napi_create_string_utf8(env, "ModuleUserLimit", NAPI_AUTO_LENGTH, &keyModuleUserLimit) != napi_ok
        || napi_create_string_utf8(env, "ModuleMemory", NAPI_AUTO_LENGTH, &keyModuleMemory) != napi_ok
        || napi_create_string_utf8(env, "ModuleMemoryPeak", NAPI_AUTO_LENGTH, &keyModuleMemoryPeak) != napi_ok) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Erstellen der ModuleInfos!");
            return NULL;
        }

    // übergebenen Wert als int64 interpretieren
    status = napi_get_value_int64(env, argv[0], &module);
    if (status == napi_ok) {
        // übergebenen int64-Wert als Modul-Handle interpretieren
        hModule = (vPHANDLE)module;
        // Infos zu Modul abrufen
        vC16_PgifInfo c16moduleInfo;
        c16moduleInfo.InfoSize = sizeof(c16moduleInfo);
        nErg = C16_QueryPgifInfo(hModule, &c16moduleInfo);
        if (nErg == C16ERR_OK) {
            if (   napi_create_object(env, &moduleInfo) != napi_ok
                || napi_create_uint32(env, c16moduleInfo.PgifType, &moduleType) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleType, moduleType) != napi_ok
                || napi_create_string_latin1(env, c16moduleInfo.PgifLicense, NAPI_AUTO_LENGTH, &moduleLicense) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleLicense, moduleLicense) != napi_ok
                || napi_create_string_latin1(env, c16moduleInfo.PgifRelease, NAPI_AUTO_LENGTH, &moduleRelease) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleRelease, moduleRelease) != napi_ok
                || napi_create_uint32(env, c16moduleInfo.PgifUserLimit, &moduleUserLimit) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleUserLimit, moduleUserLimit) != napi_ok
                || napi_create_uint32(env, c16moduleInfo.PgifMemory, &moduleMemory) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleMemory, moduleMemory) != napi_ok
                || napi_create_uint32(env, c16moduleInfo.PgifMemoryPeak, &moduleMemoryPeak) != napi_ok
                || napi_set_property(env, moduleInfo, keyModuleMemoryPeak, moduleMemoryPeak) != napi_ok
                ) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Erstellen der ModuleInfos!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die ModulInfos!");
            return NULL;
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }

    return moduleInfo;
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
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
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
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }

    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// CONZEPT 16 Datenbank-Instanz initialisieren
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

    // Funktionsargumente lesen, erwartet wird das Modul-Handle, also mind. ein int64-Wert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
        return NULL;
    }

    // übergebenen Wert als int64 interpretieren
    status = napi_get_value_int64(env, argv[0], &module);
    if (status != napi_ok) {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }

    // übergebenen int64-Wert als Modul-Handle interpretieren
    hModule = (vPHANDLE)module;
    // C16-DB-Instanz initialisieren
    nErg = C16_InitInstance(hModule, &hInstance);
    if (nErg != C16ERR_OK) {
        // C16-Rückgabewert in napi_value f. JavaScript umwandeln
        status = napi_create_int32(env, nErg, &erg);
        if (status != napi_ok) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
            return NULL;
        }
        return erg;
    }

    int64_t instanceHandle = (int64_t)hInstance;
    status = napi_create_int64(env, instanceHandle, &instance);
    if (status != napi_ok) {
        C16_TermInstance(hInstance);
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Instanz-Handles in einen napi_value!");
        return NULL;
    }

    return instance;
}

// CONZEPT 16 Datenbank-Instanz schliessen
napi_value c16TermInstance(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[1];
    size_t argc = 1;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;
    
    // Funktionsargumente lesen, erwartet wird das DB-Instanz-Handle, also mind. ein int64-Wert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
        return NULL;
    }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        // DB-Instanz schliessen 
        nErg = C16_TermInstance(hInstance);
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// CONZEPT 16 Datenbank öffnen
napi_value c16OpenArea(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[6];
    size_t argc = 6;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;
    // Angaben zum Öffnen einer CONZEPT 16-DB
    size_t bufsize;
    
    char *dbArgs[5] = {
        NULL, // serverName
        NULL, // serverPassword
        NULL, // dbName
        NULL, // userName
        NULL  // userPassword
    };
    

    // Funktionsargumente lesen, erwartet wird: 
    // - das DB-Instanz-Handle (int64)
    // - Servername
    // - Serverpasswort
    // - DB-Name
    // - DB-Benutzer
    // - DB-Benutzer-Passwort
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 6) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens sechs Funktionsargument übergeben werden!");
        return NULL;
    }

    // ersten übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des ersten übergebenen Werts in einen int64!");
        return NULL;
    }

    int i;
    for (i = 0; i < 5; i++) {
        // TODO: utf8/latin1 und C16 ist ein Problem
        status = napi_get_value_string_latin1(env, argv[i+1], NULL, 0, &bufsize);
        if (status == napi_ok) {
            if (bufsize > 0) {
                dbArgs[i] = calloc(bufsize+1, sizeof(char));
                if (dbArgs[i]) {
                    status = napi_get_value_string_latin1(env, argv[i+1], dbArgs[i], bufsize+1, &bufsize);
                    if (status != napi_ok) 
                        break;
                } else 
                    break;
            }
        } else
            break;  
    }
    if (i < 5) {
        bool errMemory = !dbArgs[i];
        for (i = 0; i < 5; i++) {
            if (dbArgs[i]) free(dbArgs[i]);
        }
        if (status != napi_ok) {
            napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Lesen eines übergebenen Strings (Arg. 2-6)!");
            return NULL;
        }
        else if (errMemory && bufsize > 0) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Reservieren von Speicher!");
            return NULL;
        }
    }

    //printf("%s, %s, %s, %s, %s\n", dbArgs[0], dbArgs[1], dbArgs[2], dbArgs[3], dbArgs[4]);

    nErg = C16_OpenArea(hInstance, dbArgs[0], dbArgs[1], dbArgs[2], dbArgs[3], dbArgs[4], NULL, NULL, 32);
    // Speicher wieder freigeben
    for (i = 0; i < 5; i++) {
        if (dbArgs[i]) free(dbArgs[i]);
    }

    // TODO bringt offenbar nichts in Bezug auf die Procs/Texte
    /* if (nErg = C16ERR_OK) {
        nErg = C16_SetCharDefs(hInstance, '.', ',' , '.', ':', '.', 'Y', 'N', _FldExc_CharSet_ANSI);
    } */

    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// CONZEPT 16 Datenbank schliessen
napi_value c16CloseArea(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[1];
    size_t argc = 1;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;
    
    // Funktionsargumente lesen, erwartet wird das DB-Instanz-Handle, also mind. ein int64-Wert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 1) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
        return NULL;
    }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        // DB schliessen 
        nErg = C16_CloseArea(hInstance);
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// Hilfsfunktion, um CONZEPT 16 Date/Time-Werte in Unixzeit (in millis) umzuwandeln
double _dateTimeToUnixTimeStamp(vDATE date, vTIME time) {
    struct tm _time;
    
    _time.tm_sec   = time.s;
    _time.tm_min   = time.m;
    _time.tm_hour  = time.h;
    _time.tm_mday  = date.d;
    _time.tm_mon   = date.m-1;
    _time.tm_year  = date.y;
    _time.tm_isdst = -1;
    return (double) (mktime(&_time) * 1000);
}

// Metadaten aller Prozeduren einer CONZEPT 16 Datenbank lesen
// TODO options (user data) nutzen, für den Fall, nur eine?
// TODO zweite temp. Instanz öffnen, um Sperrstatus und sperrenden Benutzer zu ermitteln
//      dazu müssten für die übergebene Instanz die Verbindungsinfos gespeichert sein,
//      damit man mit diesen Infos eine weitere Instanz und DB-Verbindung öffnen kann,
//      gibt es eine andere Möglichkeit??
napi_value c16GetProcsMetadata(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[2];
    size_t argc = 2;

    vERROR nErg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;

    vCHAR procedureName[100];

    // Text-Handle
    vPHANDLE hProc;
    
    // Optionen, die steuern, welche Funktion in welcher Weise am Ende aufgerufen wird
    struct c16napi_options *options;
    
    // Funktionsargumente lesen, erwartet wird:
    // - das DB-Instanz-Handle
    // - Prozedurname
    // - welche Funktion in welchem Modus ausgeführt wird, wird über die options (user data) gesteuert

    // Funktionsargumente lesen, erwartet wird das DB-Instanz-Handle, also mind. ein int64-Wert, | mz: und Procname
    status = napi_get_cb_info(env, args, &argc, argv, NULL, &options);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (!options) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsoptionen!");
        return NULL;
    }
    
    switch (options->func) {
        case c16napi_fun_getprocmetadata:
        {
            if (argc < 2) {
                napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens zwei Funktionsargumente übergeben werden!");
                return NULL;
            }
            break;
        } 
        case c16napi_fun_getprocsmetadata:
        {
            if (argc < 1) {
                napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es muss mindestens ein Funktionsargument übergeben werden!");
                return NULL;
            }
            break;
        }
        default: 
        {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsoptionen!");
            return NULL;
        }
    }

    // Elemente, um die Metadata-Infos zu den Prozeduren zusammenzustellen
    // in dieser Form: [{Name: "Name", Size: size, Creator: "Creator", Modifier: "Modifier", Ctime: time, Mtime: time}, ...]
    napi_value keyProcName;
    napi_value procName;
    napi_value keyProcSize;
    napi_value procSize;
    napi_value keyProcCreator;
    napi_value procCreator;
    napi_value keyProcModifier;
    napi_value procModifier;
    napi_value keyProcCTime;
    napi_value procCTime;
    napi_value keyProcMTime;
    napi_value procMTime;
    napi_value proc = NULL;
    napi_value procs;

    // die Keys können schon vorbereitet werden
    if (   napi_create_string_utf8(env, "Name", NAPI_AUTO_LENGTH, &keyProcName) != napi_ok 
        || napi_create_string_utf8(env, "Size", NAPI_AUTO_LENGTH, &keyProcSize) != napi_ok
        || napi_create_string_utf8(env, "Creator", NAPI_AUTO_LENGTH, &keyProcCreator) != napi_ok
        || napi_create_string_utf8(env, "Modifier", NAPI_AUTO_LENGTH, &keyProcModifier) != napi_ok
        || napi_create_string_utf8(env, "Ctime", NAPI_AUTO_LENGTH, &keyProcCTime) != napi_ok
        || napi_create_string_utf8(env, "Mtime", NAPI_AUTO_LENGTH, &keyProcMTime) != napi_ok
        || napi_create_array(env, &procs) != napi_ok) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Ermitteln der Metadaten!");
            return NULL;
        }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;

        if (options->func == c16napi_fun_getprocmetadata) {
            status = napi_get_value_string_latin1(env, argv[1], procedureName, 100, NULL);
            if (status != napi_ok) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des Prozedurnamens!");
                return NULL;
            }
        }

        nErg = C16_ProcOpen(hInstance, &hProc);
        if (nErg == C16ERR_OK) {
            vC16_TextInfo procInfo; 
            procInfo.InfoSize = sizeof(vC16_TextInfo);
            uint32_t i = 0;

            if (options->func == c16napi_fun_getprocmetadata) {
                nErg = C16_TextRead(hProc, procedureName, 0, &procInfo);

            } else {
                nErg = C16_TextRead(hProc, NULL, _TextFirst, &procInfo);

            }

            // auch bei Berechtigungsproblemen erfährt man den Textnamen 
            while (nErg == C16ERR_OK || nErg == C16ERR_TEXT_RIGHTS) {
                // TODO, C16 macht nicht utf8 und auch nicht wirklich latin1 (c16-Kodierung), 
                // aber latin1 geht in beide Richtungen
                if (   napi_create_object(env, &proc) != napi_ok
                    || napi_create_string_latin1(env, procInfo.TextName, NAPI_AUTO_LENGTH, &procName) != napi_ok
                    || napi_set_property(env, proc, keyProcName, procName) != napi_ok
                    || napi_create_uint32(env, procInfo.TextSize, &procSize) != napi_ok
                    || napi_set_property(env, proc, keyProcSize, procSize) != napi_ok
                    || napi_create_string_latin1(env, procInfo.TextCreateUser, NAPI_AUTO_LENGTH, &procCreator) != napi_ok
                    || napi_set_property(env, proc, keyProcCreator, procCreator) != napi_ok
                    || napi_create_string_latin1(env, procInfo.TextModifiedUser, NAPI_AUTO_LENGTH, &procModifier) != napi_ok
                    || napi_set_property(env, proc, keyProcModifier, procModifier) != napi_ok
                    || napi_create_double(env, _dateTimeToUnixTimeStamp(procInfo.TextCreateDate, procInfo.TextCreateTime), &procCTime) != napi_ok
                    || napi_set_property(env, proc, keyProcCTime, procCTime) != napi_ok
                    || napi_create_double(env, _dateTimeToUnixTimeStamp(procInfo.TextModifiedDate, procInfo.TextModifiedTime), &procMTime) != napi_ok
                    || napi_set_property(env, proc, keyProcMTime, procMTime) != napi_ok
                    || napi_set_element(env, procs, i, proc) != napi_ok) {
                        C16_TextClose(hProc);
                        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Ermitteln der Metadaten!");
                        return NULL;
                    }
                if (options->func == c16napi_fun_getprocmetadata) {
                    break;
                }  
                nErg = C16_TextRead(hProc, NULL, _TextNext, &procInfo);
                i++;
            }
            if (C16_TextClose(hProc) != C16ERR_OK) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf Prozeduren!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf Prozeduren!");
            return NULL;
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    if (options->func == c16napi_fun_getprocmetadata) {
        napi_valuetype result;
        status = napi_typeof(env, proc, &result);
        //todo: Diese Überrpüfung muss unter Umtänden angepasst werden, wenn proc nicht mehr typeof object sein könnte!!!
        if (status == napi_ok && result == napi_object) {
            
            return  proc;
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Prozedur ist nicht vorhanden!");
            return NULL;
        }
    }
    return procs;
}

// Einfache Funktionen in Bezug auf eine CONZEPT 16-Prozedur ausführen:
// - (Ent)Sperren einer Prozedur
// - Anlegen einer Prozedur
// - Löschen einer Prozedur
// - Kopieren einer Prozedur
// - Umbenennen einer Prozedur
napi_value c16SimpleProcFuns(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[3];
    size_t argc = 3;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;

    // Text-Handle
    vPHANDLE hProc;
    // Puffer f. Prozedurname(n)
    vCHAR procName[100];
    // copy, rename
    vCHAR procNameNew[100];

    // Optionen, die steuern, welche Funktion in welcher Weise am Ende aufgerufen wird
    struct c16napi_options *options;
    
    // Funktionsargumente lesen, erwartet wird:
    // - das DB-Instanz-Handle
    // - Prozedurname
    // - welche Funktion in welchem Modus ausgeführt wird, wird über die options (user data) gesteuert
    status = napi_get_cb_info(env, args, &argc, argv, NULL, &options);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (!options) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsoptionen!");
        return NULL;
    }
    switch (options->func) {
        case c16napi_fun_textread:
        case c16napi_fun_textcreate:
        case c16napi_fun_textdelete: {
            if (argc < 2) {
                napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens zwei Funktionsargumente übergeben werden!");
                return NULL;
            }
            break;
        }
        case c16napi_fun_textcopy:
        case c16napi_fun_textrename: {
            if (argc < 3) {
                napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens drei Funktionsargumente übergeben werden!");
                return NULL;
            }
            break;
        }
        default: {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsoptionen!");
            return NULL;
        }
    }
    

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        nErg = C16_ProcOpen(hInstance, &hProc);
        if (nErg == C16ERR_OK) {
            // utf8 geht in beide Richtungen nicht richtig,
            // latin1 gibt zwar auch Kodierungsprobleme, aber geht trotzdem in beide Richtungen,
            // d.h. reines Darstellungsproblem
            // vectorsoft: Prozedurnamen können derzeit nur in C16-Kodierung gelesen werden :-(
            status = napi_get_value_string_latin1(env, argv[1], procName, 100, NULL);
            if (status == napi_ok) {
                vC16_TextInfo procInfo;
                procInfo.InfoSize = sizeof(vC16_TextInfo);

                if (options->func == c16napi_fun_textread) {
                    nErg = C16_TextRead(hProc, procName, options->funcopts, &procInfo);
                    /* if (options->funcopts == _TextLock && nErg != C16ERR_OK) {
                        vINT UserId = 0;
                        nErg = C16_RecInfo(hInstance, 0, _RecLockedBy, 0, &UserId);
                        if (nErg == C16ERR_OK) nErg = UserId;
                    } */
                }
                else if (options->func == c16napi_fun_textcreate)
                    nErg = C16_TextCreate(hProc, procName, false);
                else if (options->func == c16napi_fun_textdelete)
                    nErg = C16_TextDelete(hProc, procName);
                else if (options->func == c16napi_fun_textcopy || options->func == c16napi_fun_textrename) {
                    status = napi_get_value_string_latin1(env, argv[2], procNameNew, 100, NULL);
                    if (status == napi_ok) {
                        if (options->func == c16napi_fun_textcopy)
                            nErg = C16_TextCopy(hProc, procName, procNameNew);
                        else if (options->func == c16napi_fun_textrename)
                            nErg = C16_TextRename(hProc, procName, procNameNew);
                    } else {
                        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des zweiten übergebenen Prozedurnamens!");
                        return NULL;
                    }
                }
                else {
                    napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsoptionen!");
                    return NULL;
                }
            } else {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des ersten übergebenen Prozedurnamens!");
                return NULL;        
            }
            if (C16_TextClose(hProc) != C16ERR_OK) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
            return NULL;
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// Inhalt einer CONZEPT 16-Prozedur lesen
napi_value c16ReadProc(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[2];
    size_t argc = 2;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    //napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;

    // Text-Handle
    vPHANDLE hProc;
    // Puffer f. Prozedurname
    vCHAR procName[100];

    napi_value procContent;

    // Funktionsargumente lesen, erwartet wird:
    // - das DB-Instanz-Handle
    // - Prozedurname
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 2) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens zwei Funktionsargumente übergeben werden!");
        return NULL;
    }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        nErg = C16_ProcOpen(hInstance, &hProc);
        if (nErg == C16ERR_OK) {
            // utf8 geht in beide Richtungen nicht richtig,
            // latin1 gibt zwar auch Kodierungsprobleme, aber geht trotzdem in beide Richtungen,
            // d.h. reines Darstellungsproblem
            status = napi_get_value_string_latin1(env, argv[1], procName, 100, NULL);
            if (status == napi_ok) { 
                vC16_TextInfo procInfo;
                procInfo.InfoSize = sizeof(vC16_TextInfo);
                nErg = C16_TextRead(hProc, procName, 0, &procInfo);
                if (nErg == C16ERR_OK) {
                    char *buffer = calloc(procInfo.TextSize+1, sizeof(char));
                    vINT textSize = 0;
                    if (buffer) {
                        // trailing \r\n, falls Text im C16-Editor verändert?
                        nErg = C16_TextReadData(hProc, buffer, _Text_CharSet_ANSI | _TextEolCRLF, procInfo.TextSize+1, &textSize, NULL);
                        if (nErg == C16ERR_OK) {
                            if (napi_create_string_latin1(env, buffer, NAPI_AUTO_LENGTH, &procContent) != napi_ok) {
                                free(buffer);
                                C16_TextClose(hProc);
                                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                                return NULL;
                            }
                            free(buffer);
                        } else {
                            free(buffer);
                            C16_TextClose(hProc);
                            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                            return NULL;  
                        }
                    } else {
                        C16_TextClose(hProc);
                        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Reservieren von Speicher!");
                        return NULL;    
                    }
                } else {
                    C16_TextClose(hProc);
                    napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                    return NULL;
                }
            } else {
                C16_TextClose(hProc);
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des übergebenen Prozedurnamens!");
                return NULL;        
            }
            if (C16_TextClose(hProc) != C16ERR_OK) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
            return NULL;
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    return procContent;
}

// CONZEPT 16-Prozedur sperren, Inhalt dieser Prozedur schreiben 
napi_value c16LockWriteProc(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[3];
    size_t argc = 3;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;

    // Text-Handle
    vPHANDLE hProc;
    // Puffer f. Prozedurname
    vCHAR procName[100];
    char *buffer = NULL;
    size_t bufsize = 0;

    // Funktionsargumente lesen, erwartet wird:
    // - das DB-Instanz-Handle
    // - Prozedurname
    // - Text
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 3) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens drei Funktionsargumente übergeben werden!");
        return NULL;
    }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        nErg = C16_ProcOpen(hInstance, &hProc);
        if (nErg == C16ERR_OK) {
            // utf8 geht in beide Richtungen nicht richtig,
            // latin1 gibt zwar auch Kodierungsprobleme, aber geht trotzdem in beide Richtungen,
            // d.h. wäre ein reines Darstellungsproblem
            status = napi_get_value_string_latin1(env, argv[1], procName, 100, NULL);
            if (status == napi_ok) { 
                vC16_TextInfo procInfo;
                procInfo.InfoSize = sizeof(vC16_TextInfo);
                // Text sperren, das geht, wenn er a) noch von niemandem gesperrt ist, b) von dem akt. Benutzer gesperrt ist
                nErg = C16_TextRead(hProc, procName, _TextLock, &procInfo);
                if (nErg == C16ERR_OK) {
                    if (napi_get_value_string_latin1(env, argv[2], NULL, 0, &bufsize) == napi_ok && bufsize > 0) {
                        buffer = calloc(bufsize+1, sizeof(char));
                        if (buffer) {
                            if (napi_get_value_string_latin1(env, argv[2], buffer, bufsize+1, NULL) == napi_ok) {
                                nErg = C16_TextWriteData(hProc, buffer, _Text_CharSet_ANSI | _TextEolCRLF, bufsize);
                                free(buffer);
                            } else {
                                free(buffer);
                                C16_TextClose(hProc);
                                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des übergebenen Prozedurtextes!");
                                return NULL;
                            }
                        } else {
                            C16_TextClose(hProc);
                            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Reservieren von Speicher!");
                            return NULL;
                        }
                    } else {
                        C16_TextClose(hProc);
                        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des übergebenen Prozedurtextes!");
                        return NULL;
                    }
                }
            } else {
                C16_TextClose(hProc);
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des übergebenen Prozedurnamens!");
                return NULL;
            }
            if (C16_TextClose(hProc) != C16ERR_OK) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Zugriff auf die Prozedur!");
            return NULL;
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    // C16-Rückgabewert in napi_value f. JavaScript umwandeln
    status = napi_create_int32(env, nErg, &erg);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Umwandeln des CONZEPT 16-Error-Codes in einen napi_value!");
        return NULL;
    }

    return erg;
}

// Übersetzen einer CONZEPT 16-Prozedur
napi_value c16CompileProc(napi_env env, napi_callback_info args) {
    napi_status status;
    // Funktionsparameter
    napi_value argv[2];
    size_t argc = 2;

    vERROR nErg;
    // Weitergabe des C16-Rückgabewerts
    //napi_value erg;
    
    vPHANDLE hInstance;
    // Übernahme des C16-DB-Instanz-Handles aus JavaScript
    int64_t instance;

    // Puffer f. Prozedurname(n)
    vCHAR procName[100];
    
    // Funktionsargumente lesen, erwartet wird:
    // - das DB-Instanz-Handle
    // - Prozedurname
    status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen der Funktionsargumente!");
        return NULL;
    }
    if (argc < 2) {
        napi_throw_error(env, C16NAPI_ERR_ARGCOUNT, "Es müssen mindestens zwei Funktionsargumente übergeben werden!");
        return NULL;
    }

    napi_value keyErrorCode;
    napi_value errorCode;
    napi_value keyErrorLine;
    napi_value errorLine;
    napi_value keyErrorColumn;
    napi_value errorColumn;
    napi_value keyErrorProcSource;
    napi_value errorProcSource;
    napi_value keyErrorLineText;
    napi_value errorLineText;
    napi_value keyErrorCodeText;
    napi_value errorCodeText;
    napi_value errorInfo;

    // die Keys können schon vorbereitet werden
    if (   napi_create_string_utf8(env, "ErrorCode", NAPI_AUTO_LENGTH, &keyErrorCode) != napi_ok 
        || napi_create_string_utf8(env, "ErrorLine", NAPI_AUTO_LENGTH, &keyErrorLine) != napi_ok
        || napi_create_string_utf8(env, "ErrorColumn", NAPI_AUTO_LENGTH, &keyErrorColumn) != napi_ok
        || napi_create_string_utf8(env, "ErrorProcSource", NAPI_AUTO_LENGTH, &keyErrorProcSource) != napi_ok
        || napi_create_string_utf8(env, "ErrorLineText", NAPI_AUTO_LENGTH, &keyErrorLineText) != napi_ok
        || napi_create_string_utf8(env, "ErrorCodeText", NAPI_AUTO_LENGTH, &keyErrorCodeText) != napi_ok) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Erstellen der ErrorInfos!");
            return NULL;
        }

    // übergebenen Wert als in64 interpretieren
    status = napi_get_value_int64(env, argv[0], &instance);
    if (status == napi_ok) {
        // übergebenen int64-Wert als DB-Instanz-Handle interpretieren
        hInstance = (vPHANDLE)instance;
        // utf8 geht in beide Richtungen nicht richtig,
        // latin1 gibt zwar auch Kodierungsprobleme, aber geht trotzdem in beide Richtungen,
        // d.h. reines Darstellungsproblem
        // vectorsoft: Prozedurnamen können derzeit nur in C16-Kodierung gelesen werden :-(
        status = napi_get_value_string_latin1(env, argv[1], procName, 100, NULL);
        if (status == napi_ok) {
            vC16_CompileInfo compileInfo;
            memset(&compileInfo, 0, sizeof(compileInfo));
            nErg = C16_ProcCompile(hInstance, procName, "*U", &compileInfo);
            if (   napi_create_object(env, &errorInfo) != napi_ok
                || napi_create_int32(env, compileInfo.ErrorCode, &errorCode) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorCode, errorCode) != napi_ok
                || napi_create_uint32(env, compileInfo.ErrorLine, &errorLine) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorLine, errorLine) != napi_ok
                || napi_create_uint32(env, compileInfo.ErrorColumn, &errorColumn) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorColumn, errorColumn) != napi_ok
                || napi_create_string_latin1(env, compileInfo.ErrorProcSource, NAPI_AUTO_LENGTH, &errorProcSource) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorProcSource, errorProcSource) != napi_ok
                || napi_create_string_latin1(env, compileInfo.ErrorLineText, NAPI_AUTO_LENGTH, &errorLineText) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorLineText, errorLineText) != napi_ok
                || napi_create_string_latin1(env, compileInfo.ErrorCodeText, NAPI_AUTO_LENGTH, &errorCodeText) != napi_ok
                || napi_set_property(env, errorInfo, keyErrorCodeText, errorCodeText) != napi_ok
                ) {
                napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Erstellen der ErrorInfos!");
                return NULL;
            }
        } else {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Lesen des übergebenen Prozedurnamens!");
            return NULL;        
        }
    } else {
        napi_throw_type_error(env, C16NAPI_ERR_TYPE, "Fehler beim Umwandeln des übergebenen Werts in einen int64!");
        return NULL;
    }
    
    return errorInfo;
}

// Finalizer für d. einfachen Prozedurfunktionen
void c16SimpleProcFunsFinalizer(napi_env env, void *data, void *hint) {
    if (data) free(data);
}

// N-API-Modul initialisieren
napi_value init(napi_env env, napi_value exports) {
    napi_value c16InitModuleFun, c16GetModuleInfoFun, c16TermModuleFun, c16InitInstanceFun
             , c16TermInstanceFun, c16OpenAreaFun, c16CloseAreaFun
             , c16GetProcsMetadataFun, c16GetProcMetadataFun, c16LockProcFun, c16UnlockProcFun
             , c16ReadProcFun, c16LockWriteProcFun, c16CreateProcFun
             , c16DeleteProcFun, c16CopyProcFun, c16RenameProcFun
             , c16CompileProcFun;

    // init der Funktionen ohne user data
    if (   napi_create_function(env, NULL, 0, c16InitModule, NULL, &c16InitModuleFun) != napi_ok
        || napi_set_named_property(env, exports, "c16InitModule", c16InitModuleFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16GetModuleInfo, NULL, &c16GetModuleInfoFun) != napi_ok
        || napi_set_named_property(env, exports, "c16GetModuleInfo", c16GetModuleInfoFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16TermModule, NULL, &c16TermModuleFun) != napi_ok
        || napi_set_named_property(env, exports, "c16TermModule", c16TermModuleFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16InitInstance, NULL, &c16InitInstanceFun) != napi_ok
        || napi_set_named_property(env, exports, "c16InitInstance", c16InitInstanceFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16TermInstance, NULL, &c16TermInstanceFun) != napi_ok
        || napi_set_named_property(env, exports, "c16TermInstance", c16TermInstanceFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16OpenArea, NULL, &c16OpenAreaFun) != napi_ok
        || napi_set_named_property(env, exports, "c16OpenArea", c16OpenAreaFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16CloseArea, NULL, &c16CloseAreaFun) != napi_ok
        || napi_set_named_property(env, exports, "c16CloseArea", c16CloseAreaFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16ReadProc, NULL, &c16ReadProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16ReadProc", c16ReadProcFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16LockWriteProc, NULL, &c16LockWriteProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16LockWriteProc", c16LockWriteProcFun) != napi_ok
        || napi_create_function(env, NULL, 0, c16CompileProc, NULL, &c16CompileProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16CompileProc", c16CompileProcFun) != napi_ok
        ) {
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Initialisieren des Moduls!");
            return NULL;
        }


    // optionen (user data) f. nachfolgende Funktionen erstellen
    struct c16napi_options *opt_lock = malloc(sizeof(struct c16napi_options));
    if (opt_lock) {
        opt_lock->func = c16napi_fun_textread;
        opt_lock->funcopts = _TextLock;
    }
    struct c16napi_options *opt_unlock = malloc(sizeof(struct c16napi_options));
    if (opt_unlock) {
        opt_unlock->func = c16napi_fun_textread;
        opt_unlock->funcopts = _TextUnlock;
    }
    struct c16napi_options *opt_create = malloc(sizeof(struct c16napi_options));
    if (opt_create) {
        opt_create->func = c16napi_fun_textcreate;
        opt_create->funcopts = 0;
    }
    struct c16napi_options *opt_delete = malloc(sizeof(struct c16napi_options));
    if (opt_delete) {
        opt_delete->func = c16napi_fun_textdelete;
        opt_delete->funcopts = 0;
    }
    struct c16napi_options *opt_copy = malloc(sizeof(struct c16napi_options));
    if (opt_copy) {
        opt_copy->func = c16napi_fun_textcopy;
        opt_copy->funcopts = 0;
    }
    struct c16napi_options *opt_rename = malloc(sizeof(struct c16napi_options));
    if (opt_rename) {
        opt_rename->func = c16napi_fun_textrename;
        opt_rename->funcopts = 0;
    }
    struct c16napi_options *opt_getprocsmetadata = malloc(sizeof(struct c16napi_options));
    if (opt_getprocsmetadata) {
        opt_getprocsmetadata->func = c16napi_fun_getprocsmetadata;
        opt_getprocsmetadata->funcopts = 0;
    }
    struct c16napi_options *opt_getprocmetadata = malloc(sizeof(struct c16napi_options));
    if (opt_getprocmetadata) {
        opt_getprocmetadata->func = c16napi_fun_getprocmetadata;
        opt_getprocmetadata->funcopts = 0;
    }

    if (!opt_lock || !opt_unlock || !opt_create || !opt_delete || !opt_copy || !opt_rename || !opt_getprocsmetadata || !opt_getprocmetadata) {
        if (opt_lock) free(opt_lock);
        if (opt_unlock) free(opt_unlock);
        if (opt_create) free(opt_create);
        if (opt_delete) free(opt_delete);
        if (opt_copy) free(opt_copy);
        if (opt_rename) free(opt_rename);
        if (opt_getprocsmetadata) free(opt_getprocsmetadata);
        if (opt_getprocmetadata) free(opt_getprocmetadata);

        napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Initialisieren des Moduls!");
        return NULL;
    }

    //todo: mz simple proc finalizer nutzen
    // init der Funktionen mit user data
    if (   napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_lock, &c16LockProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16LockProc", c16LockProcFun) != napi_ok
        || napi_add_finalizer(env, c16LockProcFun, opt_lock, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok
        || napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_unlock, &c16UnlockProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16UnlockProc", c16UnlockProcFun) != napi_ok
        || napi_add_finalizer(env, c16UnlockProcFun, opt_unlock, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok
        || napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_create, &c16CreateProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16CreateProc", c16CreateProcFun) != napi_ok
        || napi_add_finalizer(env, c16CreateProcFun, opt_create, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok
        || napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_delete, &c16DeleteProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16DeleteProc", c16DeleteProcFun) != napi_ok
        || napi_add_finalizer(env, c16DeleteProcFun, opt_delete, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok
        || napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_copy, &c16CopyProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16CopyProc", c16CopyProcFun) != napi_ok
        || napi_add_finalizer(env, c16CopyProcFun, opt_copy, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok

        || napi_create_function(env, NULL, 0, c16SimpleProcFuns, opt_rename, &c16RenameProcFun) != napi_ok
        || napi_set_named_property(env, exports, "c16RenameProc", c16RenameProcFun) != napi_ok
        || napi_add_finalizer(env, c16RenameProcFun, opt_rename, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok

        || napi_create_function(env, NULL, 0, c16GetProcsMetadata, opt_getprocsmetadata, &c16GetProcsMetadataFun) != napi_ok
        || napi_set_named_property(env, exports, "c16GetProcsMetadata", c16GetProcsMetadataFun) != napi_ok
        || napi_add_finalizer(env, c16RenameProcFun, opt_getprocsmetadata, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok

        || napi_create_function(env, NULL, 0, c16GetProcsMetadata, opt_getprocmetadata, &c16GetProcMetadataFun) != napi_ok
        || napi_set_named_property(env, exports, "c16GetProcMetadata", c16GetProcMetadataFun) != napi_ok
        || napi_add_finalizer(env, c16RenameProcFun, opt_getprocmetadata, c16SimpleProcFunsFinalizer, NULL, NULL) != napi_ok

        ) {
            if (opt_lock) free(opt_lock);
            if (opt_unlock) free(opt_unlock);
            if (opt_create) free(opt_create);
            if (opt_delete) free(opt_delete);
            if (opt_copy) free(opt_copy);
            if (opt_rename) free(opt_rename);
            napi_throw_error(env, C16NAPI_ERR_GENERIC, "Fehler beim Initialisieren des Moduls!");
            return NULL;
        }
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);