/*
 * Bit.ly URL Shortener for libpurple
 *
 * Copyright (C) 2009, John Bellone <jb@thunkbrightly.com>
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

static PurplePluginPrefFrame *
get_plugin_pref_frame (PurplePlugin * plugin) {
	PurplePluginPrefFrame * frame = purple_plugin_pref_frame_new();
	PurplePluginPref * pref;

	/* Bit.ly Account */
	pref = purple_plugin_pref_new_with_name_and_label ("/plugins/core/bitly_urlshort/account", "Account:");
	purple_plugin_pref_frame_add (frame, pref);

	/* Bit.ly API Key */
	pref = purple_plugin_pref_new_with_name_and_label ("/plugins/core/bitly_urlshort/apikey", "API Key:");
	purple_plugin_pref_frame_add (frame, pref);
	
	return frame;
}

static gboolean
plugin_load (PurplePlugin * plugin) {
	/* STUB: Connect any callbacks here, initialize libcurl, etc. */
	return TRUE;
}

static gboolean
plugin_unload (PurplePlugin * plugin) {
	/* STUB: Deinitialize libcurl, etc */
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
	"http://thunkbrightly.com/",

	plugin_load,                                                
	NULL,
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
	purple_prefs_add_none ("/plugins/core/bitly_urlshort");
	purple_prefs_add_string ("/plugins/core/bitly_urlshort/account", "");
	purple_prefs_add_string ("/plugins/core/bitly_urlshort/apikey", "");
}

PURPLE_INIT_PLUGIN (bitlyurlshort, init_plugin, info)
