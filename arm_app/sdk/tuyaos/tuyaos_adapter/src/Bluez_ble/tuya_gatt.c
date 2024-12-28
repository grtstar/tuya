/**
 * Copy from gatt-service.c
 */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "uni_log.h"
#include "tuya_gatt.h"
#if defined(TKL_BLUEZ_BLE_TEST) && TKL_BLUEZ_BLE_TEST == 1
#include <glib.h>
#include <dbus/dbus.h>
#include "gdbus/gdbus.h"

#define ERROR_INTERFACE        "org.bluez.Error"

#define DEVICE_INFACE          "org.bluez.Device1"
#define GATT_MGR_IFACE         "org.bluez.GattManager1"
#define GATT_SERVICE_IFACE     "org.bluez.GattService1"
#define GATT_CHR_IFACE         "org.bluez.GattCharacteristic1"
#define GATT_DESCRIPTOR_IFACE  "org.bluez.GattDescriptor1"

#define PATH_PREFIX    "/com/tuya"

struct characteristic
{
    char *service;
    char *uuid;
    char *path;
    uint8_t *value;
    int vlen;
    uint8_t props;
};

struct descriptor
{
    struct characteristic *chr;
    char *uuid;
    char *path;
    uint8_t *value;
    int vlen;
    uint8_t props;
};

static DBusConnection *connection = NULL;
static GDBusClient *client = NULL;
static GSList *chr_list;

static void (*__gatt_connect_event)(int status) = NULL;
static void (*__gatt_write_request_event)(uint16_t uuid, uint8_t *data, uint8_t len) = NULL;

static gboolean desc_get_uuid(const GDBusPropertyTable *property,
                              DBusMessageIter *iter, void *user_data)
{
    struct descriptor *desc = user_data;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &desc->uuid);

    return TRUE;
}

static gboolean desc_get_characteristic(const GDBusPropertyTable *property,
                                        DBusMessageIter *iter, void *user_data)
{
    struct descriptor *desc = user_data;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_OBJECT_PATH,
                                   &desc->chr->path);

    return TRUE;
}

static bool desc_read(struct descriptor *desc, DBusMessageIter *iter)
{
    DBusMessageIter array;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                     DBUS_TYPE_BYTE_AS_STRING, &array);

    if (desc->vlen && desc->value)
        dbus_message_iter_append_fixed_array(&array, DBUS_TYPE_BYTE,
                                             &desc->value, desc->vlen);

    dbus_message_iter_close_container(iter, &array);

    return true;
}

static gboolean desc_get_value(const GDBusPropertyTable *property,
                               DBusMessageIter *iter, void *user_data)
{
    struct descriptor *desc = user_data;

    PR_DEBUG("Descriptor(%s): Get(\"Value\")", desc->uuid);

    return desc_read(desc, iter);
}

static void desc_write(struct descriptor *desc, const uint8_t *value, int len)
{
    g_free(desc->value);
    desc->value = g_memdup(value, len);
    desc->vlen = len;

    g_dbus_emit_property_changed(connection, desc->path,
                                 GATT_DESCRIPTOR_IFACE, "Value");
}

static int parse_value(DBusMessageIter *iter, const uint8_t **value, int *len)
{
    DBusMessageIter array;

    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
        return -EINVAL;

    dbus_message_iter_recurse(iter, &array);
    dbus_message_iter_get_fixed_array(&array, value, len);

    return 0;
}

static void desc_set_value(const GDBusPropertyTable *property,
                           DBusMessageIter *iter,
                           GDBusPendingPropertySet id, void *user_data)
{
    struct descriptor *desc = user_data;
    const uint8_t *value;
    int len;

    PR_DEBUG("Descriptor(%s): Set(\"Value\", ...)", desc->uuid);

    if (parse_value(iter, &value, &len))
    {
        PR_ERR("Invalid value for Set('Value'...)");
        g_dbus_pending_property_error(id,
                                      ERROR_INTERFACE ".InvalidArguments",
                                      "Invalid arguments in method call");
        return;
    }

    desc_write(desc, value, len);

    g_dbus_pending_property_success(id);
}

static gboolean desc_get_props(const GDBusPropertyTable *property,
                               DBusMessageIter *iter, void *data)
{
    struct descriptor *desc = data;
    DBusMessageIter array;
    int i;
    char *prop = NULL;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                     DBUS_TYPE_STRING_AS_STRING, &array);

    if (desc->props & LE_GATT_CHR_PROP_WRITE_NO_RSP) {
        prop = "write-without-response";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (desc->props & LE_GATT_CHR_PROP_WRITE) {
        prop = "write";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (desc->props & LE_GATT_CHR_PROP_NOTIFY) {
        prop = "notify";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (desc->props & LE_GATT_CHR_PROP_INDICATE) {
        prop = "indicate";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (desc->props & LE_GATT_CHR_PROP_READ) {
        prop = "read";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }

    dbus_message_iter_close_container(iter, &array);

    return TRUE;
}

static const GDBusPropertyTable desc_properties[] = {
    {"UUID", "s", desc_get_uuid},
    {"Characteristic", "o", desc_get_characteristic},
    {"Value", "ay", desc_get_value, desc_set_value, NULL},
    {"Flags", "as", desc_get_props, NULL, NULL},
    {}};

static gboolean chr_get_uuid(const GDBusPropertyTable *property,
                             DBusMessageIter *iter, void *user_data)
{
    struct characteristic *chr = user_data;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &chr->uuid);

    return TRUE;
}

static gboolean chr_get_service(const GDBusPropertyTable *property,
                                DBusMessageIter *iter, void *user_data)
{
    struct characteristic *chr = user_data;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_OBJECT_PATH,
                                   &chr->service);

    return TRUE;
}

static bool chr_read(struct characteristic *chr, DBusMessageIter *iter)
{
    DBusMessageIter array;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                     DBUS_TYPE_BYTE_AS_STRING, &array);

    dbus_message_iter_append_fixed_array(&array, DBUS_TYPE_BYTE,
                                         &chr->value, chr->vlen);

    dbus_message_iter_close_container(iter, &array);

    return true;
}

static gboolean chr_get_value(const GDBusPropertyTable *property,
                              DBusMessageIter *iter, void *user_data)
{
    struct characteristic *chr = user_data;

    PR_DEBUG("Characteristic(%s): Get(\"Value\")", chr->uuid);

    return chr_read(chr, iter);
}

static gboolean chr_get_props(const GDBusPropertyTable *property,
                              DBusMessageIter *iter, void *data)
{
    struct characteristic *chr = data;
    DBusMessageIter array;
    int i;
    char *prop = NULL;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                     DBUS_TYPE_STRING_AS_STRING, &array);

    if (chr->props & LE_GATT_CHR_PROP_WRITE_NO_RSP) {
        prop = "write-without-response";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (chr->props & LE_GATT_CHR_PROP_WRITE) {
        prop = "write";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (chr->props & LE_GATT_CHR_PROP_NOTIFY) {
        prop = "notify";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (chr->props & LE_GATT_CHR_PROP_INDICATE) {
        prop = "indicate";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }
    if (chr->props & LE_GATT_CHR_PROP_READ) {
        prop = "read";
        dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &prop);
    }

    dbus_message_iter_close_container(iter, &array);

    return TRUE;
}

static void chr_write(struct characteristic *chr, const uint8_t *value, int len)
{
    g_free(chr->value);
    chr->value = g_memdup(value, len);
    chr->vlen = len;

    g_dbus_emit_property_changed(connection, chr->path, GATT_CHR_IFACE,
                                 "Value");
}

static void chr_set_value(const GDBusPropertyTable *property,
                          DBusMessageIter *iter,
                          GDBusPendingPropertySet id, void *user_data)
{
    struct characteristic *chr = user_data;
    const uint8_t *value;
    int len;

    PR_DEBUG("Characteristic(%s): Set('Value', ...)", chr->uuid);

    if (!parse_value(iter, &value, &len))
    {
        PR_ERR("Invalid value for Set('Value'...)");
        g_dbus_pending_property_error(id,
                                      ERROR_INTERFACE ".InvalidArguments",
                                      "Invalid arguments in method call");
        return;
    }

    chr_write(chr, value, len);

    g_dbus_pending_property_success(id);
}

static const GDBusPropertyTable chr_properties[] = {
    {"UUID", "s", chr_get_uuid},
    {"Service", "o", chr_get_service},
    {"Value", "ay", chr_get_value, chr_set_value, NULL},
    {"Flags", "as", chr_get_props, NULL, NULL},
    {}};

static gboolean service_get_primary(const GDBusPropertyTable *property,
                                    DBusMessageIter *iter, void *user_data)
{
    dbus_bool_t primary = TRUE;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_BOOLEAN, &primary);

    return TRUE;
}

static gboolean service_get_uuid(const GDBusPropertyTable *property,
                                 DBusMessageIter *iter, void *user_data)
{
    const char *uuid = user_data;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &uuid);

    return TRUE;
}

static gboolean service_get_includes(const GDBusPropertyTable *property,
                                     DBusMessageIter *iter, void *user_data)
{
#if 0
    const char *uuid = user_data;
    char service_path[100] = {
        0,
    };
    DBusMessageIter array;
    char *p = NULL;

    snprintf(service_path, 100, "/service3");
    printf("Get Includes: %s\n", uuid);

    p = service_path;

    printf("Includes path: %s\n", p);

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                     DBUS_TYPE_OBJECT_PATH_AS_STRING, &array);

    dbus_message_iter_append_basic(&array, DBUS_TYPE_OBJECT_PATH,
                                   &p);

    snprintf(service_path, 100, "/service2");
    p = service_path;
    printf("Get Includes: %s\n", p);

    dbus_message_iter_append_basic(&array, DBUS_TYPE_OBJECT_PATH,
                                   &p);
    dbus_message_iter_close_container(iter, &array);
#endif

    return TRUE;
}

static gboolean service_exist_includes(const GDBusPropertyTable *property,
                                       void *user_data)
{
    return FALSE;
}

static const GDBusPropertyTable service_properties[] = {
    {"Primary", "b", service_get_primary},
    {"UUID", "s", service_get_uuid},
    {"Includes", "ao", service_get_includes, NULL,
     service_exist_includes},
    {}};

static void chr_iface_destroy(gpointer user_data)
{
    struct characteristic *chr = user_data;

    g_free(chr->uuid);
    g_free(chr->service);
    g_free(chr->value);
    g_free(chr->path);
    g_free(chr);
}

static void desc_iface_destroy(gpointer user_data)
{
    struct descriptor *desc = user_data;

    g_free(desc->uuid);
    g_free(desc->value);
    g_free(desc->path);
    g_free(desc);
}

static int parse_options(DBusMessageIter *iter, const char **device)
{
    DBusMessageIter dict;

    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
        return -EINVAL;

    dbus_message_iter_recurse(iter, &dict);

    while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY)
    {
        const char *key;
        DBusMessageIter value, entry;
        int var;

        dbus_message_iter_recurse(&dict, &entry);
        dbus_message_iter_get_basic(&entry, &key);

        dbus_message_iter_next(&entry);
        dbus_message_iter_recurse(&entry, &value);

        var = dbus_message_iter_get_arg_type(&value);
        if (strcasecmp(key, "device") == 0)
        {
            if (var != DBUS_TYPE_OBJECT_PATH)
                return -EINVAL;
            dbus_message_iter_get_basic(&value, device);
            PR_DEBUG("Device: %s", *device);
        }

        dbus_message_iter_next(&dict);
    }

    return 0;
}

static DBusMessage *chr_read_value(DBusConnection *conn, DBusMessage *msg,
                                   void *user_data)
{
    struct characteristic *chr = user_data;
    DBusMessage *reply;
    DBusMessageIter iter;
    const char *device;

    if (!dbus_message_iter_init(msg, &iter))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    if (parse_options(&iter, &device))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    reply = dbus_message_new_method_return(msg);
    if (!reply)
        return g_dbus_create_error(msg, DBUS_ERROR_NO_MEMORY,
                                   "No Memory");

    dbus_message_iter_init_append(reply, &iter);

    chr_read(chr, &iter);

    return reply;
}

static DBusMessage *chr_write_value(DBusConnection *conn, DBusMessage *msg,
                                    void *user_data)
{
    struct characteristic *chr = user_data;
    DBusMessageIter iter;
    const uint8_t *value;
    int len;
    const char *device;

    dbus_message_iter_init(msg, &iter);

    if (parse_value(&iter, &value, &len))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    if (parse_options(&iter, &device))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    chr_write(chr, value, len);

    if (__gatt_write_request_event) {
        __gatt_write_request_event((uint16_t)strtol(chr->uuid, NULL, 16), (uint8_t *)value, len);
    }

    return dbus_message_new_method_return(msg);
}

static DBusMessage *chr_start_notify(DBusConnection *conn, DBusMessage *msg,
                                     void *user_data)
{
    return g_dbus_create_error(msg, DBUS_ERROR_NOT_SUPPORTED,
                               "Not Supported");
}

static DBusMessage *chr_stop_notify(DBusConnection *conn, DBusMessage *msg,
                                    void *user_data)
{
    return g_dbus_create_error(msg, DBUS_ERROR_NOT_SUPPORTED,
                               "Not Supported");
}

static const GDBusMethodTable chr_methods[] = {
    {GDBUS_ASYNC_METHOD("ReadValue", GDBUS_ARGS({"options", "a{sv}"}),
                        GDBUS_ARGS({"value", "ay"}),
                        chr_read_value)},
    {GDBUS_ASYNC_METHOD("WriteValue", GDBUS_ARGS({"value", "ay"}, {"options", "a{sv}"}),
                        NULL, chr_write_value)},
    {GDBUS_ASYNC_METHOD("StartNotify", NULL, NULL, chr_start_notify)},
    {GDBUS_METHOD("StopNotify", NULL, NULL, chr_stop_notify)},
    {}};

static DBusMessage *desc_read_value(DBusConnection *conn, DBusMessage *msg,
                                    void *user_data)
{
    struct descriptor *desc = user_data;
    DBusMessage *reply;
    DBusMessageIter iter;
    const char *device;

    if (!dbus_message_iter_init(msg, &iter))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    if (parse_options(&iter, &device))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    reply = dbus_message_new_method_return(msg);
    if (!reply)
        return g_dbus_create_error(msg, DBUS_ERROR_NO_MEMORY,
                                   "No Memory");

    dbus_message_iter_init_append(reply, &iter);

    desc_read(desc, &iter);

    return reply;
}

static DBusMessage *desc_write_value(DBusConnection *conn, DBusMessage *msg,
                                     void *user_data)
{
    struct descriptor *desc = user_data;
    DBusMessageIter iter;
    const char *device;
    const uint8_t *value;
    int len;

    if (!dbus_message_iter_init(msg, &iter))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    if (parse_value(&iter, &value, &len))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    if (parse_options(&iter, &device))
        return g_dbus_create_error(msg, DBUS_ERROR_INVALID_ARGS,
                                   "Invalid arguments");

    desc_write(desc, value, len);

    return dbus_message_new_method_return(msg);
}

static const GDBusMethodTable desc_methods[] = {
    {GDBUS_ASYNC_METHOD("ReadValue", GDBUS_ARGS({"options", "a{sv}"}),
                        GDBUS_ARGS({"value", "ay"}),
                        desc_read_value)},
    {GDBUS_ASYNC_METHOD("WriteValue", GDBUS_ARGS({"value", "ay"}, {"options", "a{sv}"}),
                        NULL, desc_write_value)},
    {}};

static void register_app_reply(DBusMessage *reply, void *user_data)
{
    DBusError derr;

    dbus_error_init(&derr);
    dbus_set_error_from_message(&derr, reply);

    if (dbus_error_is_set(&derr))
        PR_DEBUG("RegisterApplication: %s", derr.message);
    else
        PR_DEBUG("RegisterApplication: OK");

    dbus_error_free(&derr);
}

static void register_app_setup(DBusMessageIter *iter, void *user_data)
{
    const char *path = "/";
    DBusMessageIter dict;

    dbus_message_iter_append_basic(iter, DBUS_TYPE_OBJECT_PATH, &path);

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, "{sv}", &dict);

    /* TODO: Add options dictionary */

    dbus_message_iter_close_container(iter, &dict);
}

static void register_app(GDBusProxy *proxy)
{
    if (!g_dbus_proxy_method_call(proxy, "RegisterApplication",
                                  register_app_setup, register_app_reply,
                                  NULL, NULL))
    {
        PR_ERR("Unable to call RegisterApplication");
        return;
    }
}

static void proxy_added_cb(GDBusProxy *proxy, void *user_data)
{
    const char *iface;

    iface = g_dbus_proxy_get_interface(proxy);

    if (g_strcmp0(iface, GATT_MGR_IFACE))
        return;

    register_app(proxy);
}

static void property_changed_cb(GDBusProxy *proxy, const char *name,
                    DBusMessageIter *iter, void *user_data)
{
    dbus_bool_t conn_status = FALSE;
    const char *interface = g_dbus_proxy_get_interface(proxy);
    const char *path = g_dbus_proxy_get_path(proxy);

    PR_DEBUG("property_changed, path: %s, iface: %s, name: %s", path, interface, name);

    if (!g_strcmp0(interface, DEVICE_INFACE)) {
        if (!g_strcmp0(name, "ServicesResolved")) {
            dbus_message_iter_get_basic(iter, &conn_status);
            if (__gatt_connect_event) {
                __gatt_connect_event(conn_status);
            }
        }
    }
}

int tuya_gatt_init(void)
{
    connection = g_dbus_setup_bus(DBUS_BUS_SYSTEM, NULL, NULL);

    if (!g_dbus_attach_object_manager(connection)) {
        PR_ERR("g_dbus_attach_object_manager error");
        return LE_COM_ERROR;
    }

    client = g_dbus_client_new(connection, "org.bluez", "/");

    g_dbus_client_set_proxy_handlers(client, proxy_added_cb, NULL, property_changed_cb,
                                     NULL);

    return LE_SUCCESS;
}

int tuya_gatt_register_service(uint16_t uuid)
{
    if (!connection) {
        PR_WARN("Connection not initialized");
        return LE_COM_ERROR;
    }

    char path[64] = {0};
    char *uuid_str = NULL;

    uuid_str = g_strdup_printf("%04x", uuid);
    g_snprintf(path, sizeof(path), "%s/service%s", PATH_PREFIX, uuid_str);
    if (!g_dbus_register_interface(connection, path, GATT_SERVICE_IFACE,
                                   NULL, NULL, service_properties,
                                   uuid_str, g_free))
    {
        PR_ERR("Couldn't register service interface");
        g_free(uuid_str);
        return LE_COM_ERROR;
    }

    return LE_SUCCESS;
}

void convert_uuid(const char *input_uuid, char *output_uuid)
{
    if (input_uuid == NULL)
        return;
    int j = 0;
    for (int i = 0; i < strlen(input_uuid); i++) {
        if (input_uuid[i] != '-') {
            output_uuid[j++] = input_uuid[i];
        }
    }
    output_uuid[j] = '\0';  // Null-terminate the output string
}

int tuya_gatt_register_characteristic(uint16_t svc_uuid, const uint8_t *chr_uuid, uint8_t props, uint16_t desc_uuid, uint8_t desc_props)
{
    if (!connection) {
        PR_WARN("Connection not initialized");
        return LE_COM_ERROR;
    }

    struct characteristic *chr = NULL;
    struct descriptor *desc = NULL;

    char chr_path[128] = {0};
    char svc_path[128] = {0};
    char desc_path[128] = {0};

    char svc_uuid_str[32]  = {0};
   char chr_uuid_str[40]  = {0};
    char desc_uuid_str[32] = {0};

    g_snprintf(svc_uuid_str, sizeof(svc_uuid_str), "%04x", svc_uuid);
  //  g_snprintf(chr_uuid_str, sizeof(chr_uuid_str), "%s", chr_uuid);

    printf("svc_uuid%04x\n", svc_uuid);
    printf("chr_uuid%s\n", chr_uuid);
    printf("props%u\n", props);

    convert_uuid(chr_uuid,chr_uuid_str);
    g_snprintf(svc_path, sizeof(svc_path), "%s/service%s", PATH_PREFIX, svc_uuid_str);
    g_snprintf(chr_path, sizeof(chr_path), "%s/service%s/characteristic%s", PATH_PREFIX, svc_uuid_str, chr_uuid_str);

    chr          = g_new0(struct characteristic, 1);
    chr->uuid    = g_strdup(chr_uuid);
    chr->props   = props;
    chr->service = g_strdup(svc_path);
    chr->path = g_strdup(chr_path);

    if (!g_dbus_register_interface(connection, chr->path, GATT_CHR_IFACE,
                                   chr_methods, NULL, chr_properties,
                                   chr, chr_iface_destroy))
    {
        PR_ERR("Couldn't register characteristic interface");
        chr_iface_destroy(chr);
        return LE_COM_ERROR;
    }

    chr_list = g_slist_append(chr_list, chr);

    if (!desc_uuid)
        return LE_SUCCESS;

    g_snprintf(desc_uuid_str, sizeof(desc_uuid_str), "%04x", desc_uuid);
    g_snprintf(desc_path, sizeof(desc_path), "%s/service%s/characteristic%s/descriptor%s", PATH_PREFIX, svc_uuid_str, chr_uuid_str, desc_uuid_str);

    desc = g_new0(struct descriptor, 1);
    desc->uuid = g_strdup(desc_uuid_str);
    desc->chr = chr;
    desc->props = desc_props;
    desc->path = g_strdup(desc_path);

    if (!g_dbus_register_interface(connection, desc->path,
                                   GATT_DESCRIPTOR_IFACE,
                                   desc_methods, NULL, desc_properties,
                                   desc, desc_iface_destroy))
    {
        PR_ERR("Couldn't register descriptor interface");
        g_dbus_unregister_interface(connection, chr->path,
                                    GATT_CHR_IFACE);

        desc_iface_destroy(desc);
        return LE_COM_ERROR;
    }

    return LE_SUCCESS;
}

int tuya_gatt_server_send_characteristic_notification(uint16_t uuid, uint8_t *data, uint8_t len)
{
    struct characteristic *chr = NULL;
    GSList *c = chr_list;

    while (c != NULL) {
        chr = (struct characteristic *)c->data;
        if (uuid == strtol(chr->uuid, NULL, 16)) {
            break;
        }
        c = c->next;
    }

    if (c) {
        chr_write(chr, data, len);
    }

    return LE_SUCCESS;
}

void tuya_gatt_register_connect_event(void(*cb)(int status))
{
    __gatt_connect_event = cb;
}

void tuya_gatt_register_write_req_event(void(*cb)(uint16_t uuid, uint8_t *data, uint8_t len))
{
    __gatt_write_request_event = cb;
}
#endif