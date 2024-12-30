#include <Engine.h>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const char* LOG_START_MESSAGE = R"(
.--------------------------------------------------------------.
|                        Vector 3D                             |
|                        -----------                           |
|                        A C++ game engine                     |
|                        ----------------                      |
'--------------------------------------------------------------'
)";

int main(int argc, char* argv[]) {
    // Parse command line arguments

    // Default logging options
    plog::Severity severity = plog::verbose;
    const char* log_file_name = "vector3d_log.txt";
    size_t log_file_max_size = 1000000; // 1 MB
    int log_file_max_count = 5;

    for (int i = 1; i < argc; i++) {
        // Parse logging options
        if (strcmp(argv[i], "--log-file") == 0 && i + 1 < argc) {
            log_file_name = argv[++i];
        } else if (strcmp(argv[i], "--log-size") == 0 && i + 1 < argc) {
            log_file_max_size = std::stoul(argv[++i]);
        } else if (strcmp(argv[i], "--log-count") == 0 && i + 1 < argc) {
            log_file_max_count = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--log-level") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[i + 1], "none") == 0) {
                    severity = plog::none;
                    i++;
                } else if (strcmp(argv[i + 1], "fatal") == 0) {
                    severity = plog::fatal;
                    i++;
                } else if (strcmp(argv[i + 1], "error") == 0) {
                    severity = plog::error;
                    i++;
                } else if (strcmp(argv[i + 1], "warning") == 0) {
                    severity = plog::warning;
                    i++;
                } else if (strcmp(argv[i + 1], "info") == 0) {
                    severity = plog::info;
                    i++;
                } else if (strcmp(argv[i + 1], "debug") == 0) {
                    severity = plog::debug;
                    i++;
                } else if (strcmp(argv[i + 1], "verbose") == 0) {
                    severity = plog::verbose;
                    i++;
                }
            }
        }
            
    }

    // Initialize the logger
    // log to file and console
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(log_file_name, log_file_max_size, log_file_max_count);
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(severity, &fileAppender).addAppender(&consoleAppender);

    PLOGN << LOG_START_MESSAGE;

    // Create the 3D engine instance
    v3d::Engine app(WIDTH, HEIGHT);

    try {
        // Initialize and run the 3D engine
        app.run();
    } catch (const std::exception &e) {
        // std::cerr << e.what() << std::endl;
        PLOGE << "Exception: " << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}