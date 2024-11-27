# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.
file(GLOB board_source "board/*.c"
			"board/gpio/*.c"
			)


				
idf_component_register(
    SRCS ${board_source}
    	 main.c
    	 		 # list the source files of this component
    INCLUDE_DIRS   "board" # optional, add here public include directories

    EMBED_FILES   # "html_source/index.html"
    			  # "html_source/img_avatar2.png"
    			  # "html_source/chart.html"
    			  # "html_source/highcharts.js"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)