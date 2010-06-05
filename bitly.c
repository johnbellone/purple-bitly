/*
 * Bit.ly URL Shortener Plugin for libpurple
 *
 * http://github.com/johnbellone/purple-bitly
 *
 * Copyright (C) 2009, 2010 John Bellone <jb@thunkbrightly.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02111-1301, USA.
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include "internal.h"
#include "plugin.h"
#include "pluginpref.h"
#include "prefs.h"
#include "version.h"

#include <time.h>

#include <cmds.h>
#include <conversation.h>
#include <signals.h>
#include <debug.h>

#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <glib.h>

static PurpleCmdId bitly_cmd_id;
static GString* bitly_url;
static CURL* curl_handle;

/**
 * @brief The implementation function that handles the preference (settings) frame for the plugin.
 * @arg plugin The libpurple Plugin instance to be used.
 * @return An allocated preference frame object. 
 */
static PurplePluginPrefFrame*
get_plugin_pref_frame(PurplePlugin* plugin) {
    PurplePluginPrefFrame* frame = purple_plugin_pref_frame_new();
    PurplePluginPref* pref;

    /* Bit.ly Account */
    pref = purple_plugin_pref_new_with_name_and_label("/plugins/core/bitly_urlshort/account", "Account:");
    purple_plugin_pref_frame_add(frame, pref);

    /* Bit.ly API Key */
    pref = purple_plugin_pref_new_with_name_and_label("/plugins/core/bitly_urlshort/apikey", "API Key:");
    purple_plugin_pref_frame_add(frame, pref);
	
    return frame;
}

/**
 * @brief This is the libcurl callback used when parsing the HTTP response from the bitly api.
 * @return The size of the GString buffer allocation.
 * @arg data A string of data to be appended to the data GString buffer.
 * @arg size The size of the data to be appended.
 * @arg nmemb The size of an individual character(?)
 * @arg An allocated GString buffer. 
 */
static size_t
curl_write_cb(char* data, size_t size, size_t nmemb, void* buffer) {
    GString* string = (GString*) buffer;
    g_string_append(string, data);
    return size * nmemb;						
}

/**
 * @brief This is the implementation function that does all of the heavy lifting. It utilizes its passed in
 * parameters and makes a call to the bitly api using libcurl.
 * @return GString The allocated string object that was passed in as the longUrl parameter containing the
 * result shortUrl from the bitly api call. 
 * @arg login A string representing the login name that will be used in the bitly api call.
 * @arg apiKey A string representing the api key that will be used in the bitly api call.
 * @arg longUrl An allocated GString object containing the url to be shortened. This is also the object in
 * which the shortUrl will be placed. 
 */
static GString*
process_url(const char* login, const char* apiKey, GString* longUrl) {
    char* url = NULL, * end = NULL;
	
    g_string_printf(bitly_url, "http://api.bit.ly/shorten?version=2.0.1&login=%s&apiKey=%s&longUrl=%s",
                    login, apiKey, longUrl->str);

    /* Use libcurl to do all the fancy HTTP requests that we need in order for this bad boy
     * to work correctly. */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "purple-bitly/0.1");
    curl_easy_setopt(curl_handle, CURLOPT_URL, bitly_url->str);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) longUrl);
    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
	
    /* Extract the http://bit.ly URL from the JSON returned back. Add four characters for the
     * offset, e.g. "shortUrl": "http://bit.ly/x1059" and then find position of the quote at end. */
    if ( (url = strstr(longUrl->str, "shortUrl")) != NULL) 
    {
        if ((url = strstr(url, "http://")) != NULL ) 
        {
            g_string_assign(longUrl, url);
            
            end = strstr(longUrl->str, "\"");

            g_string_truncate(longUrl, (end - longUrl->str));
        }
    }

    return longUrl;
}

/**
 * @brief This is the implementation of the actual command that is run from inside of an instant messenger
 * window. It will take the first argument and process it utilizing the process_url function. 
 * @arg conv This is the conversation object passed in from the libpurple api.
 * @arg cmd This is the actual command being executed, in this case it should be "bitly"
 * @arg args This is a character array of arguments. 
 * @arg data Most likely junk. We do not use this parameter.
 */
static PurpleCmdRet
purple_cmd_bitly (PurpleConversation* conv, const char* cmd, char** args, char* error, void* data) {
    PurpleCmdStatus ret;
    GString* str = g_string_new(args[0]);
    char* newcmd = NULL;

    if ( args[0] ) 
    {
        const char* login = purple_prefs_get_string("/plugins/core/bitly_urlshort/account");
        const char* apiKey = purple_prefs_get_string("/plugins/core/bitly_urlshort/apikey");
		
        str = process_url(login, apiKey, str);

        newcmd = g_strdup_printf("say %s", str->str);
	
        ret = purple_cmd_do_command(conv, newcmd, newcmd, &error);

        free(newcmd);
        g_string_free(str, TRUE);
    }
    return ret;
}

/**
 * @brief The implementation function that is called when the plugin is loaded into memory. 
 * @arg plugin The libpurple plugin instance.
 * @return A boolean value indicating if the plugin has been successfully loaded.
 */
static gboolean
plugin_load (PurplePlugin * plugin) {
    const char * help = _("bitly: replaces the argument URL with one through the bit.ly service");

    curl_global_init(CURL_GLOBAL_ALL);
		
    bitly_url = g_string_new("");
    curl_handle = curl_easy_init();
    bitly_cmd_id = purple_cmd_register( "bitly", "wws", PURPLE_CMD_P_PLUGIN,
                                        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT |
                                        PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS,
                                        NULL, PURPLE_CMD_FUNC (purple_cmd_bitly), help, NULL );
    return TRUE;
}

/**
 * @brief The implementation function that is called when the plugin is unloaded from memory.
 * @arg plugin The libpurple plugin instance.
 * @return A boolean value indicating if the plugin has been successfully unloaded.
 */
static gboolean
plugin_unload (PurplePlugin * plugin) {
    purple_cmd_unregister (bitly_cmd_id);

    /* Perform libcurl maintainence procedures. */
    curl_global_cleanup();
	
    /* Get rid of all the trashy global variables. */
    free(curl_handle);
    g_string_free(bitly_url, TRUE);
	
    return TRUE;
}

static PurplePluginUiInfo prefs_info = {
    get_plugin_pref_frame,
    0,
    NULL,
    /* Padding */
    NULL,
    NULL,
    NULL,
    NULL
};

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,
    "core-bitly_urlshort",
    "Bit.ly URL Shortener",
    DISPLAY_VERSION,
    "URL shortener plugin which utilizes the bit.ly API",
    "URL shortener plugin which utilizes the bit.ly API",
    "John Bellone <jb@thunkbrightly.com>",
    "http://wiki.github.com/johnbellone/purple-bitly",

    plugin_load,                                                
    plugin_unload,
    NULL,

    NULL,
    NULL,
    &prefs_info,
    NULL,
    /* Padding */
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin (PurplePlugin * plugin) {
    purple_prefs_add_none("/plugins/core/bitly_urlshort");
    purple_prefs_add_string("/plugins/core/bitly_urlshort/account", "");
    purple_prefs_add_string("/plugins/core/bitly_urlshort/apikey", "");
}

PURPLE_INIT_PLUGIN (bitlyurlshort, init_plugin, info)
