
SRCS = """do_ads_view.c
		do_application.c
		do_common_actions.c
		do_context.c
		do_config.c
		do_entry.c
		do_filter.c
		do_marshal.c
		do_message.c
		do_notebook.c
		do_setting_view.c
		do_tree_view.c
		do_utilx.c
		do_validate.c
		do_view.c
		do_view_actions.c
		do_window.c
		hig.c
		domino.c"""

for i in SRCS.split("\n"):
	n = i.strip(" \t")
	print("""\
		<link>
			<name>%s</name>
			<type>1</type>
			<location>$%%7BPARENT-1-PROJECT_LOC%%7D/src/bookcase/%s</location>
		</link>"""%(n,n))
