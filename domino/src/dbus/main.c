#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>

int main()
{

  DBusGConnection *connection;
  GError *error;
  DBusGProxy *proxy;

  g_type_init ();

  error = NULL;
  connection = dbus_g_bus_get(DBUS_BUS_SYSTEM,NULL);

  if (connection == NULL)
    {
      g_printerr ("Failed to open connection to bus: %s\n",
                  error->message);
      g_error_free (error);
      exit (1);
    }


  /* Create a proxy object for the "bus driver" (name "org.freedesktop.DBus") */
     proxy=dbus_g_proxy_new_for_name(connection,"org.freedesktop.UDisks2","/org/freedesktop/UDisks2/drives/ST1500DM003_9YN16G_W2F087KW","org.freedesktop.UDisks2");

     if(proxy == NULL)
     {
        g_printerr ("Failed To Create A proxy...: %s\n", error->message);
        g_error_free (error);
        exit(1);
     }
     else
       printf("Probably got a connection to the correct interface...\n");

    printf("%s\n", dbus_g_proxy_get_interface(proxy));


     return 0;
}
