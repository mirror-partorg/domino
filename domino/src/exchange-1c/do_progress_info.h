
#ifndef DO_PROGRESS_INFO_H
#define DO_PROGRESS_INFO_H

#include <gtk/gtk.h>

#define DO_TYPE_PROGRESS_INFO         (do_progress_info_get_type ())
#define DO_PROGRESS_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PROGRESS_INFO, DoProgressInfo))
#define DO_PROGRESS_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PROGRESS_INFO, DoProgressInfoClass))
#define DO_IS_PROGRESS_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PROGRESS_INFO))
#define DO_IS_PROGRESS_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PROGRESS_INFO))
#define DO_PROGRESS_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PROGRESS_INFO, DoProgressInfoClass))

typedef struct _DoProgressInfo      DoProgressInfo;
typedef struct _DoProgressInfoClass DoProgressInfoClass;

GType do_progress_info_get_type (void) G_GNUC_CONST;


DoProgressInfo *do_progress_info_new (void);

GList *       do_get_all_progress_info (void);

char *        do_progress_info_get_status      (DoProgressInfo *info);
char *        do_progress_info_get_details     (DoProgressInfo *info);
double        do_progress_info_get_progress    (DoProgressInfo *info);
GCancellable *do_progress_info_get_cancellable (DoProgressInfo *info);
void          do_progress_info_cancel          (DoProgressInfo *info);
gboolean      do_progress_info_get_is_started  (DoProgressInfo *info);
gboolean      do_progress_info_get_is_finished (DoProgressInfo *info);
gboolean      do_progress_info_get_is_paused   (DoProgressInfo *info);
GtkWidget    *do_progress_info_get_window  (DoProgressInfo *info);


void          do_progress_info_start           (DoProgressInfo *info);
void          do_progress_info_finish          (DoProgressInfo *info);
void          do_progress_info_pause           (DoProgressInfo *info);
void          do_progress_info_resume          (DoProgressInfo *info);
void          do_progress_info_set_status      (DoProgressInfo *info,
						      const char           *status);
void          do_progress_info_take_status     (DoProgressInfo *info,
						      char                 *status);
void          do_progress_info_set_details     (DoProgressInfo *info,
						      const char           *details);
void          do_progress_info_take_details    (DoProgressInfo *info,
						      char                 *details);
void          do_progress_info_set_progress    (DoProgressInfo *info,
						      double                current,
						      double                total);
void          do_progress_info_pulse_progress  (DoProgressInfo *info);



#endif /* DO_PROGRESS_INFO_H */
