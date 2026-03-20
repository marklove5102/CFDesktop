/**
 * @file    desktop/main/path/desktop_main_path_resolvers.h
 * @brief   Provides path resolution for desktop main directories and resources.
 *
 * Contains DesktopMainPathProvider class for managing and resolving paths
 * to standard desktop directories (Home, Desktop, Documents, etc.).
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup desktop_main
 */

#pragma once
#include "base/singleton/simple_singleton.hpp"
#include <QString>
#include <cstdint>

namespace cf::desktop::path {

/**
 * @brief  Provider for desktop main directory paths.
 *
 * Manages resolution of standard desktop directory paths including Home,
 * Desktop, Documents, Downloads, Music, Pictures, Videos, Apps, and Runtime.
 *
 * @ingroup desktop_main
 */
class DesktopMainPathProvider : public cf::SimpleSingleton<DesktopMainPathProvider> {
  public:
    /**
     * @brief  Constructs the path provider with a desktop root directory.
     *
     * @param[in] desktop_active_root Pointer to the desktop active root path.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_main
     */
    DesktopMainPathProvider(const char* desktop_active_root);

    /**
     * @brief  Standard desktop path types.
     *
     * Defines enumeration values for common desktop directory types.
     *
     * @ingroup desktop_main
     */
    enum class PathType {
        Home,      ///< User home directory.
        Desktop,   ///< Desktop folder.
        Documents, ///< Documents folder.
        Downloads, ///< Downloads folder.
        Music,     ///< Music folder.
        Pictures,  ///< Pictures folder.
        Videos,    ///< Videos folder.
        Apps,      ///< Applications folder.
        Runtime    ///< Runtime directory.
    };

    static constexpr const uint8_t PathTypeCnt = static_cast<uint8_t>(PathType::Runtime) + 1;

    /**
     * @brief  String constants for path component names.
     *
     * Provides static string representations of path type names.
     *
     * @ingroup desktop_main
     */
    struct PathComponents {
        static constexpr const char* Home = "Home";
        static constexpr const char* Desktop = "Desktop";
        static constexpr const char* Documents = "Documents";
        static constexpr const char* Downloads = "Downloads";
        static constexpr const char* Music = "Music";
        static constexpr const char* Pictures = "Pictures";
        static constexpr const char* Videos = "Videos";
        static constexpr const char* Apps = "Apps";
        static constexpr const char* Runtime = "Runtime";
    };

    /**
     * @brief   Checks and creates missing path directories.
     *
     * Verifies that all required path directories exist. Creates any
     * missing directories.
     *
     * @return  true if all paths are valid or created successfully,
     *          false otherwise.
     * @throws  None
     * @note    None
     * @warning None
     * @since   N/A
     * @ingroup desktop_main
     */
    bool init();

    /**
     * @brief   Resolves the absolute path for a given path type.
     *
     * Returns the absolute path string for the specified desktop path type.
     *
     * @param[in] p The PathType enum value to resolve.
     * @return      QString containing the absolute path.
     * @throws      None
     * @note        None
     * @warning     None
     * @since       N/A
     * @ingroup     desktop_main
     */
    QString absolutePath(const PathType p);

  private:
    /// @brief View of the root directory path. Ownership: observer.
    std::string_view root{};
};
} // namespace cf::desktop::path