/**
 * @file gtkwebview.h Wrapper over the Gtk WebKitWebView component
 * @ingroup pidgin
 */

/* pidgin
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 *
 */

#ifndef _PIDGIN_WEBVIEW_H_
#define _PIDGIN_WEBVIEW_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#define GTK_TYPE_WEBVIEW            (gtk_webview_get_type())
#define GTK_WEBVIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_WEBVIEW, GtkWebView))
#define GTK_WEBVIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_WEBVIEW, GtkWebViewClass))
#define GTK_IS_WEBVIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_WEBVIEW))
#define GTK_IS_WEBVIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_WEBVIEW))
#define GTK_WEBVIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_WEBVIEW, GtkWebViewClass))

typedef enum {
	GTK_WEBVIEW_BOLD          = 1 << 0,
	GTK_WEBVIEW_ITALIC        = 1 << 1,
	GTK_WEBVIEW_UNDERLINE     = 1 << 2,
	GTK_WEBVIEW_GROW          = 1 << 3,
	GTK_WEBVIEW_SHRINK        = 1 << 4,
	GTK_WEBVIEW_FACE          = 1 << 5,
	GTK_WEBVIEW_FORECOLOR     = 1 << 6,
	GTK_WEBVIEW_BACKCOLOR     = 1 << 7,
	GTK_WEBVIEW_LINK          = 1 << 8,
	GTK_WEBVIEW_IMAGE         = 1 << 9,
	GTK_WEBVIEW_SMILEY        = 1 << 10,
	GTK_WEBVIEW_LINKDESC      = 1 << 11,
	GTK_WEBVIEW_STRIKE        = 1 << 12,
	/** Show custom smileys when appropriate. */
	GTK_WEBVIEW_CUSTOM_SMILEY = 1 << 13,
	GTK_WEBVIEW_ALL           = -1
} GtkWebViewButtons;

typedef enum {
	GTK_WEBVIEW_SMILEY_CUSTOM = 1 << 0
} GtkWebViewSmileyFlags;

typedef enum {
	GTK_WEBVIEW_ACTION_BOLD,
	GTK_WEBVIEW_ACTION_ITALIC,
	GTK_WEBVIEW_ACTION_UNDERLINE,
	GTK_WEBVIEW_ACTION_STRIKE,
	GTK_WEBVIEW_ACTION_LARGER,
#if 0
	GTK_WEBVIEW_ACTION_NORMAL,
#endif
	GTK_WEBVIEW_ACTION_SMALLER,
	GTK_WEBVIEW_ACTION_FONTFACE,
	GTK_WEBVIEW_ACTION_FGCOLOR,
	GTK_WEBVIEW_ACTION_BGCOLOR,
	GTK_WEBVIEW_ACTION_CLEAR,
	GTK_WEBVIEW_ACTION_IMAGE,
	GTK_WEBVIEW_ACTION_LINK,
	GTK_WEBVIEW_ACTION_HR,
	GTK_WEBVIEW_ACTION_SMILEY,
	GTK_WEBVIEW_ACTION_ATTENTION
} GtkWebViewAction;

typedef struct _GtkWebView GtkWebView;
typedef struct _GtkWebViewClass GtkWebViewClass;
typedef struct _GtkWebViewSmiley GtkWebViewSmiley;

struct _GtkWebView
{
	WebKitWebView parent;
};

struct _GtkWebViewClass
{
	WebKitWebViewClass parent;

	GList *protocols;

	void (*buttons_update)(GtkWebView *, GtkWebViewButtons);
	void (*toggle_format)(GtkWebView *, GtkWebViewButtons);
	void (*clear_format)(GtkWebView *);
	void (*update_format)(GtkWebView *);
	void (*changed)(GtkWebView *);
	void (*html_appended)(GtkWebView *, WebKitDOMRange *);
};

G_BEGIN_DECLS

/**
 * Returns the GType for a GtkWebView widget
 *
 * @return The GType for GtkWebView widget
 */
GType gtk_webview_get_type(void);

/**
 * Create a new GtkWebView object
 *
 * @param editable Whether this GtkWebView will be user-editable
 *
 * @return A GtkWidget corresponding to the GtkWebView object
 */
GtkWidget *gtk_webview_new(gboolean editable);

/**
 * A very basic routine to append html, which can be considered
 * equivalent to a "document.write" using JavaScript.
 *
 * @param webview The GtkWebView object
 * @param markup  The html markup to append
 */
void gtk_webview_append_html(GtkWebView *webview, const char *markup);

/**
 * Requests loading of the given content.
 *
 * @param webview The GtkWebView object
 * @param html    The HTML content to load
 */
void gtk_webview_load_html_string(GtkWebView *webview, const char *html);

/**
 * Requests loading of the given content and sets the selection. You must
 * include an anchor tag with id='caret' in the HTML string, which will be
 * used to set the selection. This tag is then removed so that querying the
 * WebView's HTML contents will no longer return it.
 *
 * @param webview The GtkWebView object
 * @param html    The HTML content to load
 */
void gtk_webview_load_html_string_with_selection(GtkWebView *webview, const char *html);

/**
 * Execute the JavaScript only after the webkit_webview_load_string
 * loads completely. We also guarantee that the scripts are executed
 * in the order they are called here. This is useful to avoid race
 * conditions when calling JS functions immediately after opening the
 * page.
 *
 * @param webview The GtkWebView object
 * @param script  The script to execute
 */
void gtk_webview_safe_execute_script(GtkWebView *webview, const char *script);

/**
 * A convenience routine to quote a string for use as a JavaScript
 * string. For instance, "hello 'world'" becomes "'hello \\'world\\''"
 *
 * @param str The string to escape and quote
 *
 * @return The quoted string
 */
char *gtk_webview_quote_js_string(const char *str);

/**
 * Set the vertical adjustment for the GtkWebView.
 *
 * @param webview  The GtkWebView object
 * @param vadj     The GtkAdjustment that control the webview
 */
void gtk_webview_set_vadjustment(GtkWebView *webview, GtkAdjustment *vadj);

/**
 * Scrolls the Webview to the end of its contents.
 *
 * @param webview The GtkWebView object
 * @param smooth  A boolean indicating if smooth scrolling should be used
 */
void gtk_webview_scroll_to_end(GtkWebView *webview, gboolean smooth);

/**
 * Set whether the GtkWebView stays at its end when HTML content is appended. If
 * not already at the end before appending, then scrolling will not occur.
 *
 * @param webview The GtkWebView object
 * @param scroll  Whether to automatically scroll
 */
void gtk_webview_set_autoscroll(GtkWebView *webview, gboolean scroll);

/**
 * Set whether the GtkWebView stays at its end when HTML content is appended. If
 * not already at the end before appending, then scrolling will not occur.
 *
 * @param webview The GtkWebView object
 *
 * @return Whether to automatically scroll
 */
gboolean gtk_webview_get_autoscroll(GtkWebView *webview);

/**
 * Scrolls a GtkWebView up by one page.
 *
 * @param webview The GtkWebView.
 */
void gtk_webview_page_up(GtkWebView *webview);

/**
 * Scrolls a GtkWebView down by one page.
 *
 * @param webview The GtkWebView.
 */
void gtk_webview_page_down(GtkWebView *webview);

/**
 * Setup formatting for a GtkWebView depending on the flags specified.
 *
 * @param webview The GtkWebView.
 * @param flags   The connection flags describing the allowed formatting.
 */
void gtk_webview_setup_entry(GtkWebView *webview, PurpleConnectionFlags flags);

/**
 * Setup spell-checking on a GtkWebView.
 *
 * @param webview The GtkWebView.
 * @param enable  Whether to enable or disable spell-checking.
 */
void pidgin_webview_set_spellcheck(GtkWebView *webview, gboolean enable);

/**
 * Enables or disables whole buffer formatting only (wbfo) in a GtkWebView.
 * In this mode formatting options to the buffer take effect for the entire
 * buffer instead of specific text.
 *
 * @param webview The GtkWebView
 * @param wbfo    @c TRUE to enable the mode, or @c FALSE otherwise.
 */
void gtk_webview_set_whole_buffer_formatting_only(GtkWebView *webview,
                                                  gboolean wbfo);

/**
 * Indicates which formatting functions to enable and disable in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param buttons A GtkWebViewButtons bitmask indicating which functions to use
 */
void gtk_webview_set_format_functions(GtkWebView *webview,
                                      GtkWebViewButtons buttons);

/**
 * Activates a WebKitDOMHTMLAnchorElement object. This triggers the navigation
 * signals, and marks the link as visited (when possible).
 *
 * @param link   The WebKitDOMHTMLAnchorElement object
 *
 */
void gtk_webview_activate_anchor(WebKitDOMHTMLAnchorElement *link);

/**
 * Register a protocol with the GtkWebView widget. Registering a protocol would
 * allow certain text to be clickable.
 *
 * @param name      The name of the protocol (e.g. http://)
 * @param activate  The callback to trigger when the protocol text is clicked.
 *                  Removes any current protocol definition if @c NULL. The
 *                  callback should return @c TRUE if the link was activated
 *                  properly, @c FALSE otherwise.
 * @param context_menu  The callback to trigger when the context menu is popped
 *                      up on the protocol text. The callback should return
 *                      @c TRUE if the request for context menu was processed
 *                      successfully, @c FALSE otherwise.
 *
 * @return  @c TRUE if the protocol was successfully registered
 *          (or unregistered, when \a activate is @c NULL)
 */
gboolean gtk_webview_class_register_protocol(const char *name,
		gboolean (*activate)(GtkWebView *webview, const char *uri),
		gboolean (*context_menu)(GtkWebView *webview, WebKitDOMHTMLAnchorElement *link, GtkWidget *menu));

/**
 * Returns which formatting functions are enabled in a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return A GtkWebViewButtons bitmask indicating which functions to are enabled
 */
GtkWebViewButtons gtk_webview_get_format_functions(GtkWebView *webview);

/**
 * Sets each boolean to @c TRUE or @c FALSE to indicate if that formatting
 * option is enabled at the current position in a GtkWebView.
 *
 * @param webview       The GtkWebView
 * @param bold          The boolean to set for bold or @c NULL.
 * @param italic        The boolean to set for italic or @c NULL.
 * @param underline     The boolean to set for underline or @c NULL.
 * @param strikethrough The boolean to set for strikethrough or @c NULL.
 */
void gtk_webview_get_current_format(GtkWebView *webview, gboolean *bold,
                                    gboolean *italic, gboolean *underline,
                                    gboolean *strike);

/**
 * Returns a string containing the selected font face at the current position
 * in a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return A string containing the font face or @c NULL if none is set.
 */
char *gtk_webview_get_current_fontface(GtkWebView *webview);

/**
 * Returns a string containing the selected foreground color at the current
 * position in a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return A string containing the foreground color or @c NULL if none is set.
 */
char *gtk_webview_get_current_forecolor(GtkWebView *webview);

/**
 * Returns a string containing the selected font background color at the current
 * position in a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return A string containing the background color or @c NULL if none is set.
 */
char *gtk_webview_get_current_backcolor(GtkWebView *webview);

/**
 * Returns a integer containing the selected HTML font size at the current
 * position in a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return The HTML font size.
 */
gint gtk_webview_get_current_fontsize(GtkWebView *webview);

/**
 * Gets the content of the head element of a GtkWebView as HTML.
 *
 * @param webview The GtkWebView
 *
 * @return The HTML from the head element.
 */
gchar *gtk_webview_get_head_html(GtkWebView *webview);

/**
 * Gets the HTML content of a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return The HTML that is currently displayed.
 */
gchar *gtk_webview_get_body_html(GtkWebView *webview);

/**
 * Gets the text content of a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return The HTML-free text that is currently displayed.
 */
gchar *gtk_webview_get_body_text(GtkWebView *webview);

/**
 * Gets the selected text of a GtkWebView.
 *
 * @param webview The GtkWebView
 *
 * @return The HTML-free text that is currently selected, or NULL if nothing is
 *         currently selected.
 */
gchar *gtk_webview_get_selected_text(GtkWebView *webview);

/**
 * Clear all the formatting on a GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_clear_formatting(GtkWebView *webview);

/**
 * Toggles bold at the cursor location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_toggle_bold(GtkWebView *webview);

/**
 * Toggles italic at the cursor location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_toggle_italic(GtkWebView *webview);

/**
 * Toggles underline at the cursor location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_toggle_underline(GtkWebView *webview);

/**
 * Toggles strikethrough at the cursor location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_toggle_strike(GtkWebView *webview);

/**
 * Toggles a foreground color at the current location or selection in a
 * GtkWebView.
 *
 * @param webview The GtkWebView
 * @param color  The HTML-style color, or @c NULL or "" to clear the color.
 *
 * @return @c TRUE if a color was set, or @c FALSE if it was cleared.
 */
gboolean gtk_webview_toggle_forecolor(GtkWebView *webview, const char *color);

/**
 * Toggles a background color at the current location or selection in a
 * GtkWebView.
 *
 * @param webview The GtkWebView
 * @param color  The HTML-style color, or @c NULL or "" to clear the color.
 *
 * @return @c TRUE if a color was set, or @c FALSE if it was cleared.
 */
gboolean gtk_webview_toggle_backcolor(GtkWebView *webview, const char *color);

/**
 * Toggles a font face at the current location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param face   The font face name, or @c NULL or "" to clear the font.
 *
 * @return @c TRUE if a font name was set, or @c FALSE if it was cleared.
 */
gboolean gtk_webview_toggle_fontface(GtkWebView *webview, const char *face);

/**
 * Sets the font size at the current location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param size   The HTML font size to use.
 */
void gtk_webview_font_set_size(GtkWebView *webview, gint size);

/**
 * Decreases the font size by 1 at the current location or selection in a
 * GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_font_shrink(GtkWebView *webview);

/**
 * Increases the font size by 1 at the current location or selection in a
 * GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_font_grow(GtkWebView *webview);

/**
 * Inserts a horizontal rule at the current location or selection in a
 * GtkWebView.
 *
 * @param webview The GtkWebView
 */
void gtk_webview_insert_hr(GtkWebView *webview);

/**
 * Inserts a link at the current location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param url     The URL of the link
 * @param desc    The text description of the link. If not supplied, the URL is
 *                used instead.
 */
void gtk_webview_insert_link(GtkWebView *webview, const char *url, const char *desc);

/**
 * Inserts an image at the current location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param id      The PurpleStoredImage id
 */
void gtk_webview_insert_image(GtkWebView *webview, int id);

/**
 * Gets the protocol name associated with this GtkWebView.
 *
 * @param webview The GtkWebView
 */
const char *gtk_webview_get_protocol_name(GtkWebView *webview);

/**
 * Associates a protocol name with a GtkWebView.
 *
 * @param webview       The GtkWebView
 * @param protocol_name The protocol name to associate with the GtkWebView
 */
void gtk_webview_set_protocol_name(GtkWebView *webview, const char *protocol_name);

/**
 * Create a new GtkWebViewSmiley.
 *
 * @param file      The image file for the smiley
 * @param shortcut  The key shortcut for the smiley
 * @param hide      @c TRUE if the smiley should be hidden in the smiley dialog,
 *                  @c FALSE otherwise
 * @param flags     The smiley flags
 *
 * @return The newly created smiley
 */
GtkWebViewSmiley *gtk_webview_smiley_create(const char *file,
                                            const char *shortcut,
                                            gboolean hide,
                                            GtkWebViewSmileyFlags flags);

/**
 * Reload the image data for the smiley.
 *
 * @param smiley    The smiley to reload
 */
void gtk_webview_smiley_reload(GtkWebViewSmiley *smiley);

/**
 * Destroy a GtkWebViewSmiley.
 *
 * @param smiley    The smiley to destroy
 */
void gtk_webview_smiley_destroy(GtkWebViewSmiley *smiley);

/**
 * Returns the text associated with a smiley.
 *
 * @param smiley    The smiley
 *
 * @return The text
 */
const char *gtk_webview_smiley_get_smile(const GtkWebViewSmiley *smiley);

/**
 * Returns the file associated with a smiley.
 *
 * @param smiley    The smiley
 *
 * @return The file
 */
const char *gtk_webview_smiley_get_file(const GtkWebViewSmiley *smiley);

/**
 * Returns the invisibility of a smiley.
 *
 * @param smiley    The smiley
 *
 * @return The hidden status
 */
gboolean gtk_webview_smiley_get_hidden(const GtkWebViewSmiley *smiley);

/**
 * Returns the flags associated with a smiley.
 *
 * @param smiley    The smiley
 *
 * @return The flags
 */
GtkWebViewSmileyFlags gtk_webview_smiley_get_flags(const GtkWebViewSmiley *smiley);

/**
 * Returns the smiley object associated with the text.
 *
 * @param webview The GtkWebView
 * @param sml     The name of the smiley category
 * @param text    The text associated with the smiley
 */
GtkWebViewSmiley *gtk_webview_smiley_find(GtkWebView *webview, const char *sml,
                                          const char *text);

/**
 * Associates a smiley with a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param sml     The name of the smiley category
 * @param smiley  The GtkWebViewSmiley to associate
 */
void gtk_webview_associate_smiley(GtkWebView *webview, const char *sml,
                                  GtkWebViewSmiley *smiley);

/**
 * Removes all smileys associated with a GtkWebView.
 *
 * @param webview The GtkWebView.
 */
void gtk_webview_remove_smileys(GtkWebView *webview);

/**
 * Inserts a smiley at the current location or selection in a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param sml     The category of the smiley
 * @param smiley  The text of the smiley to insert
 */
void gtk_webview_insert_smiley(GtkWebView *webview, const char *sml,
                               const char *smiley);

/**
 * Makes the toolbar associated with a GtkWebView visible.
 *
 * @param webview The GtkWebView.
 */
void gtk_webview_show_toolbar(GtkWebView *webview);

/**
 * Makes the toolbar associated with a GtkWebView invisible.
 *
 * @param webview The GtkWebView.
 */
void gtk_webview_hide_toolbar(GtkWebView *webview);

/**
 * Activate an action on the toolbar associated with a GtkWebView.
 *
 * @param webview The GtkWebView
 * @param action  The GtkWebViewAction
 */
void gtk_webview_activate_toolbar(GtkWebView *webview, GtkWebViewAction action);

/* Do not use. */
void
gtk_webview_set_toolbar(GtkWebView *webview, GtkWidget *toolbar);

G_END_DECLS

#endif /* _PIDGIN_WEBVIEW_H_ */
