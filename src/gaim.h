/*
 * gaim
 *
 * Copyright (C) 1998-1999, Mark Spencer <markster@marko.net>
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
 *
 */

#ifndef _GAIM_GAIM_H_
#define _GAIM_GAIM_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#ifdef USE_APPLET
#include <applet-widget.h>
#endif /* USE_APPLET */
#ifdef USE_GNOME
#include <gnome.h>
#endif
#include "multi.h"


#define BROWSER_NETSCAPE              0
#define BROWSER_KFM                   1
#define BROWSER_MANUAL                2
/*#define BROWSER_INTERNAL              3*/
#define BROWSER_GNOME                 4

#define IM_FLAG_AWAY     0x01
#define IM_FLAG_CHECKBOX 0x02
#define IM_FLAG_GAIMUSER 0x04

#define IDLE_NONE        0
#define IDLE_GAIM        1
#define IDLE_SCREENSAVER 2

#define PERMIT_ALL	1
#define PERMIT_NONE	2
#define PERMIT_SOME	3
#define DENY_SOME	4

#define UC_AOL		1
#define UC_ADMIN 	2
#define UC_UNCONFIRMED	4
#define UC_NORMAL	8
#define UC_UNAVAILABLE  16

#define WFLAG_SEND	0x01
#define WFLAG_RECV	0x02
#define WFLAG_AUTO	0x04
#define WFLAG_WHISPER	0x08
#define WFLAG_FILERECV	0x10
#define WFLAG_SYSTEM	0x20
#define WFLAG_NICK	0x40

#define AUTO_RESPONSE "&lt;AUTO-REPLY&gt; : "

#define PLUGIN_DIR ".gaim/plugins/"

#define WEBSITE "http://gaim.sourceforge.net/"

#define FACE_ANGEL 0
#define FACE_BIGSMILE 1
#define FACE_BURP 2
#define FACE_CROSSEDLIPS 3
#define FACE_CRY 4
#define FACE_EMBARRASSED 5
#define FACE_KISS 6
#define FACE_MONEYMOUTH 7
#define FACE_SAD 8
#define FACE_SCREAM 9
#define FACE_SMILE 10
#define FACE_SMILE8 11
#define FACE_THINK 12
#define FACE_TONGUE 13
#define FACE_WINK 14
#define FACE_YELL 15
#define FACE_TOTAL 16

#ifndef USE_GNOME
#ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(x) gettext(x)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define N_(String) (String)
#  define _(x) (x)
#endif
#endif

extern struct debug_window *dw;

struct aim_user {
	char username[64];
	char password[32];
	char user_info[2048];
	int options;
	int protocol;
	/* prpls can use this to save information about the user,
	 * like which server to connect to, etc */
	char proto_opt[7][256];

	/* buddy icon file */
	char iconfile[256];

	struct gaim_connection *gc;

	/* stuff for modify window */
	GtkWidget *mod;
	GtkWidget *main;
	GtkWidget *name;
	GtkWidget *pwdbox;
	GtkWidget *pass;
	GtkWidget *rempass;
	int tmp_options;
	int tmp_protocol;
	GList *opt_entries;

	/* stuff for password prompt */
	GtkWidget *passprmt;
	GtkWidget *passentry;

	/* stuff for mail check prompt */
	GtkWidget *checkmail;

	/* when you get kicked offline, only show one dialog */
	GtkWidget *kick_dlg;
};

#define OPT_USR_AUTO		0x00000001
/*#define OPT_USR_KEEPALV	0x00000002 this shouldn't be optional */
#define OPT_USR_REM_PASS	0x00000004
#define OPT_USR_MAIL_CHECK      0x00000008

#define DEFAULT_INFO "Visit the GAIM website at <A HREF=\"http://gaim.sourceforge.net/\">http://gaim.sourceforge.net/</A>."

struct save_pos {
        int x;
        int y;
        int width;
        int height;
	int xoff;
	int yoff;
};


struct window_size {
	int width;
	int height;
	int entry_height;
};


struct option_set {
        int *options;
        int option;
};

struct g_url {
	char address[255];
	int port;
        char page[255];
};

enum gaim_event {
	event_signon = 0,
	event_signoff,
	event_away,
	event_back,
	event_im_recv,
	event_im_send,
	event_buddy_signon,
	event_buddy_signoff,
	event_buddy_away,
	event_buddy_back,
	event_buddy_idle,
	event_buddy_unidle,
	event_blist_update,
	event_chat_invited,
	event_chat_join,
	event_chat_leave,
	event_chat_buddy_join,
	event_chat_buddy_leave,
	event_chat_recv,
	event_chat_send,
	event_warned,
	event_error,
	event_quit,
	event_new_conversation,
	event_set_info,
	event_draw_menu,
	event_im_displayed_sent,
	event_im_displayed_rcvd,
	event_chat_send_invite,
	/* any others? it's easy to add... */
};

enum log_event {
	log_signon = 0,
	log_signoff,
	log_away,
	log_back,
	log_idle,
	log_unidle,
	log_quit
};

#ifdef GAIM_PLUGINS
#include <gmodule.h>

struct gaim_plugin {
	GModule *handle;
	char *name;
	char *description;
};

struct gaim_callback {
	GModule *handle;
	enum gaim_event event;
	void *function;
	void *data;
};

extern GList *plugins;
extern GList *callbacks;
#endif

#define EDIT_GC    0
#define EDIT_GROUP 1
#define EDIT_BUDDY 2

struct buddy {
	int edittype;
	char name[80];
	char show[80];
        int present;
	int evil;
	time_t signon;
	time_t idle;
        int uc;
	gushort caps; /* woohoo! */
	void *proto_data; /* what a hack */
	struct gaim_connection *gc; /* the connection it belongs to */
};

struct log_conversation {
	char name[80];
	char filename[512];
        struct log_conversation *next;
};

#define OPT_POUNCE_POPUP    0x001
#define OPT_POUNCE_SEND_IM  0x002
#define OPT_POUNCE_COMMAND  0x004
#define OPT_POUNCE_SOUND		0x008

#define OPT_POUNCE_SIGNON   0x010
#define OPT_POUNCE_UNAWAY   0x020
#define OPT_POUNCE_UNIDLE   0x040

#define OPT_POUNCE_SAVE     0x100

struct buddy_pounce {
        char name[80];
        char message[2048];
	char command[2048];
	char sound[2048];
	
	char pouncer[80];
	int protocol;

	int options;
};

struct queued_message {
	char name[80];
	char *message;
	time_t tm;
	struct gaim_connection *gc;
	int flags;
};

struct queued_away_response {
	char name[80];
	time_t sent_away;
};

struct away_message {
	char name[80];
	char message[2048];
};

struct group {
	int edittype;
	char name[80];
	GSList *members;
	struct gaim_connection *gc; /* the connection it belongs to */
};

struct debug_window {
	GtkWidget *window;
	GtkWidget *entry;
};

#if USE_PIXBUF
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif

/* struct buddy_chat went away and got merged with this. */
struct conversation {
	struct gaim_connection *gc;

	/* stuff used for both IM and chat */
	GtkWidget *window;
	char name[80];
	GtkWidget *text;
	GtkWidget *entry;
	GtkWidget *italic;
	GtkWidget *bold;
	GtkWidget *underline;
	GtkWidget *fgcolorbtn;
	GtkWidget *bgcolorbtn;
	GtkWidget *link;
	GtkWidget *wood;
	GtkWidget *log_button;
	GtkWidget *strike;
	GtkWidget *font;
	GtkWidget *smiley;
	GtkWidget *fg_color_dialog;
	GtkWidget *bg_color_dialog;
	GtkWidget *font_dialog;
	GtkWidget *smiley_dialog;
	GtkWidget *link_dialog;
	GtkWidget *log_dialog;
	int makesound;
	char fontface[128];
	int hasfont;
	int fontsize;
	int hassize;
	GdkColor bgcol;
	int hasbg;
	GdkColor fgcol;
	int hasfg;

	GString *history;

	GtkWidget *send;

	/* stuff used just for IM */
	GtkWidget *lbox;
	GtkWidget *bbox;
	GtkWidget *sw;
	GtkWidget *info;
	GtkWidget *warn;
	GtkWidget *block;
	GtkWidget *add;
	GtkWidget *sep1;
	GtkWidget *sep2;
	GtkWidget *menu;
	GtkWidget *check;
	gboolean unseen;

#if USE_PIXBUF
	/* buddy icon stuff. sigh. */
	GtkWidget *icon;
	GdkPixbuf *unanim;
	GdkPixbufAnimation *anim;
	guint32 icon_timer;
	int frame;
#endif

	/* stuff used just for chat */
        GList *in_room;
        GList *ignored;
	char *topic;
        int id;
	GtkWidget *count;
	GtkWidget *list;
	GtkWidget *whisper;
	GtkWidget *invite;
	GtkWidget *close;
	GtkWidget *topic_text;

	/* something to distinguish */
	gboolean is_chat;
};

#define CONVERSATION_TITLE "%s - Gaim"
#define LOG_CONVERSATION_TITLE "%s - Gaim (logged)"

#define AOL_SRCHSTR "/community/aimcheck.adp/url="

/* These should all be runtime selectable */

#define MSG_LEN 2048
/* The above should normally be the same as BUF_LEN,
 * but just so we're explictly asking for the max message
 * length. */
#define BUF_LEN MSG_LEN
#define BUF_LONG BUF_LEN * 2


#ifdef USE_APPLET
extern GtkWidget *applet;
#endif /* USE_APPLET */

/* Globals in dialog.c */
extern char fontface[64];
extern int fontsize;
extern GdkColor bgcolor;
extern GdkColor fgcolor;
extern int smiley_array[FACE_TOTAL];

/* Globals in aim.c */
extern GList *log_conversations;
extern GList *buddy_pounces;
extern GSList *away_messages;
extern GList *conversations;
extern GtkWidget *mainwindow;
extern int opt_away;
extern char *opt_away_arg;
extern char *opt_rcfile_arg;

/* Globals in buddy_chat.c */
/* it is very important that you don't use this for anything.
 * its sole purpose is to allow all group chats to be in one
 * window. use struct gaim_connection's buddy_chats instead. */
extern GList *chats;
extern GtkWidget *all_chats;
extern GtkWidget *chat_notebook;

extern GtkWidget *joinchat;

/* Globals in away.c */
extern struct away_message *awaymessage;
extern struct away_message *default_away;
extern int auto_away;
extern GtkWidget *awaymenu;
extern GtkWidget *clistqueue; 

/* Globals in prpl.c */
extern GtkWidget *protomenu;

/* Globals in buddy.c */
extern GtkWidget *buddies;
extern GtkWidget *bpmenu;
extern GtkWidget *blist;

extern guint misc_options;
#define OPT_MISC_DEBUG			0x00000001
#define OPT_MISC_BROWSER_POPUP		0x00000002
#define OPT_MISC_BUDDY_TICKER		0x00000004
#define OPT_MISC_COOL_LOOK		0x00000008

extern guint logging_options;
#define OPT_LOG_ALL			0x00000001
#define OPT_LOG_STRIP_HTML		0x00000002
#define OPT_LOG_BUDDY_SIGNON		0x00000004
#define OPT_LOG_BUDDY_IDLE		0x00000008
#define OPT_LOG_BUDDY_AWAY		0x00000010
#define OPT_LOG_MY_SIGNON		0x00000020
#define OPT_LOG_INDIVIDUAL		0x00000040

extern guint blist_options;
#define OPT_BLIST_APP_BUDDY_SHOW	0x00000001
#define OPT_BLIST_SAVED_WINDOWS		0x00000002
#define OPT_BLIST_NEAR_APPLET		0x00000004
#define OPT_BLIST_SHOW_GRPNUM		0x00000008
#define OPT_BLIST_SHOW_PIXMAPS		0x00000010
#define OPT_BLIST_SHOW_IDLETIME		0x00000020
#define OPT_BLIST_SHOW_BUTTON_XPM	0x00000040
#define OPT_BLIST_NO_BUTTONS		0x00000080
#define OPT_BLIST_NO_MT_GRP		0x00000100
#define OPT_BLIST_SHOW_WARN		0x00000200

extern guint convo_options;
#define OPT_CONVO_ENTER_SENDS		0x00000001
#define OPT_CONVO_SEND_LINKS		0x00000002
#define OPT_CONVO_CHECK_SPELLING	0x00000004
#define OPT_CONVO_CTL_CHARS		0x00000008
#define OPT_CONVO_CTL_SMILEYS		0x00000010
#define OPT_CONVO_ESC_CAN_CLOSE		0x00000020
#define OPT_CONVO_CTL_ENTER		0x00000040
#define OPT_CONVO_F2_TOGGLES		0x00000080
#define OPT_CONVO_SHOW_TIME		0x00000100
#define OPT_CONVO_IGNORE_COLOUR		0x00000200
#define OPT_CONVO_SHOW_SMILEY		0x00000400
#define OPT_CONVO_IGNORE_FONTS		0x00000800
#define OPT_CONVO_IGNORE_SIZES		0x00001000

extern guint im_options;
#define OPT_IM_POPUP			0x00000001
#define OPT_IM_LOGON			0x00000002
#define OPT_IM_BUTTON_TEXT		0x00000004
#define OPT_IM_BUTTON_XPM		0x00000008
#define OPT_IM_ONE_WINDOW		0x00000010
#define OPT_IM_SIDE_TAB			0x00000020
#define OPT_IM_BR_TAB			0x00000040
#define OPT_IM_HIDE_ICONS		0x00000080

extern guint chat_options;
#define OPT_CHAT_ONE_WINDOW		0x00000001
#define OPT_CHAT_BUTTON_TEXT		0x00000002
#define OPT_CHAT_BUTTON_XPM		0x00000004
#define OPT_CHAT_LOGON			0x00000008
#define OPT_CHAT_POPUP			0x00000010
#define OPT_CHAT_SIDE_TAB		0x00000020
#define OPT_CHAT_BR_TAB			0x00000040

extern guint font_options;
#define OPT_FONT_BOLD			0x00000001
#define OPT_FONT_ITALIC			0x00000002
#define OPT_FONT_UNDERLINE		0x00000008
#define OPT_FONT_STRIKE			0x00000010
#define OPT_FONT_FACE			0x00000020
#define OPT_FONT_FGCOL			0x00000040
#define OPT_FONT_BGCOL			0x00000080
#define OPT_FONT_SIZE			0x00000100

extern guint sound_options;
#define OPT_SOUND_LOGIN			0x00000001
#define OPT_SOUND_LOGOUT		0x00000002
#define OPT_SOUND_RECV			0x00000004
#define OPT_SOUND_SEND			0x00000008
#define OPT_SOUND_FIRST_RCV		0x00000010
#define OPT_SOUND_WHEN_AWAY		0x00000020
#define OPT_SOUND_SILENT_SIGNON		0x00000040
#define OPT_SOUND_THROUGH_GNOME		0x00000080
#define OPT_SOUND_CHAT_JOIN		0x00000100
#define OPT_SOUND_CHAT_SAY		0x00000200
#define OPT_SOUND_BEEP			0x00000400
#define OPT_SOUND_CHAT_PART		0x00000800
#define OPT_SOUND_CHAT_YOU_SAY		0x00001000

#define BUDDY_ARRIVE 0
#define BUDDY_LEAVE 1
#define RECEIVE 2
#define FIRST_RECEIVE 3
#define SEND 4
#define CHAT_JOIN 5
#define CHAT_LEAVE 6
#define CHAT_YOU_SAY 7
#define CHAT_SAY 8
#define POUNCE_DEFAULT 9
#define NUM_SOUNDS 10
extern char *sound_file[NUM_SOUNDS];

extern guint away_options;
#define OPT_AWAY_DISCARD		0x00000001
#define OPT_AWAY_BACK_ON_IM		0x00000002
#define OPT_AWAY_TIK_HACK		0x00000004
#define OPT_AWAY_AUTO			0x00000008
#define OPT_AWAY_NO_AUTO_RESP		0x00000010
#define OPT_AWAY_QUEUE			0x00000020

extern int report_idle;
extern int web_browser;
extern GList *aim_users;
extern GSList *message_queue;
extern GSList *away_time_queue;
extern char sound_cmd[2048];
extern char web_command[2048];
extern struct save_pos blist_pos;
extern struct window_size conv_size, buddy_chat_size;

/* Functions in about.c */
extern void show_about(GtkWidget *, void *);
extern void gaim_help(GtkWidget *, void *);

/* Functions in buddy_chat.c */
extern void join_chat();
extern void chat_write(struct conversation *, char *, int, char *, time_t);
extern void add_chat_buddy(struct conversation *, char *);
extern void rename_chat_buddy(struct conversation *, char *, char *);
extern void remove_chat_buddy(struct conversation *, char *);
extern void show_new_buddy_chat(struct conversation *);
extern void delete_chat(struct conversation *);
extern void build_imchat_box(gboolean);
extern void do_quit();
extern void update_chat_button_pix();
extern void update_im_button_pix();
extern void update_chat_tabs();
extern void update_im_tabs();
extern void update_idle_times();
extern void do_join_chat();
extern void chat_set_topic(struct conversation*, char*, char*);

/* Functions in html.c */
extern struct g_url parse_url(char *);
extern void grab_url(char *, void (*callback)(gpointer, char *), gpointer);
extern gchar *strip_html(gchar *);

/* Functions in idle.c */
extern gint check_idle(struct gaim_connection *);

/* Functions in util.c */
extern char *normalize(const char *);
extern char *escape_text2(const char *);
extern char *tobase64(const char *);
extern void frombase64(const char *, char **, int *);
extern gint clean_pid(gpointer);
extern char *date();
extern gint linkify_text(char *);
extern void aol_icon(GdkWindow *);
extern FILE *open_log_file (char *);
extern char *sec_to_text(guint);
extern struct aim_user *find_user(const char *, int);
extern char *full_date();
extern void check_gaim_versions();
extern char *away_subs(char *, char *);
extern GtkWidget *picture_button(GtkWidget *, char *, char **);
extern GtkWidget *picture_button2(GtkWidget *, char *, char **, short);
extern void translate_lst (FILE *, char *);
extern void translate_blt (FILE *, char *);
extern char *stylize(gchar *, int);
extern int set_dispstyle (int);
extern void show_usage (int, char *);
extern void set_first_user (char *);
extern int do_auto_login (char *);
extern int file_is_dir (const char *, GtkWidget *);
extern char *gaim_user_dir();
extern void strncpy_nohtml(gchar *, const gchar *, size_t);
extern void strncpy_withhtml(gchar *, const gchar *, size_t);
extern void away_on_login(char *);
extern void system_log(enum log_event, struct gaim_connection *, struct buddy *, int);
extern unsigned char *utf8_to_str(unsigned char *);
extern char *str_to_utf8(unsigned char *);

/* Functions in server.c */
/* input to serv */
extern void serv_login(struct aim_user *);
extern void serv_close(struct gaim_connection *);
extern void serv_touch_idle(struct gaim_connection *);
extern void serv_finish_login();
extern int  serv_send_im(struct gaim_connection *, char *, char *, int);
extern void serv_get_info(struct gaim_connection *, char *);
extern void serv_get_dir(struct gaim_connection *, char *);
extern void serv_set_idle(struct gaim_connection *, int);
extern void serv_set_info(struct gaim_connection *, char *);
extern void serv_set_away(struct gaim_connection *, char *, char *);
extern void serv_set_away_all(char *);
extern void serv_change_passwd(struct gaim_connection *, char *, char *);
extern void serv_add_buddy(struct gaim_connection *, char *);
extern void serv_add_buddies(struct gaim_connection *, GList *);
extern void serv_remove_buddy(struct gaim_connection *, char *);
extern void serv_remove_buddies(struct gaim_connection *, GList *);
extern void serv_add_permit(struct gaim_connection *, char *);
extern void serv_add_deny(struct gaim_connection *, char *);
extern void serv_rem_permit(struct gaim_connection *, char *);
extern void serv_rem_deny(struct gaim_connection *, char *);
extern void serv_set_permit_deny(struct gaim_connection *);
extern void serv_warn(struct gaim_connection *, char *, int);
extern void serv_set_dir(struct gaim_connection *, char *, char *, char *, char *, char *, char *, char *, int);
extern void serv_dir_search(struct gaim_connection *, char *, char *, char *, char *, char *, char *, char *, char *);
extern void serv_join_chat(struct gaim_connection *, GList *);
extern void serv_chat_invite(struct gaim_connection *, int, char *, char *);
extern void serv_chat_leave(struct gaim_connection *, int);
extern void serv_chat_whisper(struct gaim_connection *, int, char *, char *);
extern int serv_chat_send(struct gaim_connection *, int, char *);
extern void update_keepalive(struct gaim_connection *, gboolean);

/* output from serv */
extern void serv_got_update(struct gaim_connection *, char *, int, int, time_t, time_t, int, gushort);
extern void serv_got_im(struct gaim_connection *, char *, char *, guint32, time_t);
extern void serv_got_eviled(struct gaim_connection *, char *, int);
extern void serv_got_chat_invite(struct gaim_connection *, char *, char *, char *, GList *);
extern struct conversation *serv_got_joined_chat(struct gaim_connection *, int, char *);
extern void serv_got_chat_left(struct gaim_connection *, int);
extern void serv_got_chat_in(struct gaim_connection *, int, char *, int, char *, time_t);

/* Functions in conversation.c */
extern void gaim_setup_imhtml(GtkWidget *);
extern void update_convo_add_button(struct conversation *);
extern void write_html_with_smileys(GtkWidget *, GtkWidget *, char *);
extern void write_to_conv(struct conversation *, char *, int, char *, time_t);
extern void raise_convo_tab(struct conversation *);
extern void set_convo_tab_label(struct conversation *, char *);
extern void show_conv(struct conversation *);
extern struct conversation *new_conversation(char *);
extern struct conversation *find_conversation(char *);
extern void delete_conversation(struct conversation *);
extern void surround(GtkWidget *, char *, char *);
extern int is_logging(char *);
extern void set_state_lock(int );
extern void rm_log(struct log_conversation *a);
extern struct log_conversation *find_log_info(char *name);
extern void remove_tags(GtkWidget *entry, char *tag);
extern void update_log_convs();
extern void update_transparency();
extern void update_font_buttons();
extern void toggle_sensitive(GtkWidget *widget, GtkWidget *to_toggle);
extern void do_bold(GtkWidget *, GtkWidget *);
extern void do_italic(GtkWidget *, GtkWidget *);
extern void do_underline(GtkWidget *, GtkWidget *);
extern void do_strike(GtkWidget *, GtkWidget *);
extern void do_small(GtkWidget *, GtkWidget *);
extern void do_normal(GtkWidget *, GtkWidget *);
extern void do_big(GtkWidget *, GtkWidget *);
extern void set_font_face(char *, struct conversation *);
extern void redo_convo_menus();
extern void convo_menu_remove(struct gaim_connection *);
extern void remove_icon_data(struct gaim_connection *);
extern void got_new_icon(struct gaim_connection *, char *);
extern void toggle_spellchk();
extern void set_convo_gc(struct conversation *, struct gaim_connection *);
extern void update_buttons_by_protocol(struct conversation *);
extern void toggle_smileys();
extern void toggle_timestamps();
extern void update_pixmaps();
extern void tabize();
extern void chat_tabize();
extern void update_convo_color();
extern void update_convo_font();
extern void set_hide_icons();

/* Functions in toc.c */
extern void parse_toc_buddy_list(struct gaim_connection *, char *, int);

/* Functions in buddy.c */
extern void handle_group_rename(struct group *, char *);
extern void handle_buddy_rename(struct buddy *, char *);
extern void destroy_buddy();
extern void update_button_pix();
extern void toggle_show_empty_groups();
extern void update_all_buddies();
extern void update_num_groups();
extern void show_buddy_list();
extern void refresh_buddy_window();
extern void toc_build_config(struct gaim_connection *, char *, int len, gboolean);
extern void signoff(struct gaim_connection *);
extern void signoff_all(GtkWidget *, gpointer);
extern void do_im_back();
extern void set_buddy(struct gaim_connection *, struct buddy *);
extern struct group *add_group(struct gaim_connection *, char *);
extern void add_category(char *);
extern void build_edit_tree();
extern void remove_person(struct group *, struct buddy *);
extern void remove_category(struct group *);
extern void do_pounce(struct gaim_connection *, char *, int);
extern void do_bp_menu();
extern struct buddy *find_buddy(struct gaim_connection *, char *);
extern struct group *find_group(struct gaim_connection *, char *);
extern struct group *find_group_by_buddy(struct gaim_connection *, char *);
extern void remove_buddy(struct gaim_connection *, struct group *, struct buddy *);
extern struct buddy *add_buddy(struct gaim_connection *, char *, char *, char *);
extern void remove_group(struct gaim_connection *, struct group *);
extern void toggle_buddy_pixmaps();
extern void gaim_separator(GtkWidget *);

/* Functions in away.c */
extern void rem_away_mess(GtkWidget *, struct away_message *);
extern void do_away_message(GtkWidget *, struct away_message *);
extern void do_away_menu();
extern void away_list_unclicked(GtkWidget *, struct away_message *);
extern void away_list_clicked(GtkWidget *, struct away_message *);
extern void toggle_away_queue();
extern void purge_away_queue();

/* Functions in aim.c */
extern void show_login();
extern void gaim_setup(struct gaim_connection *gc);
#ifdef USE_APPLET
extern void createOnlinePopup();
extern void applet_show_login(AppletWidget *, gpointer);
GtkRequisition gnome_buddy_get_dimentions();
#endif


/* Functions in sound.c */
extern void play_sound(int);
extern void play_file(char *);

/* Functions in perl.c */
#ifdef USE_PERL
extern void perl_autoload();
extern void perl_end();
extern int perl_event(char *, char *);
extern int perl_load_file(char *);
extern void unload_perl_scripts();
extern void list_perl_scripts();
#endif

/* Functions in plugins.c */
#ifdef GAIM_PLUGINS
extern void show_plugins(GtkWidget *, gpointer);
extern void load_plugin (char *);
extern void gaim_signal_connect(GModule *, enum gaim_event, void *, void *);
extern void gaim_signal_disconnect(GModule *, enum gaim_event, void *);
extern void gaim_plugin_unload(GModule *);
#endif
extern int plugin_event(enum gaim_event, void *, void *, void *, void *);
extern void remove_all_plugins();

/* Functions in prefs.c */
extern void debug_printf( char * fmt, ... );
#define debug_print(x) debug_printf(x);
extern void set_option(GtkWidget *, int *);
extern void show_prefs();
extern void show_debug();
extern void update_color(GtkWidget *, GtkWidget *);
extern void set_default_away(GtkWidget *, gpointer);
extern void default_away_menu_init(GtkWidget *);
extern void update_connection_dependent_prefs();
extern void build_allow_list();
extern void build_block_list();
extern GtkWidget *prefs_away_list;
extern GtkWidget *prefs_away_menu;
extern GtkWidget *pref_fg_picture;
extern GtkWidget *pref_bg_picture;


/* Functions in gaimrc.c */
extern void load_prefs();
extern void save_prefs();
extern gint sort_awaymsg_list(gconstpointer, gconstpointer);

gint sort_awaymsg_list(gconstpointer, gconstpointer);

/* Functions in dialogs.c */
extern void alias_dialog_bud(struct buddy *);
extern void do_export(struct gaim_connection *);
extern void show_warn_dialog(struct gaim_connection *, char *);
extern GtkWidget *do_error_dialog(char *, char *);
extern void show_im_dialog();
extern void some_name(char *);
extern void show_info_dialog();
extern void show_add_buddy(struct gaim_connection *, char *, char *);
extern void show_add_group(struct gaim_connection *);
extern void show_add_perm(struct gaim_connection *, char *, gboolean);
extern void destroy_all_dialogs();
extern void show_import_dialog();
extern void show_export_dialog();
extern void show_new_bp();
extern void show_log(char *);
extern void show_log_dialog(struct conversation *);
extern void show_find_email(struct gaim_connection *gc);
extern void show_find_info();
extern void g_show_info_text(char *, ...);
extern void show_set_info(struct gaim_connection *);
extern void show_set_dir();
extern void show_fgcolor_dialog(struct conversation *c, GtkWidget *color);
extern void show_bgcolor_dialog(struct conversation *c, GtkWidget *color);
extern void cancel_fgcolor(GtkWidget *widget, struct conversation *c);
extern void cancel_bgcolor(GtkWidget *widget, struct conversation *c);
extern void put_out(struct gaim_connection *, char *, char *());
extern void create_away_mess(GtkWidget *, void *);
extern void show_ee_dialog(int);
extern void show_add_link(GtkWidget *,struct conversation *);
extern void show_change_passwd(struct gaim_connection *);
extern void do_import(GtkWidget *, struct gaim_connection *);
extern int bud_list_cache_exists(struct gaim_connection *);
extern void show_smiley_dialog(struct conversation *, GtkWidget *);
extern void close_smiley_dialog(GtkWidget *widget, struct conversation *c);
extern void set_smiley_array(GtkWidget *widget, int smiley_type);
extern void insert_smiley_text(GtkWidget *widget, struct conversation *c);
extern void cancel_log(GtkWidget *, struct conversation *);
extern void cancel_link(GtkWidget *, struct conversation *);
extern void show_font_dialog(struct conversation *c, GtkWidget *font);
extern void get_good(struct gaim_connection **);
extern void cancel_font(GtkWidget *widget, struct conversation *c);
extern void apply_font(GtkWidget *widget, GtkFontSelection *fontsel);
extern void set_color_selection(GtkWidget *selection, GdkColor color);
extern void show_rename_group(GtkWidget *, struct group *);
extern void show_rename_buddy(GtkWidget *, struct buddy *);
extern void load_perl_script();

/* Functions in browser.c */
extern void open_url(GtkWidget *, char *);
extern void open_url_nw(GtkWidget *, char *);
extern void add_bookmark(GtkWidget *, char *);

/* fucntions in ticker.c */
void SetTickerPrefs();
void BuddyTickerSignOff();
void BuddyTickerAddUser(char *, GdkPixmap *, GdkBitmap *);
void BuddyTickerSetPixmap(char *, GdkPixmap *, GdkBitmap *);
void BuddyTickerSignoff();

#endif /* _GAIM_GAIM_H_ */
