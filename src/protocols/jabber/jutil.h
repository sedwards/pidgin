/**
 * @file jutil.h utility functions
 *
 * gaim
 *
 * Copyright (C) 2003 Nathan Walp <faceprint@faceprint.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _GAIM_JABBER_JID_H_
#define _GAIM_JABBER_JID_H_

#include "account.h"


typedef struct _JabberID {
	char *node;
	char *domain;
	char *resource;
} JabberID;

JabberID* jabber_id_new(const char *str);
void jabber_id_free(JabberID *jid);

const char *jabber_get_resource(const char *jid);
char *jabber_get_bare_jid(const char *jid);

time_t str_to_time(const char *timestamp);
const char *jabber_get_state_string(int state);

const char *jabber_normalize(const GaimAccount *account, const char *in);

#endif /* _GAIM_JABBER_JID_H_ */
