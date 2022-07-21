#ifndef PLUGINNAME_H
#define PLUGINNAME_H

#define PLUGIN_NAME "aeindus-custom"

#define blog(level, msg, ...) blog(level, "[" PLUGIN_NAME "] " msg, ##__VA_ARGS__)

#endif // PLUGINNAME_H
