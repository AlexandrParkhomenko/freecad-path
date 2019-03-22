include (CheckCXXSourceRuns)

# ================================================================================
# == Macros, mostly for special targets ==========================================

MACRO (fc_copy_sources target_name outpath)
	if(BUILD_VERBOSE_GENERATION)
		set(fc_details " (fc_copy_sources called from ${CMAKE_CURRENT_SOURCE_DIR})")
	else()
		set(fc_details "")
	endif()
	foreach(it ${ARGN})
		get_filename_component(infile ${it} ABSOLUTE)
		get_filename_component(outfile "${outpath}/${it}" ABSOLUTE)
		add_file_dependencies("${infile}" "${outfile}")
		ADD_CUSTOM_COMMAND(
			COMMAND   "${CMAKE_COMMAND}" -E copy "${infile}" "${outfile}"
			OUTPUT   "${outfile}"
			COMMENT "Copying ${infile} to ${outfile}${fc_details}"
			MAIN_DEPENDENCY "${infile}"
		)
	endforeach(it)
	ADD_CUSTOM_COMMAND(
		TARGET    ${target_name}
		DEPENDS   ${ARGN}
	)
ENDMACRO(fc_copy_sources)

MACRO (fc_copy_file_if_different inputfile outputfile)
    if (EXISTS ${inputfile})
        if (EXISTS ${outputfile})
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E compare_files ${inputfile}
                                                          ${outputfile}
                RESULT_VARIABLE DIFFERENT_FILES
                OUTPUT_QUIET
                ERROR_QUIET
            )

            if (DIFFERENT_FILES)
                execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${inputfile}"
                                                                   "${outputfile}")
            endif()
        else()
            execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${inputfile}"
                                                               "${outputfile}")
        endif()
    endif()
ENDMACRO(fc_copy_file_if_different)

MACRO (fc_target_copy_resource target_name inpath outpath)
# Macro to copy a list of files into a nested directory structure
# Arguments -
#   target_name - name of the target the files will be added to
#   inpath      - name of the source directory
#   outpath     - name of the destination directory
#   ARGN        - a list of relative file names that will be copied
#
#   If a relative file name is foo/bar.txt then the foo directory
#   part will be kept so that the destination file name will be
#   ${outpath}/foo/bar.txt
#
	if(BUILD_VERBOSE_GENERATION)
		set(fc_details " (fc_target_copy_resource called from ${CMAKE_CURRENT_SOURCE_DIR})")
	else()
		set(fc_details "")
	endif()
	foreach(it ${ARGN})
		get_filename_component(infile "${inpath}/${it}" ABSOLUTE)
		get_filename_component(outfile "${outpath}/${it}" ABSOLUTE)
		add_file_dependencies("${infile}" "${outfile}")
		ADD_CUSTOM_COMMAND(
			COMMAND   "${CMAKE_COMMAND}" -E copy "${infile}" "${outfile}"
			OUTPUT   "${outfile}"
			COMMENT "Copying ${infile} to ${outfile}${fc_details}"
			MAIN_DEPENDENCY "${infile}"
		)
	endforeach(it)
	ADD_CUSTOM_COMMAND(
		TARGET    ${target_name}
		DEPENDS   ${ARGN}
	)
ENDMACRO(fc_target_copy_resource)

MACRO (fc_target_copy_resource_flat target_name inpath outpath)
# Macro to copy a list of files into a flat directory structure
# Arguments -
#   target_name - name of the target the files will be added to
#   inpath      - name of the source directory
#   outpath     - name of the destination directory
#   ARGN        - a list of relative file names that will be copied
#
#   If a relative file name is foo/bar.txt then the foo directory
#   part will be removed so that the destination file name will be
#   ${outpath}/bar.txt
#
	if(BUILD_VERBOSE_GENERATION)
		set(fc_details " (fc_target_copy_resource_flat called from ${CMAKE_CURRENT_SOURCE_DIR})")
	else()
		set(fc_details "")
	endif()
	foreach(it ${ARGN})
		get_filename_component(infile "${inpath}/${it}" ABSOLUTE)
		get_filename_component(outfile "${it}" NAME)
		get_filename_component(outfile "${outpath}/${outfile}" ABSOLUTE)
		add_file_dependencies("${infile}" "${outfile}")
		ADD_CUSTOM_COMMAND(
			COMMAND   "${CMAKE_COMMAND}" -E copy "${infile}" "${outfile}"
			OUTPUT    "${outfile}"
			COMMENT "Copying ${infile} to ${outfile}${fc_details}"
			MAIN_DEPENDENCY "${infile}"
		)
	endforeach(it)
	ADD_CUSTOM_COMMAND(
		TARGET    ${target_name}
		DEPENDS   ${ARGN}
	)
ENDMACRO(fc_target_copy_resource_flat)

# It would be a bit cleaner to generate these files in ${CMAKE_CURRENT_BINARY_DIR}

macro(generate_from_xml BASE_NAME)
    set(TOOL_PATH "${CMAKE_SOURCE_DIR}/src/Tools/generate.py")
    file(TO_NATIVE_PATH "${TOOL_PATH}" TOOL_NATIVE_PATH)
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.xml" SOURCE_NATIVE_PATH)

    set(SOURCE_CPP_PATH "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.cpp" )
    
    # BASE_NAME may include also a path name
    GET_FILENAME_COMPONENT(OUTPUT_PATH "${SOURCE_CPP_PATH}" PATH)
    file(TO_NATIVE_PATH "${OUTPUT_PATH}" OUTPUT_NATIVE_PATH)
    if(NOT EXISTS "${SOURCE_CPP_PATH}")
        # assures the source files are generated at least once
        message(STATUS "${SOURCE_CPP_PATH}")
        execute_process(COMMAND "${Python3_EXECUTABLE}" "${TOOL_NATIVE_PATH}" --outputPath "${OUTPUT_NATIVE_PATH}" "${SOURCE_NATIVE_PATH}"
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif()
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.h" "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.cpp"
        COMMAND ${Python3_EXECUTABLE} "${TOOL_NATIVE_PATH}" --outputPath "${OUTPUT_NATIVE_PATH}" ${BASE_NAME}.xml
        MAIN_DEPENDENCY "${BASE_NAME}.xml"
        DEPENDS
        "${CMAKE_SOURCE_DIR}/src/Tools/generateTemplates/templateClassPyExport.py"
        "${TOOL_PATH}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Building ${BASE_NAME}.h/.cpp out of ${BASE_NAME}.xml"
    )
endmacro(generate_from_xml)

macro(generate_from_py BASE_NAME OUTPUT_FILE)
		set(TOOL_PATH "${CMAKE_SOURCE_DIR}/src/Tools/PythonToCPP.py")
		file(TO_NATIVE_PATH "${TOOL_PATH}" TOOL_NATIVE_PATH)
		file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.py" SOURCE_NATIVE_PATH)
		add_custom_command(
		 		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}"
		 		COMMAND "${Python3_EXECUTABLE}" "${TOOL_NATIVE_PATH}" "${SOURCE_NATIVE_PATH}" "${OUTPUT_FILE}"
				MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.py"
				DEPENDS "${TOOL_PATH}"
				WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
				COMMENT "Building files out of ${BASE_NAME}.py")
endmacro(generate_from_py)


# generates the ui -> cpp h files
#macro(qt4_wrap_ui infiles )
#
#endmacro(qt4_wrap_ui)


# Macro to replace all the binary output locations.  Takes 2 optional parameters.
# ${ARGVN} is zero based so the 3rd element is ${ARGV2}.  When the 3rd element is missing,
# Runtime and Lib directories default to /bin and /lib.  When present, the 3rd element
# specifies both Runtime and Lib directories.  4th specifies linux install path.
MACRO(SET_BIN_DIR ProjectName OutputName)
    set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME ${OutputName})
    if(${ARGC} GREATER 2)
        # VS_IDE (and perhaps others) make Release and Debug subfolders.  This removes them.
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}${ARGV2})
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}${ARGV2})
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}${ARGV2})
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}${ARGV2})
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}${ARGV2})
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}${ARGV2})
    else(${ARGC} GREATER 2)
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/bin)
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
        set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin)
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
        set_target_properties(${ProjectName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/lib)
    endif(${ARGC} GREATER 2)

    if(WIN32)
        set_target_properties(${ProjectName} PROPERTIES DEBUG_OUTPUT_NAME ${OutputName}_d)
    else(WIN32)
        # FreeCADBase, SMDS, Driver, MEFISTO2 and area-native libs don't depend on parts from CMAKE_INSTALL_LIBDIR
        if(NOT ${ProjectName} MATCHES "^(FreeCADBase|SMDS|Driver|MEFISTO2|area-native)$")
            if(${ARGC} STREQUAL 4)
                set_property(TARGET ${ProjectName} APPEND PROPERTY INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}${ARGV3})
            else(${ARGC} STREQUAL 4)
                set_property(TARGET ${ProjectName} APPEND PROPERTY INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})
            endif()
        endif()
    endif(WIN32)
ENDMACRO(SET_BIN_DIR)

# Set python prefix & suffix together
MACRO(SET_PYTHON_PREFIX_SUFFIX ProjectName)
    set_target_properties(${ProjectName} PROPERTIES PREFIX "")
ENDMACRO(SET_PYTHON_PREFIX_SUFFIX)
