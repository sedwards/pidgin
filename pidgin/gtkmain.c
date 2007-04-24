/*
 * pidgin
 *
 * Pidgin is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
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

#include "internal.h"
#include "pidgin.h"

#include "account.h"
#include "conversation.h"
#include "core.h"
#include "debug.h"
#include "eventloop.h"
#include "ft.h"
#include "log.h"
#include "notify.h"
#include "prefs.h"
#include "prpl.h"
#include "pounce.h"
#include "sound.h"
#include "status.h"
#include "util.h"
#include "whiteboard.h"

#include "gtkaccount.h"
#include "gtkblist.h"
#include "gtkconn.h"
#include "gtkconv.h"
#include "gtkdebug.h"
#include "gtkdialogs.h"
#include "gtkdocklet.h"
#include "gtkeventloop.h"
#include "gtkft.h"
#include "gtkidle.h"
#include "gtklog.h"
#include "gtknotify.h"
#include "gtkplugin.h"
#include "gtkpounce.h"
#include "gtkprefs.h"
#include "gtkprivacy.h"
#include "gtkrequest.h"
#include "gtkroomlist.h"
#include "gtksavedstatuses.h"
#include "gtksession.h"
#include "gtksound.h"
#include "gtkthemes.h"
#include "gtkutils.h"
#include "pidginstock.h"
#include "gtkwhiteboard.h"

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#include <getopt.h>

#ifdef HAVE_STARTUP_NOTIFICATION
# define SN_API_NOT_YET_FROZEN
# include <libsn/sn-launchee.h>
# include <gdk/gdkx.h>
#endif



#ifdef HAVE_STARTUP_NOTIFICATION
static SnLauncheeContext *sn_context = NULL;
static SnDisplay *sn_display = NULL;
#endif

#ifdef HAVE_SIGNAL_H

/*
 * Lists of signals we wish to catch and those we wish to ignore.
 * Each list terminated with -1
 */
static int catch_sig_list[] = {
	SIGSEGV,
	SIGHUP,
	SIGINT,
	SIGTERM,
	SIGQUIT,
	SIGCHLD,
	SIGALRM,
	-1
};

static int ignore_sig_list[] = {
	SIGPIPE,
	-1
};
#endif

static int
dologin_named(const char *name)
{
	PurpleAccount *account;
	char **names;
	int i;
	int ret = -1;

	if (name != NULL) { /* list of names given */
		names = g_strsplit(name, ",", 64);
		for (i = 0; names[i] != NULL; i++) {
			account = purple_accounts_find(names[i], NULL);
			if (account != NULL) { /* found a user */
				ret = 0;
				purple_account_connect(account);
			}
		}
		g_strfreev(names);
	} else { /* no name given, use the first account */
		GList *accounts;

		accounts = purple_accounts_get_all();
		if (accounts != NULL)
		{
			account = (PurpleAccount *)accounts->data;
			ret = 0;
			purple_account_connect(account);
		}
	}

	return ret;
}

#ifdef HAVE_SIGNAL_H
static void sighandler(int sig);

/**
 * Reap all our dead children.  Sometimes libpurple forks off a separate
 * process to do some stuff.  When that process exits we are
 * informed about it so that we can call waitpid() and let it
 * stop being a zombie.
 *
 * We used to do this immediately when our signal handler was
 * called, but because of GStreamer we now wait one second before
 * reaping anything.  Why?  For some reason GStreamer fork()s
 * during their initialization process.  I don't understand why...
 * but they do it, and there's nothing we can do about it.
 *
 * Anyway, so then GStreamer waits for its child to die and then
 * it continues with the initialization process.  This means that
 * we have a race condition where GStreamer is waitpid()ing for its
 * child to die and we're catching the SIGCHLD signal.  If GStreamer
 * is awarded the zombied process then everything is ok.  But if libpurple
 * reaps the zombie process then the GStreamer initialization sequence
 * fails.
 *
 * So the ugly solution is to wait a second to give GStreamer time to
 * reap that bad boy.
 *
 * GStreamer 0.10.10 and newer have a gst_register_fork_set_enabled()
 * function that can be called by applications to disable forking
 * during initialization.  But it's not in 0.10.0, so we shouldn't
 * use it.
 *
 * All of this child process reaping stuff is currently only used for
 * processes that were forked to play sounds.  It's not needed for
 * forked DNS child, which have their own waitpid() call.  It might
 * be wise to move this code into gtksound.c.
 */
static void
clean_pid()
{
	int status;
	pid_t pid;

	do {
		pid = waitpid(-1, &status, WNOHANG);
	} while (pid != 0 && pid != (pid_t)-1);

	if ((pid == (pid_t) - 1) && (errno != ECHILD)) {
		char errmsg[BUFSIZ];
		snprintf(errmsg, BUFSIZ, "Warning: waitpid() returned %d", pid);
		perror(errmsg);
	}

	/* Restore signal catching */
	signal(SIGALRM, sighandler);
}

char *segfault_message;

static void
sighandler(int sig)
{
	switch (sig) {
	case SIGHUP:
		purple_debug_warning("sighandler", "Caught signal %d\n", sig);
		purple_connections_disconnect_all();
		break;
	case SIGSEGV:
		fprintf(stderr, "%s", segfault_message);
		abort();
		break;
	case SIGCHLD:
		/* Restore signal catching */
		signal(SIGCHLD, sighandler);
		alarm(1);
		break;
	case SIGALRM:
		clean_pid();
		break;
	default:
		purple_debug_warning("sighandler", "Caught signal %d\n", sig);
		purple_connections_disconnect_all();

		purple_plugins_unload_all();

		if (gtk_main_level())
			gtk_main_quit();
		exit(0);
	}
}
#endif

static int
ui_main()
{
#ifndef _WIN32
	GList *icons = NULL;
	GdkPixbuf *icon = NULL;
	char *icon_path;
	int i;
	const char *icon_sizes[] = {
		"16",
		"24",
		"32",
		"48"
	};

#endif

	pidgin_themes_init();

	pidgin_blist_setup_sort_methods();

#ifndef _WIN32
	/* use the nice PNG icon for all the windows */
	for(i=0; i<G_N_ELEMENTS(icon_sizes); i++) {
		icon_path = g_build_filename(DATADIR, "pixmaps", "pidgin", "icons", icon_sizes[i], "pidgin.png", NULL);
		icon = gdk_pixbuf_new_from_file(icon_path, NULL);
		g_free(icon_path);
		if (icon) {
			icons = g_list_append(icons,icon);
		} else {
			purple_debug_error("ui_main",
					"Failed to load the default window icon (%spx version)!\n", icon_sizes[i]);
		}
	}
	if(NULL == icons) {
		purple_debug_error("ui_main", "Unable to load any size of default window icon!\n");
	} else {
		gtk_window_set_default_icon_list(icons);

		g_list_foreach(icons, (GFunc)g_object_unref, NULL);
		g_list_free(icons);
	}
#endif

	return 0;
}

static void
debug_init(void)
{
	purple_debug_set_ui_ops(pidgin_debug_get_ui_ops());
	pidgin_debug_init();
}

static void
pidgin_ui_init(void)
{
	/* Set the UI operation structures. */
	purple_accounts_set_ui_ops(pidgin_accounts_get_ui_ops());
	purple_xfers_set_ui_ops(pidgin_xfers_get_ui_ops());
	purple_blist_set_ui_ops(pidgin_blist_get_ui_ops());
	purple_notify_set_ui_ops(pidgin_notify_get_ui_ops());
	purple_privacy_set_ui_ops(pidgin_privacy_get_ui_ops());
	purple_request_set_ui_ops(pidgin_request_get_ui_ops());
	purple_sound_set_ui_ops(pidgin_sound_get_ui_ops());
	purple_connections_set_ui_ops(pidgin_connections_get_ui_ops());
	purple_whiteboard_set_ui_ops(pidgin_whiteboard_get_ui_ops());
#ifdef USE_SCREENSAVER
	purple_idle_set_ui_ops(pidgin_idle_get_ui_ops());
#endif

	pidgin_stock_init();
	pidgin_account_init();
	pidgin_connection_init();
	pidgin_blist_init();
	pidgin_status_init();
	pidgin_conversations_init();
	pidgin_pounces_init();
	pidgin_privacy_init();
	pidgin_xfers_init();
	pidgin_roomlist_init();
	pidgin_log_init();
}

static void
pidgin_quit(void)
{
#ifdef USE_SM
	/* unplug */
	pidgin_session_end();
#endif

	/* Save the plugins we have loaded for next time. */
	pidgin_plugins_save();

	/* Uninit */
	pidgin_conversations_uninit();
	pidgin_status_uninit();
	pidgin_docklet_uninit();
	pidgin_blist_uninit();
	pidgin_connection_uninit();
	pidgin_account_uninit();
	pidgin_xfers_uninit();
	pidgin_debug_uninit();

	/* and end it all... */
	gtk_main_quit();
}

static PurpleCoreUiOps core_ops =
{
	pidgin_prefs_init,
	debug_init,
	pidgin_ui_init,
	pidgin_quit
};

static PurpleCoreUiOps *
pidgin_core_get_ui_ops(void)
{
	return &core_ops;
}

static void
show_usage(const char *name, gboolean terse)
{
	char *text;

	if (terse) {
		text = g_strdup_printf(_("%s %s. Try `%s -h' for more information.\n"), PIDGIN_NAME, VERSION, name);
	} else {
		text = g_strdup_printf(_("%s %s\n"
		       "Usage: %s [OPTION]...\n\n"
		       "  -c, --config=DIR    use DIR for config files\n"
		       "  -d, --debug         print debugging messages to stdout\n"
		       "  -h, --help          display this help and exit\n"
		       "  -n, --nologin       don't automatically login\n"
		       "  -l, --login[=NAME]  automatically login (optional argument NAME specifies\n"
		       "                      account(s) to use, separated by commas)\n"
		       "  -v, --version       display the current version and exit\n"), PIDGIN_NAME, VERSION, name);
	}

	purple_print_utf8_to_console(stdout, text);
	g_free(text);
}

#ifdef HAVE_STARTUP_NOTIFICATION
static void
sn_error_trap_push(SnDisplay *display, Display *xdisplay)
{
	gdk_error_trap_push();
}

static void
sn_error_trap_pop(SnDisplay *display, Display *xdisplay)
{
	gdk_error_trap_pop();
}

static void
startup_notification_complete(void)
{
	Display *xdisplay;

	xdisplay = GDK_DISPLAY();
	sn_display = sn_display_new(xdisplay,
								sn_error_trap_push,
								sn_error_trap_pop);
	sn_context =
		sn_launchee_context_new_from_environment(sn_display,
												 DefaultScreen(xdisplay));

	if (sn_context != NULL)
	{
		sn_launchee_context_complete(sn_context);
		sn_launchee_context_unref(sn_context);

		sn_display_unref(sn_display);
	}
}
#endif /* HAVE_STARTUP_NOTIFICATION */

/* FUCKING GET ME A TOWEL! */
#ifdef _WIN32
/* suppress gcc "no previous prototype" warning */
int pidgin_main(HINSTANCE hint, int argc, char *argv[]);
int pidgin_main(HINSTANCE hint, int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	gboolean opt_help = FALSE;
	gboolean opt_login = FALSE;
	gboolean opt_nologin = FALSE;
	gboolean opt_version = FALSE;
	char *opt_config_dir_arg = NULL;
	char *opt_login_arg = NULL;
	char *opt_session_arg = NULL;
	int dologin_ret = -1;
	char *search_path;
	GList *accounts;
#ifdef HAVE_SIGNAL_H
	int sig_indx;	/* for setting up signal catching */
	sigset_t sigset;
	RETSIGTYPE (*prev_sig_disp)(int);
	char errmsg[BUFSIZ];
#ifndef DEBUG
	char *segfault_message_tmp;
	GError *error = NULL;
#endif
#endif
	int opt;
	gboolean gui_check;
	gboolean debug_enabled;
	gboolean migration_failed = FALSE;

	struct option long_options[] = {
		{"config",   required_argument, NULL, 'c'},
		{"debug",    no_argument,       NULL, 'd'},
		{"help",     no_argument,       NULL, 'h'},
		{"login",    optional_argument, NULL, 'l'},
		{"nologin",  no_argument,       NULL, 'n'},
		{"session",  required_argument, NULL, 's'},
		{"version",  no_argument,       NULL, 'v'},
		{0, 0, 0, 0}
	};

#ifdef DEBUG
	debug_enabled = TRUE;
#else
	debug_enabled = FALSE;
#endif

#ifdef PURPLE_FATAL_ASSERTS
	/* Make g_return_... functions fatal. */
	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
#endif

#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

#ifdef HAVE_SETLOCALE
	/* Locale initialization is not complete here.  See gtk_init_check() */
	setlocale(LC_ALL, "");
#endif

#ifdef HAVE_SIGNAL_H

#ifndef DEBUG
		/* We translate this here in case the crash breaks gettext. */
		segfault_message_tmp = g_strdup_printf(_(
			"%s has segfaulted and attempted to dump a core file.\n"
			"This is a bug in the software and has happened through\n"
			"no fault of your own.\n\n"
			"If you can reproduce the crash, please notify the Pidgin\n"
			"developers by reporting a bug at\n"
			"%sbug.php\n\n"
			"Please make sure to specify what you were doing at the time\n"
			"and post the backtrace from the core file.  If you do not know\n"
			"how to get the backtrace, please read the instructions at\n"
			"%sgdb.php\n\n"
			"If you need further assistance, please IM either SeanEgn or \n"
			"LSchiere (via AIM).  Contact information for Sean and Luke \n"
			"on other protocols is at\n"
			"%scontactinfo.php\n"),
			PIDGIN_NAME, PURPLE_WEBSITE, PURPLE_WEBSITE, PURPLE_WEBSITE
		);

		/* we have to convert the message (UTF-8 to console
		   charset) early because after a segmentation fault
		   it's not a good practice to allocate memory */
		segfault_message = g_locale_from_utf8(segfault_message_tmp,
						      -1, NULL, NULL, &error);
		if (segfault_message != NULL) {
			g_free(segfault_message_tmp);
		}
		else {
			/* use 'segfault_message_tmp' (UTF-8) as a fallback */
			g_warning("%s\n", error->message);
			g_error_free(error);
			segfault_message = segfault_message_tmp;
		}
#else
		/* Don't mark this for translation. */
		segfault_message = g_strdup(
			"Hi, user.  We need to talk.\n"
			"I think something's gone wrong here.  It's probably my fault.\n"
			"No, really, it's not you... it's me... no no no, I think we get along well\n"
			"it's just that.... well, I want to see other people.  I... what?!?  NO!  I \n"
			"haven't been cheating on you!!  How many times do you want me to tell you?!  And\n"
			"for the last time, it's just a rash!\n"
		);
#endif

	/* Let's not violate any PLA's!!!! */
	/* jseymour: whatever the fsck that means */
	/* Robot101: for some reason things like gdm like to block     *
	 * useful signals like SIGCHLD, so we unblock all the ones we  *
	 * declare a handler for. thanks JSeymour and Vann.            */
	if (sigemptyset(&sigset)) {
		snprintf(errmsg, BUFSIZ, "Warning: couldn't initialise empty signal set");
		perror(errmsg);
	}
	for(sig_indx = 0; catch_sig_list[sig_indx] != -1; ++sig_indx) {
		if((prev_sig_disp = signal(catch_sig_list[sig_indx], sighandler)) == SIG_ERR) {
			snprintf(errmsg, BUFSIZ, "Warning: couldn't set signal %d for catching",
				catch_sig_list[sig_indx]);
			perror(errmsg);
		}
		if(sigaddset(&sigset, catch_sig_list[sig_indx])) {
			snprintf(errmsg, BUFSIZ, "Warning: couldn't include signal %d for unblocking",
				catch_sig_list[sig_indx]);
			perror(errmsg);
		}
	}
	for(sig_indx = 0; ignore_sig_list[sig_indx] != -1; ++sig_indx) {
		if((prev_sig_disp = signal(ignore_sig_list[sig_indx], SIG_IGN)) == SIG_ERR) {
			snprintf(errmsg, BUFSIZ, "Warning: couldn't set signal %d to ignore",
				ignore_sig_list[sig_indx]);
			perror(errmsg);
		}
	}

	if (sigprocmask(SIG_UNBLOCK, &sigset, NULL)) {
		snprintf(errmsg, BUFSIZ, "Warning: couldn't unblock signals");
		perror(errmsg);
	}
#endif

	/* scan command-line options */
	opterr = 1;
	while ((opt = getopt_long(argc, argv,
#ifndef _WIN32
				  "c:dhnl::s:v",
#else
				  "c:dhnl::v",
#endif
				  long_options, NULL)) != -1) {
		switch (opt) {
		case 'c':	/* config dir */
			g_free(opt_config_dir_arg);
			opt_config_dir_arg = g_strdup(optarg);
			break;
		case 'd':	/* debug */
			debug_enabled = TRUE;
			break;
		case 'h':	/* help */
			opt_help = TRUE;
			break;
		case 'n':	/* no autologin */
			opt_nologin = TRUE;
			break;
		case 'l':	/* login, option username */
			opt_login = TRUE;
			g_free(opt_login_arg);
			if (optarg != NULL)
				opt_login_arg = g_strdup(optarg);
			break;
		case 's':	/* use existing session ID */
			g_free(opt_session_arg);
			opt_session_arg = g_strdup(optarg);
			break;
		case 'v':	/* version */
			opt_version = TRUE;
			break;
		case '?':	/* show terse help */
		default:
			show_usage(argv[0], TRUE);
#ifdef HAVE_SIGNAL_H
			g_free(segfault_message);
#endif
			return 0;
			break;
		}
	}

	/* show help message */
	if (opt_help) {
		show_usage(argv[0], FALSE);
#ifdef HAVE_SIGNAL_H
		g_free(segfault_message);
#endif
		return 0;
	}
	/* show version message */
	if (opt_version) {
		printf(PIDGIN_NAME " %s\n", VERSION);
#ifdef HAVE_SIGNAL_H
		g_free(segfault_message);
#endif
		return 0;
	}

	/* set a user-specified config directory */
	if (opt_config_dir_arg != NULL) {
		purple_util_set_user_dir(opt_config_dir_arg);
	}

	/*
	 * We're done piddling around with command line arguments.
	 * Fire up this baby.
	 */

	purple_debug_set_enabled(debug_enabled);

	/* If we're using a custom configuration directory, we
	 * do NOT want to migrate, or weird things will happen. */
	if (opt_config_dir_arg == NULL)
	{
		if (!purple_core_migrate())
		{
			migration_failed = TRUE;
		}
	}

	search_path = g_build_filename(purple_user_dir(), "gtkrc-2.0", NULL);
	gtk_rc_add_default_file(search_path);
	g_free(search_path);

	gui_check = gtk_init_check(&argc, &argv);
	if (!gui_check) {
		char *display = gdk_get_display();

		printf(PIDGIN_NAME " %s\n", VERSION);

		g_warning("cannot open display: %s", display ? display : "unset");
		g_free(display);
#ifdef HAVE_SIGNAL_H
		g_free(segfault_message);
#endif

		return 1;
	}

#ifdef _WIN32
	winpidgin_init(hint);
#endif

	if (migration_failed)
	{
		char *old = g_strconcat(purple_home_dir(),
		                        G_DIR_SEPARATOR_S ".gaim", NULL);
		const char *text = _(
			"Pidgin encountered errors migrating your settings "
			"from %s to %s. Please investigate and complete the "
			"migration by hand.");
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(NULL,
		                                0,
		                                GTK_MESSAGE_ERROR,
		                                GTK_BUTTONS_CLOSE,
		                                text, old, purple_user_dir());
		g_free(old);

		g_signal_connect_swapped(dialog, "response",
		                         G_CALLBACK(gtk_main_quit), NULL);

		gtk_widget_show_all(dialog);

		gtk_main();

#ifdef HAVE_SIGNAL_H
		g_free(segfault_message);
#endif
		return 0;
	}

	purple_core_set_ui_ops(pidgin_core_get_ui_ops());
	purple_eventloop_set_ui_ops(pidgin_eventloop_get_ui_ops());

	/*
	 * Set plugin search directories. Give priority to the plugins
	 * in user's home directory.
	 */
	search_path = g_build_filename(purple_user_dir(), "plugins", NULL);
	purple_plugins_add_search_path(search_path);
	g_free(search_path);
	purple_plugins_add_search_path(LIBDIR);

	if (!purple_core_init(PIDGIN_UI)) {
		fprintf(stderr,
				"Initialization of the " PIDGIN_NAME " core failed. Dumping core.\n"
				"Please report this!\n");
#ifdef HAVE_SIGNAL_H
		g_free(segfault_message);
#endif
		abort();
	}

	/* TODO: Move blist loading into purple_blist_init() */
	purple_set_blist(purple_blist_new());
	purple_blist_load();

	/* TODO: Move prefs loading into purple_prefs_init() */
	purple_prefs_load();
	purple_prefs_update_old();
	pidgin_prefs_update_old();

	/* load plugins we had when we quit */
	purple_plugins_load_saved(PIDGIN_PREFS_ROOT "/plugins/loaded");
	pidgin_docklet_init();

	/* TODO: Move pounces loading into purple_pounces_init() */
	purple_pounces_load();


	/* HACK BY SEANEGAN:
	 * We've renamed prpl-oscar to prpl-aim and prpl-icq, accordingly.
	 * Let's do that change right here... after everything's loaded, but
	 * before anything has happened
	 */
	for (accounts = purple_accounts_get_all(); accounts != NULL; accounts = accounts->next) {
		PurpleAccount *account = accounts->data;
		if (!strcmp(purple_account_get_protocol_id(account), "prpl-oscar")) {
			if (isdigit(*purple_account_get_username(account)))
				purple_account_set_protocol_id(account, "prpl-icq");
			else
				purple_account_set_protocol_id(account, "prpl-aim");
		}
	}

	ui_main();

#ifdef USE_SM
	pidgin_session_init(argv[0], opt_session_arg, opt_config_dir_arg);
#endif
	if (opt_session_arg != NULL) {
		g_free(opt_session_arg);
		opt_session_arg = NULL;
	}
	if (opt_config_dir_arg != NULL) {
		g_free(opt_config_dir_arg);
		opt_config_dir_arg = NULL;
	}

	/*
	 * We want to show the blist early in the init process so the
	 * user feels warm and fuzzy (not cold and prickley).
	 */
	purple_blist_show();

	if (purple_prefs_get_bool(PIDGIN_PREFS_ROOT "/debug/enabled"))
		pidgin_debug_window_show();

	if (opt_login) {
		dologin_ret = dologin_named(opt_login_arg);
		if (opt_login_arg != NULL) {
			g_free(opt_login_arg);
			opt_login_arg = NULL;
		}
	}

	if (opt_nologin)
	{
		/* Set all accounts to "offline" */
		PurpleSavedStatus *saved_status;

		/* If we've used this type+message before, lookup the transient status */
		saved_status = purple_savedstatus_find_transient_by_type_and_message(
							PURPLE_STATUS_OFFLINE, NULL);

		/* If this type+message is unique then create a new transient saved status */
		if (saved_status == NULL)
			saved_status = purple_savedstatus_new(NULL, PURPLE_STATUS_OFFLINE);

		/* Set the status for each account */
		purple_savedstatus_activate(saved_status);
	}
	else
	{
		/* Everything is good to go--sign on already */
		if (!purple_prefs_get_bool("/core/savedstatus/startup_current_status"))
			purple_savedstatus_activate(purple_savedstatus_get_startup());
		purple_accounts_restore_current_statuses();
	}

	if ((accounts = purple_accounts_get_all_active()) == NULL)
	{
		pidgin_accounts_window_show();
	}
	else
	{
		g_list_free(accounts);
	}

#ifdef HAVE_STARTUP_NOTIFICATION
	startup_notification_complete();
#endif

#ifdef _WIN32
	winpidgin_post_init();
#endif

	gtk_main();

#ifdef HAVE_SIGNAL_H
	g_free(segfault_message);
#endif

#ifdef _WIN32
	winpidgin_cleanup();
#endif

	return 0;
}
