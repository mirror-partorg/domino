#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#if GTK_MAJOR_VERSION == 2
#include <gdk/gdkkeysyms.h>
#endif

#define APPLICATION_NAME "price"
#define ORGANIZAION_NAME "glekar"

#define MESSAGE(...) g_log(APPLICATION_NAME,G_LOG_LEVEL_MESSAGE,__VA_ARGS__)
#define ERROR(...) g_log(APPLICATION_NAME,G_LOG_LEVEL_ERROR,__VA_ARGS__)
#define MESSAGE_WITH_TIME(...) {gchar *str=g_strdup_printf(__VA_ARGS__); time_t t=time(NULL); gchar *s = ctime(&t); gchar *msg = g_strdup_printf("%s%s",s,str); g_free(str); g_log(APPLICATION_NAME,G_LOG_LEVEL_MESSAGE,"%s", msg); g_free(msg);}

#ifndef GDK_KEY_BackSpace
#define GDK_KEY_BackSpace GDK_BackSpace
#endif
#ifndef GDK_KEY_Escape
#define GDK_KEY_Escape GDK_Escape
#endif
#ifndef GDK_KEY_Up
#define GDK_KEY_Up GDK_Up
#endif
#ifndef GDK_KEY_Down
#define GDK_KEY_Down GDK_Down
#endif
#ifndef GDK_KEY_Page_Up
#define GDK_KEY_Page_Up GDK_Page_Up
#endif
#ifndef GDK_KEY_Page_Down
#define GDK_KEY_Page_Down GDK_Page_Down
#endif
#ifndef GDK_KEY_Return
#define GDK_KEY_Return GDK_Return
#endif
#ifndef GDK_KEY_KP_Enter
#define GDK_KEY_KP_Enter GDK_KP_Enter
#endif
#ifndef GDK_KEY_ISO_Enter
#define GDK_KEY_ISO_Enter GDK_ISO_Enter
#endif
#ifndef GDK_KEY_Right
#define GDK_KEY_Right GDK_Right
#endif
#ifndef GDK_KEY_Left
#define GDK_KEY_Left GDK_Left
#endif
#ifndef GDK_KEY_F12
#define GDK_KEY_F12 GDK_F12
#endif
#ifndef GDK_KEY_F4
#define GDK_KEY_F4 GDK_F4
#endif

#endif // CONFIG_H_INCLUDED
