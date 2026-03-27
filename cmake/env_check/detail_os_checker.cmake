# WSL Detection Module
# Sets IS_WSL variable (TRUE if running in WSL)

function(cf_detect_wsl)
    set(IS_WSL FALSE PARENT_SCOPE)

    # Method 1: WSL_DISTRO_NAME environment variable (most reliable)
    if(DEFINED ENV{WSL_DISTRO_NAME})
        set(IS_WSL TRUE PARENT_SCOPE)
        return()
    endif()

    # Method 2: Check kernel release for microsoft/wsl
    execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(KERNEL_RELEASE)
        string(TOLOWER "${KERNEL_RELEASE}" KERNEL_RELEASE_LOWER)
        if(KERNEL_RELEASE_LOWER MATCHES "microsoft|wsl")
            set(IS_WSL TRUE PARENT_SCOPE)
            return()
        endif()
    endif()

    # Method 3: Check /proc/version
    if(EXISTS "/proc/version")
        file(READ "/proc/version" PROC_VERSION LIMIT 1024)
        string(TOLOWER "${PROC_VERSION}" PROC_VERSION_LOWER)
        if(PROC_VERSION_LOWER MATCHES "microsoft|wsl")
            set(IS_WSL TRUE PARENT_SCOPE)
            return()
        endif()
    endif()
endfunction()
