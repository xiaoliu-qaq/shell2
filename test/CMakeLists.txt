#  CmakeLists for the testcases for ISH
cmake_host_system_information(RESULT host_name QUERY HOSTNAME)
message("-- _host_name variable is set to: " ${host_name})
set(HOME_DIR "$ENV{HOME}")

add_test(NAME BasicTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} basic.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	
add_test(NAME BasicErrors 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} basic-error.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	
add_test(NAME BuiltinTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} builtin.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME EnvironmentTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} environment.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME RedirectionTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} redirection.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME RedirectionErrors 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} redirection-error.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME CornerTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} corner.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME AdditionalTests 
         COMMAND shelltest -DPROMPT=${host_name}% 
                           -DSHELL=$<TARGET_FILE:ish> 
                           -DHOME=${HOME_DIR} additional.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
