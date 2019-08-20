<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_project_file>
	<FileVersion major="1" minor="6" />
	<Project>
		<Option title="dclubd" />
		<Option pch_mode="2" />
		<Option compiler="gcc" />
		<Build>
			<Target title="Debug">
				<Option output="bin/Debug/dclubd" prefix_auto="1" extension_auto="1" />
				<Option object_output="obj/Debug/" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Option parameters="-F " />
				<Compiler>
					<Add option="-g" />
				</Compiler>
			</Target>
			<Target title="Release">
				<Option output="bin/Release/dclubd" prefix_auto="1" extension_auto="1" />
				<Option object_output="obj/Release/" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Compiler>
					<Add option="-O2" />
				</Compiler>
				<Linker>
					<Add option="-s" />
				</Linker>
			</Target>
		</Build>
		<Compiler>
			<Add option="-Wall" />
			<Add option="-DDEBUG" />
			<Add option="-D_REENTERANT" />
			<Add option="-DPSEUDO_TELNET" />
		</Compiler>
		<Linker>
			<Add library="psqlmif" />
			<Add library="ctrlauth" />
			<Add directory="../../lib/control/auth" />
			<Add directory="../../lib" />
		</Linker>
		<Unit filename="../../lib/control/ctrl.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/control/ctrl_cmd.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/control/ctrl_data.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/control/ctrl_info.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/domino/do_alias.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/domino/do_db.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/domino/do_misc.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/domino/do_param.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/domino/domino.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/amem.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/csv_util.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/list.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/log.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/mask.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/proc_state.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/scanner.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/service.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/thread.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="../../lib/misc/util.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="dclub.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="dclubd.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="dclubinfo.c">
			<Option compilerVar="CC" />
		</Unit>
		<Unit filename="option.c">
			<Option compilerVar="CC" />
		</Unit>
		<Extensions>
			<code_completion />
			<debugger />
		</Extensions>
	</Project>
</CodeBlocks_project_file>
