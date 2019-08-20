/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * ssconvert.c: A wrapper application to convert spreadsheet formats
 *
 * Author:
 *   Jon K�re Hellan <hellan@acm.org>
 *   Morten Welinder <terra@gnome.org>
 *   Jody Goldberg <jody@gnome.org>
 *
 * Copyright (C) 2002-2003 Jody Goldberg
 * Copyright (C) 2006-2009 Morten Welinder (terra@gnome.org)
 */

#include <glib/gi18n.h>
#include <gnumeric.h>
#include <workbook.h>
#include <workbook-priv.h>
#include <workbook-control.h>
#include <sheet.h>
#include <dependent.h>
#include <expr-name.h>
#include <libgnumeric.h>
#include <gutils.h>
//#include <gnumeric-paths.h>
#include <gnm-plugin.h>
#include <command-context.h>
#include <command-context-stderr.h>
#include <workbook-view.h>
//#include <dialogs/dialogs.h>
#include <goffice/goffice.h>
#include <gsf/gsf-utils.h>
#include <string.h>

static gboolean ssconvert_show_version = FALSE;
static gboolean ssconvert_list_exporters = FALSE;
static gboolean ssconvert_list_importers = FALSE;
static gboolean ssconvert_one_file_per_sheet = FALSE;
static gboolean ssconvert_recalc = FALSE;
static gboolean ssconvert_solve = FALSE;
static char *ssconvert_range = NULL;
static char *ssconvert_import_encoding = NULL;
static char *ssconvert_import_id = NULL;
static char *ssconvert_export_id = NULL;
static char *ssconvert_export_options = NULL;
static char *ssconvert_merge_target = NULL;
static char **ssconvert_goal_seek = NULL;

static const GOptionEntry ssconvert_options [] = {
	{
		"version", 'v',
		0, G_OPTION_ARG_NONE, &ssconvert_show_version,
		N_("Display program version"),
		NULL
	},

	/* ---------------------------------------- */

	{
		"import-encoding", 'E',
		0, G_OPTION_ARG_STRING, &ssconvert_import_encoding,
		N_("Optionally specify an encoding for imported content"),
		N_("ENCODING")
	},

	{
		"import-type", 'I',
		0, G_OPTION_ARG_STRING, &ssconvert_import_id,
		N_("Optionally specify which importer to use"),
		N_("ID")
	},

	{
		"list-importers", 0,
		0, G_OPTION_ARG_NONE, &ssconvert_list_importers,
		N_("List the available importers"),
		NULL
	},

	/* ---------------------------------------- */

	{
		"merge-to", 'M',
		0, G_OPTION_ARG_STRING, &ssconvert_merge_target,
		N_("Merge listed files (all same format) to make this file"),
		N_("file")
	},

	{
		"export-type", 'T',
		0, G_OPTION_ARG_STRING, &ssconvert_export_id,
		N_("Optionally specify which exporter to use"),
		N_("ID")
	},

	{
		"export-options", 'O',
		0, G_OPTION_ARG_STRING, &ssconvert_export_options,
		N_("Detailed instructions for the chosen exporter"),
		N_("string")
	},

	{
		"list-exporters", 0,
		0, G_OPTION_ARG_NONE, &ssconvert_list_exporters,
		N_("List the available exporters"),
		NULL
	},

	{
		"export-file-per-sheet", 'S',
		0, G_OPTION_ARG_NONE, &ssconvert_one_file_per_sheet,
		N_("Export a file for each sheet if the exporter only supports one sheet at a time"),
		NULL
	},

	{
		"recalc", 0,
		0, G_OPTION_ARG_NONE, &ssconvert_recalc,
		N_("Recalculate all cells before writing the result"),
		NULL
	},


	/* ---------------------------------------- */

	/* For now these are for INTERNAL GNUMERIC USE ONLY.  */
	{
		"export-range", 0,
		G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &ssconvert_range,
		N_("The range to export"),
		NULL
	},

	{
		"goal-seek", 0,
		G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING_ARRAY, &ssconvert_goal_seek,
		N_("Goal seek areas"),
		NULL
	},

	{
		"solve", 0,
		G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &ssconvert_solve,
		N_("Run the solver"),
		NULL
	},

	/* ---------------------------------------- */

	{ NULL }
};

static void
setup_range (GObject *obj, const char *key, Workbook *wb, const char *rtxt)
{
	GnmParsePos pp;
	const char *end;
	GnmRangeRef rr;

	pp.wb = wb;
	pp.sheet = workbook_sheet_by_index (wb, 0);
	pp.eval.col = 0;
	pp.eval.row = 0;

	end = rangeref_parse (&rr, rtxt, &pp, gnm_conventions_default);
	if (!end || end == rtxt || *end != 0) {
		g_printerr ("Invalid range specified.\n");
		exit (1);
	}

	g_object_set_data_full (obj, key,
				g_memdup (&rr, sizeof (rr)),
				g_free);
}

static int
handle_export_options (GOFileSaver *fs, GODoc *doc)
{
	guint sig = g_signal_lookup ("set-export-options",
				     G_TYPE_FROM_INSTANCE (fs));

	if (!ssconvert_export_options)
		return 0;

	if (g_signal_handler_find (fs, G_SIGNAL_MATCH_ID,
				   sig, 0, NULL, NULL, NULL)) {
		GError *err = NULL;
		gboolean fail =
			go_file_saver_set_export_options
			(fs, doc,
			 ssconvert_export_options,
			 &err);

		if (fail) {
			g_printerr ("ssconvert: %s\n", err
				    ? err->message
				    : _("Cannot parse export options."));
			return 1;
		}

		return 0;
	} else {
		g_printerr (_("The file saver does not take options"));
		return 1;
	}
}


typedef GList *(*get_them_f)(void);
typedef gchar const *(*get_desc_f)(void *);

static void
list_them (get_them_f get_them,
	   get_desc_f get_his_id,
	   get_desc_f get_his_description)
{
	GList *ptr;
	int len = 0;

	for (ptr = (*get_them) (); ptr ; ptr = ptr->next) {
		char const *id = (*get_his_id) (ptr->data);
		int tmp = strlen (id);
		if (len < tmp)
			len = tmp;
	}

	g_printerr ("%-*s | %s\n", len,
		    /* Translate these? */
		    "ID",
		    "Description");
	for (ptr = (*get_them) (); ptr ; ptr = ptr->next) {
		char const *id = (*get_his_id) (ptr->data);
		g_printerr ("%-*s | %s\n", len,
			    id,
			    (*get_his_description) (ptr->data));
	}
}



static void
run_solver (Sheet *sheet, WorkbookView *wbv)
{
	GnmSolverParameters *params = sheet->solver_parameters;
	GError *err = NULL;
	WorkbookControl *wbc;
	GnmSolver *sol = NULL;

	wbc = g_object_new (WORKBOOK_CONTROL_TYPE, NULL);
	wb_control_set_view (wbc, wbv, NULL);

	/* Pick a functional algorithm.  */
	if (!gnm_solver_factory_functional (params->options.algorithm,
					    NULL)) {
		GSList *l;
		for (l = gnm_solver_db_get (); l; l = l->next) {
			GnmSolverFactory *factory = l->data;
			if (params->options.model_type != factory->type)
				continue;
			if (gnm_solver_factory_functional (factory, NULL)) {
				gnm_solver_param_set_algorithm (params,
								factory);
				break;
			}
		}
	}

	if (!gnm_solver_param_valid (params, &err))
		goto done;

	sol = params->options.algorithm
		? gnm_solver_factory_create (params->options.algorithm, params)
		: NULL;
	if (!sol) {
		g_set_error (&err, go_error_invalid (), 0,
			     _("Failed to create solver"));
		goto done;
	}

	if (!gnm_solver_start (sol, wbc, &err))
		goto done;

	while (!gnm_solver_finished (sol)) {
		g_main_context_iteration (NULL, TRUE);
	}

	if (sol->status != GNM_SOLVER_STATUS_DONE) {
		g_set_error (&err, go_error_invalid (), 0,
			     _("Solver ran, but failed"));
		goto done;
	}

	gnm_solver_store_result (sol);

 done:
	if (sol)
		g_object_unref (sol);
	if (err) {
		g_printerr (_("Solver: %s\n"), err->message);
		g_error_free (err);
	}
}


static int
convert (char const *inarg, char const *outarg, char const *mergeargs[],
	 GOCmdContext *cc)
{
	int res = 0;
	GOFileSaver *fs = NULL;
	GOFileOpener *fo = NULL;
	char *infile = go_shell_arg_to_uri (inarg);
	char *outfile = outarg ? go_shell_arg_to_uri (outarg) : NULL;

	if (ssconvert_export_id != NULL) {
		fs = go_file_saver_for_id (ssconvert_export_id);
		if (fs == NULL) {
			res = 1;
			g_printerr (_("Unknown exporter '%s'.\n"
				      "Try --list-exporters to see a list of possibilities.\n"),
				    ssconvert_export_id);
			goto out;
		} else if (outfile == NULL &&
			   go_file_saver_get_extension (fs) != NULL) {
			char const *ext = gsf_extension_pointer (infile);
			if (*infile) {
				GString *res = g_string_new (NULL);
				g_string_append_len (res, infile, ext - infile);
				g_string_append (res, go_file_saver_get_extension(fs));
				outfile = g_string_free (res, FALSE);
			}
		}
	} else {
		if (outfile != NULL) {
			fs = go_file_saver_for_file_name (outfile);
			if (fs == NULL) {
				res = 2;
				g_printerr (_("Unable to guess exporter to use for '%s'.\n"
					      "Try --list-exporters to see a list of possibilities.\n"),
					    outfile);
				goto out;
			}
			g_print ("Using exporter %s\n",
				 go_file_saver_get_id (fs));
		}
	}

	if (outfile == NULL) {
		g_printerr (_("An output file name or an explicit export type is required.\n"
			      "Try --list-exporters to see a list of possibilities.\n"));
		res = 1;
		goto out;
	}

	if (ssconvert_import_id != NULL) {
		fo = go_file_opener_for_id (ssconvert_import_id);
		if (fo == NULL) {
			res = 1;
			g_printerr (_("Unknown importer '%s'.\n"
				      "Try --list-importers to see a list of possibilities.\n"),
				    ssconvert_import_id);
			goto out;
		}
	}

	if (fs != NULL) {
		GOIOContext *io_context = go_io_context_new (cc);
		WorkbookView *wbv;
		if (mergeargs==NULL) {
			wbv = wb_view_new_from_uri (infile, fo,
						    io_context,
						    ssconvert_import_encoding);
		} else {
			wbv = workbook_view_new (NULL);
		}

		if (wbv == NULL || go_io_error_occurred (io_context)) {
			go_io_error_display (io_context);
			res = 1;
		} else {
			Workbook *wb = wb_view_get_workbook (wbv);
			Sheet *sheet = wb_view_cur_sheet (wbv);

			res = handle_export_options (fs, GO_DOC (wb));
			if (res) {
				g_object_unref (wb);
				goto out;
			}

			if (mergeargs!=NULL) {
					goto out;
			}

			if (ssconvert_goal_seek) {
				int i;

				for (i = 0; ssconvert_goal_seek[i]; i++) {
					setup_range (G_OBJECT (sheet),
						     "ssconvert-goal-seek",
						     wb,
						     ssconvert_goal_seek[i]);
				}
			}

			if (ssconvert_solve)
				run_solver (sheet, wbv);

			if (ssconvert_recalc)
				workbook_recalc_all (wb);
			else
				workbook_recalc (wb);

			if (ssconvert_range)
				setup_range (G_OBJECT (wb),
					     "ssconvert-range",
					     wb,
					     ssconvert_range);
			else if (workbook_sheet_count (wb) > 1 &&
				 go_file_saver_get_save_scope (fs) != GO_FILE_SAVE_WORKBOOK) {
				if (ssconvert_one_file_per_sheet) {
					GSList *ptr;
					GString *s;
					char *tmpfile;
					int idx = 0;
					res = 0;

					for (ptr = workbook_sheets(wb); ptr && !res; ptr = ptr->next, idx++) {
						wb_view_sheet_focus(wbv, (Sheet *)ptr->data);
						s = g_string_new (outfile);
						g_string_append_printf(s, ".%d", idx);
						tmpfile = g_string_free (s, FALSE);
						res = !wb_view_save_as (wbv, fs, tmpfile, cc);
						g_free(tmpfile);
					}
					goto printing_done;
				} else
					g_printerr (_("Selected exporter (%s) does not support saving multiple sheets in one file.\n"
						      "Only the current sheet will be saved.\n"),
						    go_file_saver_get_id (fs));
			}
			res = !wb_view_save_as (wbv, fs, outfile, cc);
		printing_done:
			g_object_unref (wb);
		}
		g_object_unref (io_context);
	}

 out:
	g_free (infile);
	g_free (outfile);

	return res;
}

int
main1(int argc, char const **argv)
{
	GOErrorInfo	*plugin_errs;
	int		 res = 0;
	GOCmdContext	*cc;
	GOptionContext *ocontext;
	GError *error = NULL;

	/* No code before here, we need to init threads */
	argv = gnm_pre_parse_init (argc, argv);

	ocontext = g_option_context_new ("INFILE [OUTFILE]");
	//g_option_context_add_main_entries (ocontext, ssconvert_options, GETTEXT_PACKAGE);
	//g_option_context_add_group	  (ocontext, gnm_get_option_group ());
	g_option_context_parse (ocontext, &argc, (char ***)&argv, &error);
	g_option_context_free (ocontext);

	if ( error ) {
		g_printerr (_("%s\nRun '%s --help' to see a full list of available command line options.\n"),
			    error->message, argv[0]);
		g_error_free (error);
		return 1;
	}

	if ( ssconvert_show_version ) {
		g_print (_("ssconvert version '%s'\ndatadir := '%s'\nlibdir := '%s'\n"),
			 GNM_VERSION_FULL, gnm_sys_data_dir (), gnm_sys_lib_dir ());
		return 0;
	}

	gnm_init ();

	cc = cmd_context_stderr_new ();
	gnm_plugins_init (GO_CMD_CONTEXT (cc));
	go_plugin_db_activate_plugin_list (
		go_plugins_get_available_plugins (), &plugin_errs);
	if (plugin_errs) {
		/* FIXME: What do we want to do here? */
		go_error_info_free (plugin_errs);
	}

	if (ssconvert_list_exporters)
		list_them (&go_get_file_savers,
			   (get_desc_f) &go_file_saver_get_id,
			   (get_desc_f) &go_file_saver_get_description);
	else if (ssconvert_list_importers)
		list_them (&go_get_file_openers,
			   (get_desc_f) &go_file_opener_get_id,
			   (get_desc_f) &go_file_opener_get_description);
	else if (ssconvert_merge_target!=NULL && argc>=3) {
		res = convert (argv[1], ssconvert_merge_target, argv+1, cc);
	} else if (argc == 2 || argc == 3) {
		res = convert (argv[1], argv[2], NULL, cc);
	} else {
		g_printerr (_("Usage: %s [OPTION...] %s\n"),
			    g_get_prgname (),
			    _("INFILE [OUTFILE]"));
		res = 1;
	}

	g_object_unref (cc);
	gnm_shutdown ();
	gnm_pre_parse_shutdown ();

	return res;
}
